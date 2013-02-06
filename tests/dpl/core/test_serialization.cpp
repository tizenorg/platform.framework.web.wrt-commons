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
 * @file        test_address.cpp
 * @author      Tomasz Swierczek (t.swierczek@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of serialization tests
 */

#include <vector>
#include <string>
#include <list>
#include <map>

#include <dpl/test/test_runner.h>
#include <dpl/serialization.h>

RUNNER_TEST_GROUP_INIT(DPL)

// test stream class
class BinaryStream : public DPL::IStream
{
  public:
    virtual void Read(size_t num, void * bytes)
    {
        for (unsigned i = 0; i < num; ++i) {
            ((unsigned char*)bytes)[i] = data[i + readPosition];
        }
        readPosition += num;
    }
    virtual void Write(size_t num, const void * bytes)
    {
        for (unsigned i = 0; i < num; ++i) {
            data.push_back(((unsigned char*)bytes)[i]);
        }
    }
    BinaryStream()
    {
        readPosition = 0;
    }
    virtual ~BinaryStream(){}

  private:
    std::vector<unsigned char> data;
    unsigned readPosition;
};

//test ISerializable class
class TestClass : public DPL::ISerializable
{
  public:
    TestClass(int val, std::string str1, std::string str2)
    {
        a = val;
        b = str1;
        c.push_back(str1);
        c.push_back(str2);
        c.push_back(str1 + str2);
    }
    TestClass(DPL::IStream& stream)
    {
        DPL::Deserialization::Deserialize(stream, a);
        DPL::Deserialization::Deserialize(stream, b);
        DPL::Deserialization::Deserialize(stream, c);
    }
    virtual void Serialize(DPL::IStream& stream) const
    {
        DPL::Serialization::Serialize(stream, a);
        DPL::Serialization::Serialize(stream, b);
        DPL::Serialization::Serialize(stream, c);
    }
    virtual ~TestClass(){}
    virtual bool operator==(const TestClass& other)
    {
        return (a == other.a &&
                b == other.b &&
                c.size() == other.c.size() &&
                c[0] == other.c[0] &&
                c[1] == other.c[1] &&
                c[2] == other.c[2]);
    }

  private:
    int a;
    std::string b;
    std::vector<std::string> c;
};

RUNNER_TEST(Serialize_primitives)
{
    int a = 1;
    bool b = true;
    unsigned c = 23;
    BinaryStream stream;
    DPL::Serialization::Serialize(stream, a);
    DPL::Serialization::Serialize(stream, b);
    DPL::Serialization::Serialize(stream, c);
    int test_int;
    DPL::Deserialization::Deserialize(stream, test_int);
    RUNNER_ASSERT(test_int == a);
    bool test_bool;
    DPL::Deserialization::Deserialize(stream, test_bool);
    RUNNER_ASSERT(test_bool == b);
    unsigned test_unsigned;
    DPL::Deserialization::Deserialize(stream, test_unsigned);
    RUNNER_ASSERT(test_unsigned == c);
}

RUNNER_TEST(Serialize_primitive_pointers)
{
    int a = 1;
    bool b = true;
    unsigned c = 23;
    BinaryStream stream;
    DPL::Serialization::Serialize(stream, &a);
    DPL::Serialization::Serialize(stream, &b);
    DPL::Serialization::Serialize(stream, &c);
    int* test_int;
    DPL::Deserialization::Deserialize(stream, test_int);
    RUNNER_ASSERT(test_int != NULL && *test_int == a);
    bool* test_bool;
    DPL::Deserialization::Deserialize(stream, test_bool);
    RUNNER_ASSERT(test_bool != NULL && *test_bool == b);
    unsigned* test_unsigned;
    DPL::Deserialization::Deserialize(stream, test_unsigned);
    RUNNER_ASSERT(test_unsigned != NULL && *test_unsigned == c);
    delete test_int;
    delete test_bool;
    delete test_unsigned;
}

RUNNER_TEST(Serialize_strings)
{
    std::string str1 = "ALA MA KOTA";
    std::string str2 = "MULTILINE\nTEST";
    BinaryStream stream;
    DPL::Serialization::Serialize(stream, str1);
    DPL::Serialization::Serialize(stream, str2);
    std::string test_str1;
    DPL::Deserialization::Deserialize(stream, test_str1);
    RUNNER_ASSERT(test_str1 == str1);
    std::string test_str2;
    DPL::Deserialization::Deserialize(stream, test_str2);
    RUNNER_ASSERT(test_str2 == str2);
}

RUNNER_TEST(Serialize_string_pointers)
{
    std::string str1 = "ALA MA KOTA";
    std::string str2 = "MULTILINE\nTEST";
    BinaryStream stream;
    DPL::Serialization::Serialize(stream, &str1);
    DPL::Serialization::Serialize(stream, &str2);
    std::string* test_str1;
    DPL::Deserialization::Deserialize(stream, test_str1);
    RUNNER_ASSERT(test_str1 != NULL && *test_str1 == str1);
    std::string* test_str2;
    DPL::Deserialization::Deserialize(stream, test_str2);
    RUNNER_ASSERT(test_str2 != NULL && *test_str2 == str2);
    delete test_str1;
    delete test_str2;
}

RUNNER_TEST(Serialize_containers)
{
    std::vector<int> vec;
    vec.push_back(134);
    vec.push_back(265);
    std::list<bool> list;
    list.push_back(true);
    list.push_back(false);
    std::pair<int, unsigned> pair;
    pair.first = -23;
    pair.second = 1234;
    std::map<int, std::string> map;
    map.insert(std::pair<int, std::string>(45, "ALA MA CZARNEGO KOTA"));
    map.insert(std::pair<int, std::string>(-78, "...A MOZE\nMA\nWIELE LINIJEK"));
    BinaryStream stream;
    DPL::Serialization::Serialize(stream, vec);
    DPL::Serialization::Serialize(stream, list);
    DPL::Serialization::Serialize(stream, pair);
    DPL::Serialization::Serialize(stream, map);
    std::vector<int> test_vec;
    DPL::Deserialization::Deserialize(stream, test_vec);
    RUNNER_ASSERT(test_vec.size() == vec.size() &&
                  test_vec[0] == vec[0] && test_vec[1] == vec[1]);
    std::list<bool> test_list;
    DPL::Deserialization::Deserialize(stream, test_list);
    RUNNER_ASSERT(test_list.size() == list.size() &&
                  test_list.front() == list.front() &&
                  test_list.back() == test_list.back());
    std::pair<int, unsigned> test_pair;
    DPL::Deserialization::Deserialize(stream, test_pair);
    RUNNER_ASSERT(test_pair.first == pair.first &&
                  test_pair.second == pair.second);
    std::map<int, std::string> test_map;
    DPL::Deserialization::Deserialize(stream, test_map);
    RUNNER_ASSERT(test_map.size() == map.size() &&
                  test_map.at(45) == map.at(45) &&
                  test_map.at(-78) == map.at(-78));
}

RUNNER_TEST(Serialize_objects)
{
    TestClass a(123, "ASDGHUADB\n\n5679b^^()*", "TEST_STRING"),
    b(679, "HUSPIDNSAHDPA", "\nASDSADASD\naDSADASD8");
    BinaryStream stream;
    DPL::Serialization::Serialize(stream, a);
    DPL::Serialization::Serialize(stream, b);
    TestClass test_a(0, "", ""), test_b(0, "", "");
    DPL::Deserialization::Deserialize(stream, test_a);
    RUNNER_ASSERT(test_a == a);
    DPL::Deserialization::Deserialize(stream, test_b);
    RUNNER_ASSERT(test_b == b);
}

RUNNER_TEST(Serialize_all)
{
    std::map<std::string, std::vector<TestClass*> > map;
    std::vector<TestClass*> vec;
    vec.push_back(new TestClass(123, "ASDGHUADB\n\n5679b^^()*", "TEST_STRING"));
    vec.push_back(new TestClass(679, "HUSPIDNSAHDPA", "\nASDSADASD\naDSADASD8"));
    map.insert(std::pair<std::string, std::vector<TestClass*> >("KEY1", vec));
    map.insert(std::pair<std::string, std::vector<TestClass*> >("KEY2", vec));
    BinaryStream stream;

    DPL::Serialization::Serialize(stream, map);

    std::map<std::string, std::vector<TestClass*> > test_map;
    DPL::Deserialization::Deserialize(stream, test_map);
    RUNNER_ASSERT(map.size() == test_map.size());
    std::vector<TestClass*> test_vec1, test_vec2;
    test_vec1 = map.at("KEY1");
    test_vec2 = test_map.at("KEY1");
    RUNNER_ASSERT(test_vec1.size() == test_vec2.size());
    unsigned i;
    for (i = 0; i < test_vec1.size(); ++i) {
        RUNNER_ASSERT((*test_vec1[i]) == (*test_vec2[i]));
    }
    test_vec1 = map.at("KEY2");
    test_vec2 = test_map.at("KEY2");
    RUNNER_ASSERT(test_vec1.size() == test_vec2.size());
    for (i = 0; i < test_vec1.size(); ++i) {
        RUNNER_ASSERT((*test_vec1[i]) == (*test_vec2[i]));
    }
}

