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
//  @ File Name : Combiner.h
//  @ Date : 2009-05-06
//  @ Author : Samsung
//
//

#if !defined(_COMBINER_H)
#define _COMBINER_H

#include <set>

#include <dpl/ace/Attribute.h>
#include <dpl/ace/TreeNode.h>

class Combiner
{
  protected:

    const AttributeSet * attrSet;

  public:

    virtual Effect combineRules(const TreeNode * rule) = 0;
    virtual Effect combinePolicies(const TreeNode * policy) = 0;

    const AttributeSet * getAttributeSet() const
    {
        return this->attrSet;
    }
    void setAttributeSet(const AttributeSet * attrSet)
    {
        this->attrSet = attrSet;
    }
    virtual ~Combiner()
    {
    }                       //attrSet is deleted elsewhere
};

#endif  //_COMBINER_H
