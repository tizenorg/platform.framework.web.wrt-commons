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
 * @file        event_listener.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of MVC event listener
 */
#ifndef DPL_EVENT_LISTENER_H
#define DPL_EVENT_LISTENER_H

#include <dpl/noncopyable.h>

namespace DPL
{
namespace Event
{

template<typename EventType>
class EventListener
    : private Noncopyable
{
public:
    EventListener()
    {
    }

    virtual ~EventListener()
    {
    }

    virtual void OnEventReceived(const EventType &event) = 0;
};

}
} // namespace DPL

#endif // DPL_EVENT_LISTENER_H
