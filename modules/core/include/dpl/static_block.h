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
 * @file        static_block.h
 * @author      Iwanek Tomasz (t.iwanek@samsung.com)
 * @version     1.0
 */
#ifndef STATIC_BLOCK_H
#define STATIC_BLOCK_H

#include <dpl/preprocessor.h>

//NOTE: order of static initialization of blocks is not specified

// to be used only outside class of function scopes
#define STATIC_BLOCK_IMPL( UNIQUE )                                                                 \
    static void DPL_MACRO_CONCAT( _staticBlock , UNIQUE() );                                        \
    static int DPL_MACRO_CONCAT( _staticBlockInitAssurence , UNIQUE ) = []() -> int                 \
    {                                                                                               \
        (void) DPL_MACRO_CONCAT( _staticBlockInitAssurence , UNIQUE );                              \
        DPL_MACRO_CONCAT( _staticBlock ,  UNIQUE() );                                               \
        return 0;                                                                                   \
    }();                                                                                            \
    void DPL_MACRO_CONCAT( _staticBlock , UNIQUE() )                                                \

#define STATIC_BLOCK                                                                                \
    STATIC_BLOCK_IMPL( __COUNTER__ )                                                               \

//for class implementation
#define STATIC_BLOCK_CLASS( classname, methodname ) STATIC_BLOCK { classname::methodname(); }

#endif // STATIC_BLOCK_H
