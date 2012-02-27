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
#if !defined(_SERIALIZER_H)
#define _SERIALIZER_H

#include <string>
#include <iostream>
#include <fstream>

#include "TreeNode.h"
#include "Attribute.h"
#include "Subject.h"
#include "Policy.h"
#include "Condition.h"

class Serializer
{
  public:
    static Serializer* getInstance();

    /**** Interaface ****************/

    /**
     * Serialized tree given by
     * @param root
     * */
    // KW    void serializeTree(TreeNode* root);

    /**
     * deserialize tree
     * @return pointer to deserialized tree
     * or NULL if error occur
     * */
    // KW     TreeNode* deserializeTree();

    /* ------------------------------*/

    void serializeInt(std::ostream& os,
            const int value);
    int  deserializeInt(std::istream& is);

    void serializeString(std::ostream& os,
            const std::string& text) const;
    std::string deserializeString(std::istream& is);

    void serializeListAttributes(std::ostream& os,
            const std::list<Attribute>& inputList);
    std::list<Attribute> deserializeListAttributes(std::istream& is);

    void serializeType(std::ostream& os,
            const Attribute::Type& typeId) const;
    Attribute::Type deserializeType(std::istream& is);

    void serializeListStrings(std::ostream& os,
            const std::list<std::string>& inputList) const;

    std::list<std::string> deserializeListStrings(std::istream&is);

    void serializeMatch(std::ostream& os,
            const Attribute::Match& match) const;
    Attribute::Match deserializeMatch(std::istream& is);
    //TODO add smartptr
    bool serializeModifier(std::ostream& os,
            const Attribute::Modifier& modifier) const;
    Attribute::Modifier deserializeModifier(std::istream& is);

    void serializeListTreeNode(std::ostream& os,
            std::list<TreeNode*>& inputList);
    std::list<TreeNode* > deserializeListTreeNode(std::istream&is,
            TreeNode* parent);

    void serializeTypeAbstract(std::ostream& os,
            const TreeNode::TypeID& typeId);
    TreeNode::TypeID deserializeTypeAbstract(std::istream& is);

    void serializeAbstractElement(std::ostream& os,
            AbstractTreeElement* element);
    AbstractTreeElement* deserializeAbstractElement(std::istream& is,
            TreeNode::TypeID&);

    void serializeListSubjects(std::ostream& os,
            std::list<const Subject*>& inputList);
    std::list<const Subject*>* deserializeListSubjects(std::istream& is);

    void serializeCombineAlgorithm(std::ostream& os,
            const Policy::CombineAlgorithm & combAlg);
    Policy::CombineAlgorithm deserializeCombineAlgorithm(std::istream& is);

    void serializeCombineType(std::ostream& os,
            const Condition::CombineType& combType);
    Condition::CombineType deserializeCombineType(std::istream& is);

    void serializeListConditions(std::ostream& os,
            std::list<Condition>& inputList);
    std::list<Condition> deserializeListConditions(std::istream& is,
            Condition* parent);

    void serializeEffect(std::ostream& os,
            Effect effect);
    Effect deserializeEffect(std::istream& is);

    void serializeCondition(std::ostream& os,
            Condition& cond);
    Condition& deserializeCondition(std::istream& is);

  private:
    Serializer()
    {
    }
    static Serializer* pInstance;

    std::filebuf fileBuffer;
};

#endif

