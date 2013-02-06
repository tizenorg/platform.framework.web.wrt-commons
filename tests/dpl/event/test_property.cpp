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
 * @file        test_property.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of test property
 */
#include <dpl/test/test_runner.h>
#include <dpl/event/property.h>
#include <dpl/event/model.h>
#include <string>

namespace {
const int PROPERTY_VALUE_INT = 2;
const std::string PROPERTY_VALUE_STRING = "aaa";
}

int ReadSomething2(DPL::Event::Model */*model*/);
int ReadSomething2(DPL::Event::Model */*model*/)
{
    return PROPERTY_VALUE_INT;
}

std::string ReadSomething(DPL::Event::Model */*model*/);
std::string ReadSomething(DPL::Event::Model */*model*/)
{
    return PROPERTY_VALUE_STRING;
}

void WriteSomething(const std::string & /*value*/, DPL::Event::Model */*model*/);
void WriteSomething(const std::string & /*value*/, DPL::Event::Model */*model*/)
{}

class MyModel :
    public DPL::Event::Model
{
  public:
    ~MyModel() {}

    DPL::Event::Property<std::string>
    Caption;

    DPL::Event::Property<std::string>
    Testproperty0;

    DPL::Event::Property<std::string, DPL::Event::PropertyReadOnly>
    Testproperty1;

    DPL::Event::Property<std::string, DPL::Event::PropertyReadWrite>
    Testproperty2;

    DPL::Event::Property<std::string, DPL::Event::PropertyReadWrite,
                         DPL::Event::PropertyStorageCached> Testproperty3;

    DPL::Event::Property<std::string, DPL::Event::PropertyReadWrite,
                         DPL::Event::PropertyStorageDynamic> Testproperty4;

    DPL::Event::Property<int, DPL::Event::PropertyReadOnly,
                         DPL::Event::PropertyStorageDynamicCached>
    Testproperty5;

    MyModel() :
        Caption(this, "Foo caption"),
        Testproperty0(this, "", &ReadSomething),
        Testproperty1(this),
        Testproperty2(this),
        Testproperty3(this),
        Testproperty4(this, "test", &ReadSomething, &WriteSomething),
        Testproperty5(this, &ReadSomething2)
    {}
};

std::string g_caption;

void OnNameChanged(const DPL::Event::PropertyEvent<std::string> &event);
void OnNameChanged(const DPL::Event::PropertyEvent<std::string> &event)
{
    g_caption = event.value;
}

RUNNER_TEST(Model_Test)
{
    MyModel model;

    g_caption = "It is a bad caption";

    model.Caption.AddListener(&OnNameChanged);
    model.Caption.Set("Test name");

    RUNNER_ASSERT(model.Testproperty4.Get() == PROPERTY_VALUE_STRING);
    RUNNER_ASSERT(PROPERTY_VALUE_INT == model.Testproperty5.Get());
    RUNNER_ASSERT(g_caption == "Test name");
    RUNNER_ASSERT(model.Caption.Get() == "Test name");

    model.Caption.RemoveListener(&OnNameChanged);
}
