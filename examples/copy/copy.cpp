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
 * @file        copy.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of copy example
 */
#include <stddef.h>
#include <dpl/file_input.h>
#include <dpl/file_output.h>
#include <dpl/copy.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
    if (argc != 3 && argc != 4)
    {
        std::cout << "Invalid parameters: copy [input_file] [output_file] [OPTIONAL: number_of_bytes]" << std::endl;
        return -1;
    }
    UNHANDLED_EXCEPTION_HANDLER_BEGIN
    {
        DPL::FileInput input(argv[1]);
        DPL::FileOutput output(argv[2]);

        if (argc == 3)
            DPL::Copy(&input, &output);
	else
	    DPL::Copy(&input, &output, static_cast<size_t>(atoi(argv[3])));
    }
    UNHANDLED_EXCEPTION_HANDLER_END

    return 0;
}
