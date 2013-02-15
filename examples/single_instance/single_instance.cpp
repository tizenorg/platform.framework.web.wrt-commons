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
 * @file        single_instance.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of single instance example
 */
#include <stddef.h>
#include <dpl/single_instance.h>
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Specify instance name!" << std::endl;
        return 0;
    }

    DPL::SingleInstance singleInstance;

    if (singleInstance.TryLock(argv[1]))
    {
        std::cout << "Succedded to lock single instance." << std::endl << "Press ENTER to release lock..." << std::endl;

        // Wait for any key
        getchar();

        // Release gathered lock
        singleInstance.Release();

        // Done
        return 0;
    }

    std::cout << "Cannot retrieve single instance lock." << std::endl
              << "Another application has locked single instance." << std::endl
              << "Will now exit." << std::endl;

    // Done
    return 0;
}
