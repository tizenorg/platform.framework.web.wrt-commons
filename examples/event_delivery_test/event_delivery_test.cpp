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
 * @file        event_delivery_server.h
 * @author      Jaroslaw Osmanski j.osmanski@samsung.com
 * @version     1.0
 * @brief       This file is the implementation file of simple event delivery test
 */
#include <dpl/application.h>
#include <dpl/controller.h>
#include <dpl/event_delivery.h>
#include <dpl/event_delivery_injector.h>
#include <dpl/generic_event.h>
#include <dpl/type_list.h>
#include <iostream>

namespace TestEvents
{
EVENT_DELIVERY_DECLARE_EVENT_MESSAGE_0(EmptyMessage)
EVENT_DELIVERY_DECLARE_EVENT_MESSAGE_2(HelloWorldMessage, std::string, std::string)
EVENT_DELIVERY_DECLARE_EVENT_MESSAGE_2(NumberMessage, int, float)
EVENT_DELIVERY_DECLARE_EVENT_MESSAGE_4(TestMessage, int, int, int, int)

EVENT_DELIVERY_IMPLEMENT_EVENT_MESSAGE(EmptyMessage)
EVENT_DELIVERY_IMPLEMENT_EVENT_MESSAGE(HelloWorldMessage)
EVENT_DELIVERY_IMPLEMENT_EVENT_MESSAGE(NumberMessage)
EVENT_DELIVERY_IMPLEMENT_EVENT_MESSAGE(TestMessage)
} // namespace TestEvents

DECLARE_GENERIC_EVENT_0(StartEvent)
DECLARE_GENERIC_EVENT_0(CloseEvent)
DECLARE_GENERIC_EVENT_0(DeleteEvent)

class MyListener :
    public DPL::Application,
    private DPL::Controller<DPL::TypeListDecl<DeleteEvent, CloseEvent>::Type>,
    public DPL::EventListener<TestEvents::HelloWorldMessage>,
    public DPL::EventListener<TestEvents::NumberMessage>
{
private:
    class OneTimeListener :
        public DPL::EventListener<TestEvents::EmptyMessage>
    {
    public:
        OneTimeListener()
        {
            DPL::EventDeliverySystem::AddListener<TestEvents::EmptyMessage > (this);
        }

        ~OneTimeListener()
        {
            LogError("Deleting OneTimeListener");
            DPL::EventDeliverySystem::RemoveListener<TestEvents::EmptyMessage > (this);
        }

    private:
        void OnEventReceived(const TestEvents::EmptyMessage &message)
        {
            (void) message;
            std::cout << "OneTimeListener empty message";

        }
    };

    OneTimeListener * oneTimeListener;

    virtual void OnEventReceived(const CloseEvent &event)
    {
        (void) event;
        Quit();
    }

    virtual void OnEventReceived(const DeleteEvent &event)
    {
        (void) event;
        delete oneTimeListener;
        oneTimeListener = NULL;
    }

protected:

    void OnEventReceived(const TestEvents::HelloWorldMessage &message)
    {
        std::cout << "Got HelloWorldMessage: " << message.GetArg0() << " : " << message.GetArg1() << std::endl;

        if (oneTimeListener != NULL)
        {
            DPL::ControllerEventHandler<DeleteEvent>::PostTimedEvent(DeleteEvent(), 1);
        }
    }

    void OnEventReceived(const TestEvents::NumberMessage &message)
    {
        std::cout << "Got NumberMessage: " << message.GetArg0() << ", " << message.GetArg1() << std::endl;
    }

public:

    MyListener(int argc, char **argv)
        : Application(argc, argv, "Listener")
    {
        Touch();

        DPL::EventDeliverySystem::AddListener<TestEvents::HelloWorldMessage>(this);
        DPL::EventDeliverySystem::AddListener<TestEvents::NumberMessage>(this);

        DPL::ControllerEventHandler<CloseEvent>::PostTimedEvent(CloseEvent(), 8);
        oneTimeListener = new OneTimeListener();
    }

    virtual ~MyListener()
    {
        DPL::EventDeliverySystem::RemoveListener<TestEvents::HelloWorldMessage > (this);
        DPL::EventDeliverySystem::RemoveListener<TestEvents::NumberMessage > (this);
        delete oneTimeListener;
    }
};

class MyPusher :
    public DPL::Application,
    private DPL::Controller<DPL::TypeListDecl<StartEvent, CloseEvent>::Type>
{
private:
    virtual void OnEventReceived(const CloseEvent &event)
    {
        (void) event;
        Quit();
    }

    virtual void OnEventReceived(const StartEvent &event)
    {
        (void) event;
        std::cout << "Publishing HelloWorldMessage..." << std::endl;
        TestEvents::HelloWorldMessage hello("Hello cruel world !", "AAA BBB CCC");
        DPL::EventDeliverySystem::Publish(hello);
        std::cout << "HelloWorldMessage published." << std::endl;

        std::cout << "Publishing NumberMessage message..." << std::endl;
        TestEvents::NumberMessage number(13, 3.14f);
        DPL::EventDeliverySystem::Publish(number);
        std::cout << "NumberMessage published." << std::endl;

        std::cout << "Publishing EmptyMessage..." << std::endl;
        TestEvents::EmptyMessage empty;
        DPL::EventDeliverySystem::Publish(empty);
        std::cout << "EmptyMessage published." << std::endl;

    }
public:
    MyPusher(int argc, char **argv)
        : Application(argc, argv, "Pusher")
    {
        Touch();

        DPL::ControllerEventHandler<StartEvent>::PostTimedEvent(StartEvent(), 2);
        DPL::ControllerEventHandler<StartEvent>::PostTimedEvent(StartEvent(), 6);
        DPL::ControllerEventHandler<CloseEvent>::PostTimedEvent(CloseEvent(), 8);
    }

    virtual ~MyPusher()
    {
    }
};

int main(int argc, char* argv[])
{
    switch (fork())
    {
        case 0:
        {
            MyPusher myPusher(argc, argv);
            myPusher.Exec();
        }
        break;

        case -1:
            printf("fork() failed!");
            break;

        default:
        {
            MyListener myListener(argc, argv);
            myListener.Exec();
        }
        break;
    }

    return 0;
}
