/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @file        test_runner_child.cpp
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of test runner
 */
#include <stddef.h>
#include <dpl/test/test_runner.h>
#include <dpl/test/test_runner_child.h>
#include <dpl/test/test_results_collector.h>
#include <dpl/binary_queue.h>
#include <dpl/exception.h>
#include <dpl/scoped_free.h>
#include <dpl/foreach.h>
#include <dpl/colors.h>
#include <pcrecpp.h>
#include <algorithm>
#include <cstdio>
#include <memory.h>
#include <libgen.h>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dpl/utils/wrt_global_settings.h>

namespace {
const int PIPE_CLOSED = -1;
}

namespace DPL {
namespace Test {
class PipeWrapper : DPL::Noncopyable
{
  public:
    enum Usage {
        READONLY,
        WRITEONLY
    };

    enum Status {
        SUCCESS,
        TIMEOUT,
        ERROR
    };

    PipeWrapper()
    {
        if (-1 == pipe(m_pipefd)) {
            m_pipefd[0] = PIPE_CLOSED;
            m_pipefd[1] = PIPE_CLOSED;
        }
    }

    bool isReady()
    {
        return m_pipefd[0] != PIPE_CLOSED || m_pipefd[1] != PIPE_CLOSED;
    }

    void setUsage(Usage usage)
    {
        if (usage == READONLY) {
            closeHelp(1);
        }
        if (usage == WRITEONLY) {
            closeHelp(0);
        }
    }
    ~PipeWrapper()
    {
        closeHelp(0);
        closeHelp(1);
    }

    Status send(int code, std::string &message)
    {
        if (m_pipefd[1] == PIPE_CLOSED) {
            return ERROR;
        }

        std::ostringstream output;
        output << toBinaryString(code);
        output << toBinaryString(static_cast<int>(message.size()));
        output << message;

        std::string binary = output.str();
        int size = binary.size();

        if ((writeHelp(&size,
                       sizeof(int)) == ERROR) ||
            (writeHelp(binary.c_str(), size) == ERROR))
        {
            return ERROR;
        }
        return SUCCESS;
    }

    Status receive(int &code, std::string &data, time_t deadline)
    {
        if (m_pipefd[0] == PIPE_CLOSED) {
            return ERROR;
        }

        int size;
        Status ret;

        if ((ret = readHelp(&size, sizeof(int), deadline)) != SUCCESS) {
            return ret;
        }

        std::vector<char> buffer;
        buffer.resize(size);

        if ((ret = readHelp(&buffer[0], size, deadline)) != SUCCESS) {
            return ret;
        }

        try {
            DPL::BinaryQueue queue;
            queue.AppendCopy(&buffer[0], size);

            queue.FlattenConsume(&code, sizeof(int));
            queue.FlattenConsume(&size, sizeof(int));

            buffer.resize(size);

            queue.FlattenConsume(&buffer[0], size);
            data.assign(buffer.begin(), buffer.end());
        } catch (DPL::BinaryQueue::Exception::Base &e) {
            return ERROR;
        }
        return SUCCESS;
    }

    void closeAll()
    {
        closeHelp(0);
        closeHelp(1);
    }

  private:
    std::string toBinaryString(int data)
    {
        char buffer[sizeof(int)];
        memcpy(buffer, &data, sizeof(int));
        return std::string(buffer, buffer + sizeof(int));
    }

    void closeHelp(int desc)
    {
        if (m_pipefd[desc] != PIPE_CLOSED) {
            TEMP_FAILURE_RETRY(close(m_pipefd[desc]));
            m_pipefd[desc] = PIPE_CLOSED;
        }
    }

    Status writeHelp(const void *buffer, int size)
    {
        int ready = 0;
        const char *p = static_cast<const char *>(buffer);
        while (ready != size) {
            int ret = write(m_pipefd[1], &p[ready], size - ready);

            if (ret == -1 && (errno == EAGAIN || errno == EINTR)) {
                continue;
            }

            if (ret == -1) {
                closeHelp(1);
                return ERROR;
            }

            ready += ret;
        }
        return SUCCESS;
    }

    Status readHelp(void *buf, int size, time_t deadline)
    {
        int ready = 0;
        char *buffer = static_cast<char*>(buf);
        while (ready != size) {
            time_t wait = deadline - time(0);
            wait = wait < 1 ? 1 : wait;
            pollfd fds = { m_pipefd[0], POLLIN, 0 };

            int pollReturn = poll(&fds, 1, wait * 1000);

            if (pollReturn == 0) {
                return TIMEOUT; // Timeout
            }

            if (pollReturn < -1) {
                return ERROR;
            }

            int ret = read(m_pipefd[0], &buffer[ready], size - ready);

            if (ret == -1 && (errno == EAGAIN || errno == EINTR)) {
                continue;
            }

            if (ret == -1 || ret == 0) {
                closeHelp(0);
                return ERROR;
            }

            ready += ret;
        }
        return SUCCESS;
    }

    int m_pipefd[2];
};

void RunChildProc(TestRunner::TestCase procChild)
{
    PipeWrapper pipe;
    if (!pipe.isReady()) {
        throw TestRunner::TestFailed("Pipe creation failed");
    }

    pid_t pid = fork();

    if (pid == -1) {
        throw TestRunner::TestFailed("Child creation failed");
    }

    if (pid != 0) {
        // parent code
        pipe.setUsage(PipeWrapper::READONLY);

        int code;
        std::string message;

        int pipeReturn = pipe.receive(code, message, time(0) + 10);

        if (pipeReturn != PipeWrapper::SUCCESS) { // Timeout or reading error
            pipe.closeAll();
            kill(pid, SIGKILL);
        }

        int status;
        waitpid(pid, &status, 0);

        if (pipeReturn == PipeWrapper::TIMEOUT) {
            throw TestRunner::TestFailed("Timeout");
        }

        if (pipeReturn == PipeWrapper::ERROR) {
            throw TestRunner::TestFailed("Reading pipe error");
        }

        if (code == 0) {
            throw TestRunner::TestFailed(message);
        }
    } else {
        // child code

        // End Runner after current test
        TestRunnerSingleton::Instance().Terminate();

        int code = 1;
        std::string msg;

        bool allowLogs = TestRunnerSingleton::Instance().GetAllowChildLogs();

        close(0);            // stdin
        if (!allowLogs) {
            close(1);        // stdout
            close(2);        // stderr
        }

        pipe.setUsage(PipeWrapper::WRITEONLY);

        try {
            procChild();
        } catch (DPL::Test::TestRunner::TestFailed &e) {
            msg = e.GetMessage();
            code = 0;
        } catch (...) { // Pokemon Catch... cache them all...
            msg = "unhandled exeception";
            code = 0;
        }

        if (allowLogs) {
            close(1);   // stdout
            close(2);   // stderr
        }

        pipe.send(code, msg);
    }
}
} // namespace Test
} // namespace DPL
