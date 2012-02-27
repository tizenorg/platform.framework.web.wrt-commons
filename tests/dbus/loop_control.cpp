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
 * @file        loop_control.cpp
 * @author      Jaroslaw Osmanski (j.osmanski@samsung.com)
 * @version     1.0
 * @brief       This is implementation of EFL version of loop control
 */

#include "loop_control.h"
#include <dpl/log/log.h>

#include <dpl/framework_efl.h>

#include <glib.h>
#include <glib-object.h>


namespace LoopControl
{
void init_loop(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    g_type_init();
    g_thread_init(NULL);

    LogInfo("Starting");
    elm_init(argc, argv);
}

void wait_for_wrt_init()
{
    ecore_main_loop_begin();
}

void finish_wait_for_wrt_init()
{
    ecore_main_loop_quit();
}

void quit_loop()
{
    elm_shutdown();
}

void wrt_start_loop()
{
    ecore_main_loop_begin();
}

void wrt_end_loop()
{
    ecore_main_loop_quit();
}

void *abstract_window()
{
    return elm_win_add(NULL, "hello", ELM_WIN_BASIC);
}

}//end of LoopControl namespace
