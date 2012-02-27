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
#include <dpl/log/log.h>
#include <dpl/foreach.h>
#include <dpl/ace/Serializer.h>
#include <dpl/ace/NodeFactory.h>

Serializer* Serializer::pInstance = NULL;

Serializer* Serializer::getInstance()
{
    if (pInstance == NULL) {
        pInstance = new Serializer;
    }
    return pInstance;
}

void Serializer::serializeInt(std::ostream& os,
        int value)
{
    os.write((char*)(&value), sizeof(value));
    //LogInfo("SerializeInt: " <<value);

    Assert(os.bad() == false && "Error Writing to file failure");
}

int Serializer::deserializeInt(std::istream& is)
{
    int value = 0;
    is.read((char*)(&value), sizeof(value));

    LogInfo("DEserializeInt: " << value);
    return value;
}

void Serializer::serializeString(std::ostream& os,
        const std::string& text) const
{
    int dataLength = text.size();
    os.write((char*)(&dataLength), sizeof(dataLength));
    os.write(text.c_str(), dataLength);

    Assert(os.bad() == false && "Error Writing to file failure");

    //LogInfo("SerializeString: " <<text);
}

std::string Serializer::deserializeString(std::istream& is)
{
    //length
    int dataLength = 0;
    is.read((char*)(&dataLength), sizeof(dataLength));

    char *buf = new char[dataLength + 1];
    is.read(buf, dataLength);
    buf[dataLength] = '\0';

    std::string result = buf;
    delete[] buf;

    //LogInfo("DEserializeString: " <<result);
    return result;
}

void Serializer::serializeListAttributes(std::ostream& os,
        const std::list<Attribute>& inputList)
{
    int dataLength = inputList.size();
    os.write((char*)(&dataLength), sizeof(dataLength));

    //LogInfo("SerializeListAttributes SIZE: " <<dataLength);
    FOREACH(it,inputList)
    {
        it->serialize(os);
    }

    Assert(os.bad() == false && "Error Writing to file failure");
}

std::list<Attribute> Serializer::deserializeListAttributes(std::istream& is)
{
    std::list<Attribute> outputList;

    int numElem = 0;
    is.read((char*)(&numElem), sizeof(numElem));

    //LogInfo("DESerializeListAttributes SIZE: " <<numElem);

    Attribute* attr;

    for (int i = 0; i < numElem; i++) {
        attr = new Attribute(is);
        outputList.push_back(*attr);
        delete attr;
    }

    return outputList;
}

void Serializer::serializeType(std::ostream& os,
        const Attribute::Type& typeId) const
{
    os.write((char*)(&typeId), sizeof(typeId));

    Assert(os.bad() == false && "Error Writing to file failure");

    //LogInfo("SerializeTypeId: " <<typeId);
}

Attribute::Type Serializer::deserializeType(std::istream& is)
{
    Attribute::Type typeId(AceDB::BaseAttribute::Type::Subject);

    is.read((char*)(&typeId), sizeof(typeId));
    //LogInfo("DEserializeTypeId: " <<typeId);
    return typeId;
}

void Serializer::serializeListStrings(std::ostream& os,
        const std::list<std::string>& inputList) const
{
    int dataLength = inputList.size();
    os.write((char*)(&dataLength), sizeof(dataLength));

    FOREACH(it, inputList)
    {
        serializeString(os, *it);
    }

    Assert(os.bad() == false && "Error Writing to file failure");
}

std::list<std::string> Serializer::deserializeListStrings(std::istream& is)
{
    std::list<std::string> outputList;

    int numElem = 0;
    is.read((char*)(&numElem), sizeof(numElem));

    for (int i = 0; i < numElem; i++) {
        outputList.push_back(this->deserializeString(is));
    }

    return outputList;
}

void Serializer::serializeMatch(std::ostream& os,
        const Attribute::Match& match) const
{
    os.write((char*)(&match), sizeof(match));
    //    LogInfo("SerializeMatch: " << match);
    Assert(os.bad() == false && "Error Writing to file failure");
}

Attribute::Match Serializer::deserializeMatch(std::istream& is)
{
    Attribute::Match match(Attribute::Match::Error);
    is.read((char*)(&match), sizeof(match));
    //    LogInfo("DESerializeMatch: " << match);
    return match;
}

bool Serializer::serializeModifier(std::ostream& os,
        const Attribute::Modifier& modifier) const
{
    os.write((char*)(&modifier), sizeof(modifier));
    //    LogInfo("SerializeModifier: " << modifier);
    return 0;
}

Attribute::Modifier Serializer::deserializeModifier(std::istream& is)
{
    Attribute::Modifier modifier(Attribute::Modifier::Non);
    is.read((char*)(&modifier), sizeof(modifier));
    //    LogInfo("DESerializeModifer: " << modifier);
    return modifier;
}

//TODO zrobic z tego szablon - z list jezeli sie da
void Serializer::serializeListTreeNode(std::ostream& os,
        std::list<TreeNode*>& inputList)
{
    int dataLength = inputList.size();
    os.write((char*)(&dataLength), sizeof(dataLength));

    //    LogInfo("SerializeListTreeNode SIZE: " <<dataLength);
    FOREACH(it, inputList)
    {
        (*it)->serialize(os);
    }

    Assert(os.bad() == false && "Error Writing to file failure");
}

std::list<TreeNode* > Serializer::deserializeListTreeNode(std::istream&is,
        TreeNode* parent)
{
    std::list<TreeNode* > outputList;

    int numElem = 0;
    is.read((char*)(&numElem), sizeof(numElem));

    //    LogInfo("DESerializeListTreeNode SIZE: " <<numElem);

    TreeNode* node;

    for (int i = 0; i < numElem; i++) {
        node = new TreeNode(is, parent);
        outputList.push_back(node);
    }

    return outputList;
}

void Serializer::serializeTypeAbstract(std::ostream& os,
        const TreeNode::TypeID& typeId)
{
    os.write((char*)(&typeId), sizeof(typeId));
    //    LogInfo(" Serialize TypeAbstract: " << typeId);

    Assert(os.bad() == false && "Error Writing to file failure");
}

TreeNode::TypeID Serializer::deserializeTypeAbstract(std::istream& is)
{
    TreeNode::TypeID typeId(TreeNode::Policy);
    is.read((char*)(&typeId), sizeof(typeId));
    //    LogInfo(" DESerialize TypeAbstract: " << typeId);
    return typeId;
}

void Serializer::serializeAbstractElement(std::ostream& os,
        AbstractTreeElement* element)
{
    element->serialize(os);

    //    LogInfo("Serialize AbstractElem: " << element);
    Assert(os.bad() == false && "Error Writing to file failure");
}

AbstractTreeElement* Serializer::deserializeAbstractElement(std::istream& is,
        TreeNode::TypeID& typeId)
{
    NodeFactory* factory = NodeFactory::getInstance();

    AbstractTreeElement* element;

    element = factory->create(is, typeId);

    //LogInfo("DESerialize AbstractElem: " << typeId);
    return element;
}

void Serializer::serializeListSubjects(std::ostream& os,
        std::list<const Subject*>& inputList)
{
    int dataLength = inputList.size();
    os.write((char*)(&dataLength), sizeof(dataLength));

    //LogInfo("Serialize list Subjects SIZE: " << dataLength);

    FOREACH (it, inputList)
    {
        //serialize method doesn't change anything in object
        const_cast<Subject* >((*it))->serialize(os);
    }

    Assert(os.bad() == false && "Error Writing to file failure");
}

std::list<const Subject*>* Serializer::deserializeListSubjects(std::istream& is)
{
    std::list<const Subject*>* outputList = new std::list<const Subject*>;

    int numElem = 0;
    is.read((char*)(&numElem), sizeof(numElem));

    //LogInfo("DESerialize list Subjects SIZE: " <<numElem);
    Subject* subject;

    for (int i = 0; i < numElem; i++) {
        subject = new Subject(is);
        outputList->push_back(subject);
    }

    return outputList;
}

void Serializer::serializeCombineAlgorithm(std::ostream& os,
        const Policy::CombineAlgorithm& combAlg)
{
    os.write((char*)(&combAlg), sizeof(combAlg));
    //LogInfo("Serialize Combine Alg:" << combAlg);

    Assert(os.bad() == false && "Error Writing to file failure");
}

Policy::CombineAlgorithm Serializer::deserializeCombineAlgorithm(
        std::istream& is)
{
    Policy::CombineAlgorithm combAlg(Policy::DenyOverride);
    is.read((char*)(&combAlg), sizeof(combAlg));
    //LogInfo("DESerialize Combine Alg:" << combAlg);
    return combAlg;
}

void Serializer::serializeCombineType(std::ostream& os,
        const Condition::CombineType& combType)
{
    os.write((char*)(&combType), sizeof(combType));
    //LogInfo("Serialize CombineType: " <<combType);

    Assert(os.bad() == false && "Error Writing to file failure");
}

Condition::CombineType Serializer::deserializeCombineType(std::istream& is)
{
    Condition::CombineType combType(Condition::AND);
    is.read((char*)(&combType), sizeof(combType));
    //LogInfo("Deserialize CombineType: " <<combType);
    return combType;
}

void Serializer::serializeListConditions(std::ostream& os,
        std::list<Condition>& inputList)
{
    int dataLength = inputList.size();
    os.write((char*)(&dataLength), sizeof(dataLength));

    //LogInfo("Serialize List Conditions SIZE:" <<dataLength);
    FOREACH (it, inputList)
    {
        it->serialize(os);
    }

    Assert(os.bad() == false && "Error Writing to file failure");
}

std::list<Condition> Serializer::deserializeListConditions(std::istream& is,
        Condition* parent)
{
    std::list<Condition> outputList;

    int numElem = 0;
    is.read((char*)(&numElem), sizeof(numElem));

    //LogInfo("DESerialize list Condition SIZE: " <<numElem);
    Condition* cond;

    for (int i = 0; i < numElem; i++) {
        cond = new Condition(is, parent);
        outputList.push_back(*cond);
        delete cond;
    }

    return outputList;
}

void Serializer::serializeEffect(std::ostream& os,
        Effect effect)
{
    os.write((char*)(&effect), sizeof(effect));
    //LogInfo("Serialize Effect: " << effect);

    Assert(os.bad() == false && "Error Writing to file failure");
}

Effect Serializer::deserializeEffect(std::istream& is)
{
    Effect effect(Deny);
    is.read((char*)(&effect), sizeof(effect));
    //LogInfo("DESerialize Effect: " << effect);
    return effect;
}

void Serializer::serializeCondition(std::ostream& os,
        Condition& cond)
{
    cond.serialize(os);

    Assert(os.bad() == false && "Error Writing to file failure");
}

Condition& Serializer::deserializeCondition(std::istream& is)
{
    //[CR] maybe we should use Factory here
    Condition * cond = new Condition(is);
    return *cond;
}
