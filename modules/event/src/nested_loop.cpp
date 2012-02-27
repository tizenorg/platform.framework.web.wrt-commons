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
/**
 * This file contains the definition of the NestedLoopManager class.
 *
 * @file    nested_loop.cpp
 * @author  Lukasz Marek (l.marek@samsung.com)
 * @version 0.1
 * @brief   This file contains the definition of the NestedLoopManager class.
 */

#include <dpl/event/nested_loop.h>
#include <algorithm>
#include <Ecore.h>
#include <dpl/log/log.h>
#include <dpl/assert.h>
#include <dpl/singleton_impl.h>

IMPLEMENT_SINGLETON(DPL::Event::NestedLoopManager)

namespace DPL
{
namespace Event
{

NestedLoopManager::NestedLoopManager() : m_eventGuard(false),
    m_handle(0)
{
    Touch();
}

NestedLoopManager::~NestedLoopManager()
{
}

void* NestedLoopManager::begin(LoopHandle loopHandle)
{
    LoopInformation info(loopHandle);
    m_runningLoops.push_back(info);
    LogPedantic("Nested loop begin. Nested loop level: " << getLevel());

    ecore_main_loop_begin();

    Assert(m_runningLoops.size() && "No loop on the stack");

    info = m_runningLoops.back();
    m_runningLoops.pop_back();

    Assert(info.loopHandle == loopHandle && "You exit from wrong loop");
    Assert(info.exitFlag == true && "Exit flag not set");

    LogPedantic("Nested loop quit. Nested loop level: " << getLevel());

    if (!m_runningLoops.empty() && m_runningLoops.back().exitFlag &&
        !m_eventGuard) {
        m_eventGuard = true;
        LoopExitEvent event;
        PostEvent(event);
    }
    return info.userParam;
}

void NestedLoopManager::exit(LoopHandle loopHandle,
        void *userParam)
{
    RunningLoopsListIterator iterator = std::find_if(
            m_runningLoops.begin(),
            m_runningLoops.end(),
            RunningLoopsHandlePredicate(loopHandle));

    Assert(iterator != m_runningLoops.end() && "Unknown loopHandle");
    Assert(iterator->exitFlag == false && "You cannot close a loop twice.");

    iterator->exitFlag = true;
    iterator->userParam = userParam;

    if (m_runningLoops.back().exitFlag && !m_eventGuard) {
        m_eventGuard = true;
        LoopExitEvent event;
        PostEvent(event);
    }
}

size_t NestedLoopManager::getLevel() const
{
    return m_runningLoops.size();
}

LoopHandle NestedLoopManager::getNewHandle()
{
    return m_handle++;
}

void NestedLoopManager::OnEventReceived(const LoopExitEvent& event)
{
    (void)event;
    Assert(!m_runningLoops.empty());
    m_eventGuard = false; // no event in queue
    if (m_runningLoops.back().exitFlag) {
        //exit loop when last started one is readu to finish
        //this will post event if next loop is ready to exit
        ecore_main_loop_quit();
    }
}

}
}
