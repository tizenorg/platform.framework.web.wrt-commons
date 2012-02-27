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
//
//
//
//  @ Project : Access Control Engine
//  @ File Name : AbstractTreeElement.h
//  @ Date : 2009-05-25
//  @ Author : Samsung
//
//
#if !defined(_ABSTRACTTREEELEMENT_H)
#define _ABSTRACTTREEELEMENT_H

#include <list>
#include "Effect.h"
#include <iostream>

class AbstractTreeElement
{
  public:

    virtual ~AbstractTreeElement()
    {
    }

    virtual void printData() = 0;

    virtual void serialize(std::ostream& os) = 0;
  protected:
};

#endif  //_ABSTRACTTREEELEMENT_H
