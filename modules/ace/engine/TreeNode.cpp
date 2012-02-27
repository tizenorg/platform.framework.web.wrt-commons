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
#include <dpl/ace/TreeNode.h>
#include <dpl/assert.h>
#include <dpl/ace/Serializer.h>
#include <dpl/log/log.h>

//Tree node destructor is a tricky part, only the original tree should remove the elements
//release resources should be called when we want to destroy the whole tree
TreeNode::~TreeNode()
{
}

bool TreeNode::serialize(std::ostream& os)
{
    Serializer* serializer = Serializer::getInstance();

    //does node have any child - anty Inf recursion
    if (!children.empty()) {
        serializer->serializeInt(os, 1);
        serializer->serializeListTreeNode(os, children);
    } else {
        serializer->serializeInt(os, 0);
    }

    serializer->serializeTypeAbstract(os, typeID);
    serializer->serializeAbstractElement(os, element);

    //LOG << "TreeNode  Serialized" << std::endl;
    return 0;
}

TreeNode::TreeNode(std::istream& is,
        TreeNode* parent)
{
    Serializer* serializer = Serializer::getInstance();

    this->parent = parent;

    //check if this node has any child
    int hasChild = 0;
    hasChild = serializer->deserializeInt(is);

    if (hasChild) { //[CR] - 0 remove copying list ....
        children = serializer->deserializeListTreeNode(is, this);
    }

    typeID = serializer->deserializeTypeAbstract(is);
    element = serializer->deserializeAbstractElement(is, typeID);
    //LOG << "TreeNode  DESERIALIZED" << std::endl;
}

//TODO release resources is releaseTheSubtree and delete the element
void TreeNode::releaseResources()
{
    Assert(this != 0);
    delete element;
    std::list<TreeNode*>::iterator it = this->children.begin();
    while (it != children.end()) {
        (*it)->releaseResources();
        ++it;
    }
    delete this;
}

// KW void TreeNode::releaseTheSubtree(){
// KW
// KW     std::list<TreeNode *>::iterator it = children.begin();
// KW
// KW     for(; it != children.end();it++){
// KW         (*it)->releaseTheSubtree();
// KW     }
// KW     delete this;
// KW }

int TreeNode::level = 0;

// KW void TreeNode::printSubtree(){
// KW
// KW     TreeNode::level++;
// KW
// KW     for(int i=0;i<level;i++)
// KW     {
// KW         std::cout<<" ";
// KW     }
// KW     std::cout<<"l"<<level<<": ";
// KW     std::cout<<this;
// KW
// KW     ChildrenIterator it = this->children.begin();
// KW
// KW     for(;it != children.end();++it){
// KW         (*it)->printSubtree();
// KW     }
// KW
// KW     TreeNode::level--;
// KW }

std::ostream & operator<<(std::ostream & out,
        const TreeNode * node)
{
    std::string tmp;

    switch (node->getTypeID()) {
    case TreeNode::Policy:
        tmp = "Policy";
        break;
    case TreeNode::PolicySet:
        tmp = "PolicySet";
        break;
    case TreeNode::Rule:
        tmp = "Rule";
        break;
    default:
        break;
    }

    out << "" << tmp << "-> children count: " << node->children.size() <<
    ": " << std::endl;
    AbstractTreeElement * el = node->getElement();
    if (el != NULL) {
        el->printData();
    } else {
        std::cout << "Empty element!" << std::endl;
    }

    return out;
}

