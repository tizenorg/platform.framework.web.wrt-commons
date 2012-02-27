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
 * @file        client-example.h
 * @author      Tomasz Swierczek (t.swierczek@samsung.com)
 * @version     1.0
 * @brief       Implementation for simple echo service DBus client.
 */

#include <iostream>
#include <string>

#include <dpl/dbus/dbus_client.h>

int main(int argc, char **argv)
{
    DPL::DBus::Client client("/path/to/object",
                        "org.tizen.EchoService",
                        "org.tizen.EchoInterface");
    std::string outstr;
    client.Call("echo", "Samsung Rocks! Hello World Test!", &outstr);
    std::cout << "Returned: " << outstr << std::endl;
    exit(0);
}

