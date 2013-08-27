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
 * @file    task_list.cpp
 * @author  Tomasz Iwanek (t.iwanek@samsung.com)
 * @version 1.0
 * @brief   Implementation file for task list
 */
#include <stddef.h>
#include <dpl/mutable_task_list.h>
#include <dpl/assert.h>

namespace DPL {
MutableTaskList::MutableTaskList() :
    m_running(false)
{
    m_currentTask = m_tasks.end();
}

MutableTaskList::~MutableTaskList()
{
    for (Tasks::iterator i = m_tasks.begin(); i != m_tasks.end(); ++i) {
        delete *i;
    }
}

void MutableTaskList::AddTask(Task *task)
{
    if(m_tasks.empty())
    {
        m_tasks.push_back(task);
        m_currentTask = m_tasks.begin();
    }
    else
    {
        m_tasks.push_back(task);
    }
}

bool MutableTaskList::NextStep()
{
    m_running = true;

    Assert(
        m_currentTask != m_tasks.end() &&
        "Task list is empty or all tasks done");

    bool result = (*m_currentTask)->NextStep();

    if (result) {
        return true;
    }

    return ++m_currentTask != m_tasks.end();
}

bool MutableTaskList::Abort()
{
    m_tasks.erase(m_currentTask, m_tasks.end());
    m_tasks.reverse();
    for (Tasks::iterator i = m_tasks.begin(); i != m_tasks.end();) {
        //If given task does not have any "abortSteps", remove it from the list
        if (!(*i)->Abort()) {
            delete *i;
            i = m_tasks.erase(i);
            continue;
        }
        ++i;
    }

    if (m_tasks.empty()) {
        return false;
    }

    m_currentTask = m_tasks.begin();

    return true;
}

size_t MutableTaskList::GetStepCount() const
{
    size_t count = 0;

    for (Tasks::const_iterator i = m_tasks.begin(); i != m_tasks.end(); ++i) {
        count += (*i)->GetStepCount();
    }

    return count;
}

} // namespace DPL
