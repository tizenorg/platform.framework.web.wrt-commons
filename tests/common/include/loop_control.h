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
 * @brief       This file is the definitions of loop controlling utilities
 */


#ifndef LOOP_CONTROL_H_
#define LOOP_CONTROL_H_

namespace LoopControl
{

void init_loop(int argc, char *argv[]);
void wait_for_wrt_init();
void finish_wait_for_wrt_init();
void quit_loop();

void wrt_start_loop();
void wrt_end_loop();

void *abstract_window();

}

#endif /* LOOP_CONTROL_H_ */
