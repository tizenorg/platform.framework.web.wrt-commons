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
 * This file contains the declaration of the NestedLoopManager class.
 *
 * @file    nested_loop.h
 * @author  Lukasz Marek (l.marek@samsung.com)
 * @version 0.1
 * @brief   This file contains the declaration of the NestedLoopManager class.
 */

#ifndef DPL_NESTED_LOOP_H_
#define DPL_NESTED_LOOP_H_

#include <vector>
#include <list>
#include <map>
#include <dpl/event/controller.h>
#include <dpl/singleton.h>

namespace DPL
{
namespace Event
{

class LoopExitEvent
{
};

typedef unsigned LoopHandle;
typedef std::list<LoopHandle> LoopHandleList;

const LoopHandle UNDEFINED_LOOP_HANDLE = 0;

class NestedLoopManager :
    protected Controller<TypeListDecl<LoopExitEvent>::Type>
{
  public:
    void* begin(LoopHandle loopHandle);
    void exit(LoopHandle loopHandle,
            void *userParam = NULL);
    size_t getLevel() const;
    LoopHandle getNewHandle();

  protected:
    void OnEventReceived(const LoopExitEvent& event);

  private:
    struct LoopInformation
    {
        LoopInformation(LoopHandle handle) :
            loopHandle(handle),
            exitFlag(false),
            userParam(NULL)
        {
        }

        LoopHandle loopHandle;
        bool exitFlag;
        void *userParam;
    };

    class RunningLoopsHandlePredicate
    {
      public:
        RunningLoopsHandlePredicate(LoopHandle handle) :
            m_handle(handle)
        {
        }

        bool operator()(const LoopInformation &loopInformation) const
        {
            return loopInformation.loopHandle == m_handle;
        }

      private:
        LoopHandle m_handle;
    };

    NestedLoopManager();
    ~NestedLoopManager();

    typedef std::vector<LoopInformation> RunningLoopsList;
    typedef RunningLoopsList::iterator RunningLoopsListIterator;

    bool m_eventGuard; // only one event is allowed
    LoopHandle m_handle;
    RunningLoopsList m_runningLoops;

    friend class Singleton<NestedLoopManager>;
};

typedef Singleton<NestedLoopManager> NestedLoopManagerSingleton;

}
} // namespace DPL

#endif // WRT_ENGINE_SRC_COMMON_NESTED_LOOP_H_
