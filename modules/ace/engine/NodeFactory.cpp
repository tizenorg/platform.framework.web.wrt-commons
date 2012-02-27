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
#include <dpl/ace/NodeFactory.h>

NodeFactory* NodeFactory::pInstance = NULL;

AbstractTreeElement* NodeFactory::create(std::istream& is,
        TreeNode::TypeID id)
{
    AbstractTreeElement* node;

    switch (id) {
    case TreeNode::Policy:
        node = new Policy(is);
        break;
    case TreeNode::PolicySet:
        node = new PolicySet(is);
        break;
    case TreeNode::Rule:
        node = new Rule(is);
        break;
    default:
        node = NULL;
        break;
    }

    return node;
}

NodeFactory* NodeFactory::getInstance()
{
    if (pInstance == NULL) {
        pInstance = new NodeFactory;
    }
    return pInstance;
}
