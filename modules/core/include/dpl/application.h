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
 * @file        application.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of MVC application support
 */
#ifndef DPL_APPLICATION_H
#define DPL_APPLICATION_H

#include <dpl/exception.h>
#include <dpl/framework_efl.h>
#include <dpl/framework_appcore.h>
#include <dpl/atomic.h>
#include <string>

namespace DPL
{
class Application
{
public:
    class Exception
    {
    public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, TooManyInstances)
        DECLARE_EXCEPTION_TYPE(Base, FrameworkError)
    };

private:
    static int app_create(void *data);
    static int app_terminate(void *data);
    static int app_pause(void *data);
    static int app_resume(void *data);
    static int app_reset(bundle *b, void *data);

protected:
    int m_argc;
    char **m_argv;
    std::string m_applicationName;

    bool m_mainWindowVisible;

    virtual void OnCreate();
    virtual void OnStart();
    virtual void OnStop();
    virtual void OnResume();
    virtual void OnPause();
    virtual void OnRelaunch();
    virtual void OnReset(bundle *b);
    virtual void OnTerminate();
    virtual void OnLowMemory();
    virtual void OnLowBattery();
    virtual void OnLanguageChanged();

public:
    Application(int argc, char **argv, const std::string &applicationName, bool showMainWindow = true);
    virtual ~Application();

    /**
     * @brief Execute application and start message processing
     */
    virtual int Exec();
    
    /*
     * @brief Sends quit message to application message loop
     */
    virtual void Quit();
};

class ApplicationExt : public Application
{
public:
    ApplicationExt(int argc, char **argv, const std::string &applicationName, bool showMainWindow = true);
    virtual ~ApplicationExt();

    /**
     * @brief Execute application and start message processing
     */
    virtual int Exec();

    /*
     * @brief Sends quit message to application message loop
     */
    virtual void Quit();
private:
    static DPL::Atomic m_useCount;
};

} // namespace DPL

#endif // DPL_APPLICATION_H
