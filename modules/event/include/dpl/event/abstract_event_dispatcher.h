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
 * @file        abstract_event_dispatcher.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of abstract event
 * dispatcher
 */
#ifndef DPL_ABSTRACT_EVENT_DISPATCHER_H
#define DPL_ABSTRACT_EVENT_DISPATCHER_H

#include <dpl/event/abstract_event_call.h>
#include <dpl/noncopyable.h>

namespace DPL {
namespace Event {
class AbstractEventDispatcher :
    private Noncopyable
{
  public:
    /**
     * Constructor
     */
    explicit AbstractEventDispatcher();

    /**
     * Destructor
     */
    virtual ~AbstractEventDispatcher();

    /**
     * Add abstract event call to abstract event dispatcher
     *
     * @param[in] abstractEventCall Pointer to abstract event call to add
     * @return none
     */
    virtual void AddEventCall(AbstractEventCall *abstractEventCall) = 0;

    /**
     * Add abstract timed event call to abstract event dispatcher
     *
     * @param[in] abstractEventCall Pointer to abstract event call to add
     * @param[in] dueTime Due time for timed event in seconds
     * @return none
     */
    virtual void AddTimedEventCall(AbstractEventCall *abstractEventCall,
                                   double dueTime) = 0;
};
}
} // namespace DPL

#endif // DPL_ABSTRACT_EVENT_DISPATCHER_H
