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
 * @file        test_foreach.cpp
 * @author      Bartosz Janiak (b.janiak@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of foreach tests.
 */

#include <dpl/test/test_runner.h>
#include <dpl/foreach.h>
#include <vector>
#include <set>
#include <list>

RUNNER_TEST_GROUP_INIT(DPL)

static const size_t testContainerSize = 1024;

template<typename Container>
void VerifyForeach(Container& container)
{
    size_t i = 0;
    FOREACH(it, container)
    {
        RUNNER_ASSERT(*it == i);
        i++;
    }
    RUNNER_ASSERT(i == container.size());
}

#define VERIFY_FOREACH(container)                               \
    {                                                           \
        size_t i = 0;                                           \
        FOREACH(it, container)                                  \
        {                                                       \
            RUNNER_ASSERT(*it == i);                            \
            i++;                                                \
        }                                                       \
    }

static size_t numberOfCallsToTemporaryList = 0;
std::list<size_t> temporaryList();
std::list<size_t> temporaryList()
{
    ++numberOfCallsToTemporaryList;
    std::list<size_t> list;
    for (size_t i = 0; i < testContainerSize; i++) {
        list.push_back(i);
    }
    return list;
}

static size_t numberOfCallsToTemporaryVector = 0;
std::vector<size_t> temporaryVector();
std::vector<size_t> temporaryVector()
{
    ++numberOfCallsToTemporaryVector;
    std::vector<size_t> vector;
    for (size_t i = 0; i < testContainerSize; i++) {
        vector.push_back(i);
    }
    return vector;
}

static size_t numberOfCallsToTemporarySet = 0;
std::set<size_t> temporarySet();
std::set<size_t> temporarySet()
{
    ++numberOfCallsToTemporarySet;
    std::set<size_t> set;
    for (size_t i = 0; i < testContainerSize; i++) {
        set.insert(i);
    }
    return set;
}

RUNNER_TEST(Foreach_std_containers)
{
    std::vector<size_t> vector;
    std::list<size_t> list;
    std::set<size_t> set;

    for (size_t i = 0; i < testContainerSize; i++) {
        vector.push_back(i);
        list.push_back(i);
        set.insert(i);
    }

    VerifyForeach(vector);
    VerifyForeach(list);
    VerifyForeach(set);

    VERIFY_FOREACH(temporaryList());
    VERIFY_FOREACH(temporaryVector());
    VERIFY_FOREACH(temporarySet());

    RUNNER_ASSERT(numberOfCallsToTemporaryList == 1);
    RUNNER_ASSERT(numberOfCallsToTemporaryVector == 1);
    RUNNER_ASSERT(numberOfCallsToTemporarySet == 1);
}
