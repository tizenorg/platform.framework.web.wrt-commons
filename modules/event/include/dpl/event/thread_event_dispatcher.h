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
 * @file        thread_event_dispatcher.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of thread event dispatcher
 */
#ifndef DPL_THREAD_EVENT_DISPATCHER_H
#define DPL_THREAD_EVENT_DISPATCHER_H

#include <dpl/event/abstract_event_dispatcher.h>
#include <dpl/event/abstract_event_call.h>
#include <dpl/thread.h>

namespace DPL {
namespace Event {
class ThreadEventDispatcher :
    public AbstractEventDispatcher
{
  protected:
    Thread *m_thread;

    static void StaticEventDelete(void *event, void *userParam);
    static void StaticEventDispatch(void *event, void *userParam);

    void EventDelete(AbstractEventCall *abstractEventCall);
    void EventDispatch(AbstractEventCall *abstractEventCall);

  public:
    explicit ThreadEventDispatcher();
    virtual ~ThreadEventDispatcher();

    void SetThread(Thread *thread);

    virtual void AddEventCall(AbstractEventCall *abstractEventCall);
    virtual void AddTimedEventCall(AbstractEventCall *abstractEventCall,
                                   double dueTime);
};
}
} // namespace DPL

#endif // DPL_THREAD_EVENT_DISPATCHER_H
