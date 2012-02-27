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
 * @file        wrt_event_delivery.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of event delivery class
 */
#include <dpl/event/event_delivery.h>
#include <dpl/main.h>

namespace DPL
{
namespace Event
{

EventDeliverySystemDetail EventDeliverySystem::m_detailSystem;
EventDeliverySystem::AbstractPublisherPtrContainerMapType EventDeliverySystem::m_publishers;

namespace // anonymous
{
int InitializeMainSingleton();

int g_dummyInitializer = InitializeMainSingleton();

int InitializeMainSingleton()
{
    (void)g_dummyInitializer;
    MainSingleton::Instance();
    return 0;
}
} // namespace anonymous

AbstractEventDeliverySystemPublisher::~AbstractEventDeliverySystemPublisher() { }

}
} // namespace DPL
