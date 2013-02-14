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
 * @file        main_event_dispatcher.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of main event dispatcher
 * for EFL
 */
#ifndef DPL_MAIN_EVENT_DISPATCHER_H
#define DPL_MAIN_EVENT_DISPATCHER_H

#include <dpl/event/abstract_event_dispatcher.h>
#include <dpl/event/abstract_event_call.h>
#include <dpl/waitable_event.h>
#include <dpl/exception.h>
#include <dpl/singleton.h>
#include <dpl/mutex.h>
#include <dpl/framework_efl.h>
#include <list>

namespace DPL {
namespace Event {
class MainEventDispatcher :
    public AbstractEventDispatcher
{
  public:
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, CreateFailed)
        DECLARE_EXCEPTION_TYPE(Base, AddEventFailed)
        DECLARE_EXCEPTION_TYPE(Base, AddTimedEventFailed)
    };

  protected:
    struct WrappedEventCall
    {
        AbstractEventCall *abstractEventCall;
        bool timed;
        double dueTime;

        WrappedEventCall(AbstractEventCall *abstractEventCallArg,
                         bool timedArg,
                         double dueTimeArg) :
            abstractEventCall(abstractEventCallArg),
            timed(timedArg),
            dueTime(dueTimeArg)
        {}
    };

    typedef std::list<WrappedEventCall> WrappedEventCallList;

    // Cross thread send support
    WrappedEventCallList m_wrappedCrossEventCallList;
    Mutex m_crossEventCallMutex;
    WaitableEvent* m_crossEventCallInvoker;

    Ecore_Event_Handler *m_eventCallHandler;
    Ecore_Fd_Handler *m_crossEventCallHandler;

    int m_eventId;

    // Timed event support
    struct TimedEventStruct
    {
        AbstractEventCall *abstractEventCall;
        MainEventDispatcher *This;

        TimedEventStruct(AbstractEventCall *abstractEventCallArg,
                         MainEventDispatcher *ThisArg) :
            abstractEventCall(abstractEventCallArg),
            This(ThisArg)
        {}
    };

    void InternalAddEvent(AbstractEventCall *abstractEventCall,
                          bool timed,
                          double dueTime);

    static void StaticDeleteEvent(void *data, void *event);
    static Eina_Bool StaticDispatchEvent(void *data, int type, void *event);
    static Eina_Bool StaticDispatchTimedEvent(void *event);
    static Eina_Bool StaticDispatchCrossInvoker(void *data,
                                                Ecore_Fd_Handler *fd_handler);

    void DeleteEvent(AbstractEventCall *abstractEventCall);
    void DispatchEvent(AbstractEventCall *abstractEventCall);
    void DispatchTimedEvent(AbstractEventCall *abstractEventCall);
    void DispatchCrossInvoker();

  public:
    explicit MainEventDispatcher();
    virtual ~MainEventDispatcher();

    virtual void AddEventCall(AbstractEventCall *abstractEventCall);
    virtual void AddTimedEventCall(AbstractEventCall *abstractEventCall,
                                   double dueTime);
    virtual void ResetCrossEventCallHandler();
};

MainEventDispatcher& GetMainEventDispatcherInstance();
}
} // namespace DPL

#endif // DPL_MAIN_EVENT_DISPATCHER_H
