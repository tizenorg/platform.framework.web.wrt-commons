/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        application.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of MVC application support
 */
#include <dpl/application.h>
#include <dpl/log/log.h>

namespace // anonymous
{
static DPL::Application *g_application = NULL;
} // namespace anonymous

namespace DPL
{
int Application::app_create(void *data)
{
    Application *This=static_cast<Application *>(data);
    This->OnCreate();
    return 0;
}

int Application::app_terminate(void *data)
{
    Application *This=static_cast<Application *>(data);
    This->OnTerminate();
    return 0;
}

int Application::app_pause(void *data)
{
    Application *This=static_cast<Application *>(data);
    This->OnPause();
    return 0;
}

int Application::app_resume(void *data)
{
    Application *This=static_cast<Application *>(data);
    This->OnResume();
    return 0;
}

int Application::app_reset(bundle *b, void *data)
{
    Application *This=static_cast<Application *>(data);
    This->OnReset(b);
    return 0;
}

Application::Application(int argc, char** argv,
                         const std::string& applicationName,
                         bool showMainWindow)
    : m_argc(argc),
      m_argv(argv),
      m_applicationName(applicationName),
      m_mainWindowVisible(showMainWindow)
{
    if (g_application != NULL)
        ThrowMsg(Exception::TooManyInstances, "Only single instance of Application allowed at one time!");

    g_application = this;
}

Application::~Application()
{
    g_application = NULL;
}

int Application::Exec()
{
    LogPedantic("Starting application framework...");

    struct appcore_ops ops;
    ops.create = app_create;
    ops.terminate = app_terminate;
    ops.pause = app_pause;
    ops.resume = app_resume;
    ops.reset = app_reset;
    ops.data=this;

    int result = appcore_efl_main(m_applicationName.c_str(), &m_argc, &m_argv, &ops);

    LogPedantic("Exited application framework");

    return result;
}

void Application::OnCreate()
{
    LogPedantic("On application create");
}

void Application::OnStart()
{
    LogPedantic("On application start");
}

void Application::OnStop()
{
    LogPedantic("On application stop");
}

void Application::OnResume()
{
    LogPedantic("On application resume");
}

void Application::OnPause()
{
    LogPedantic("On application pause");
}

void Application::OnRelaunch()
{
    LogPedantic("On application relaunch");
}

void Application::OnReset(bundle *b)
{
    (void)b;
    LogPedantic("On application reset");
}

void Application::OnTerminate()
{
    LogPedantic("On application terminate");
}

void Application::OnLowMemory()
{
    LogPedantic("On application low memory");
}

void Application::OnLowBattery()
{
    LogPedantic("On application low battery");
}

void Application::OnLanguageChanged()
{
    LogPedantic("On application language changed");
}

void Application::Quit()
{
    elm_exit();
}

DPL::Atomic ApplicationExt::m_useCount(0);

ApplicationExt::ApplicationExt(int argc, char** argv, const std::string& applicationName, bool showMainWindow) :
    Application(argc, argv, applicationName, showMainWindow)
{
}

ApplicationExt::~ApplicationExt()
{
}

int ApplicationExt::Exec()
{
    if (0 == m_useCount.CompareAndExchange(0, 1))
    {
        return Application::Exec();
    }
    else
    {
        elm_run();
    }
    return 0;
}

void ApplicationExt::Quit()
{
    elm_exit();
}
} // namespace DPL
