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
 * @file        event_delivery_detail.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file of event delivery EFL detail class
 */
#ifndef DPL_EVENT_DELIVERY_DETAIL_EFL_H
#define DPL_EVENT_DELIVERY_DETAIL_EFL_H

#include <dpl/event/event_delivery_messages.h>
#include <dpl/framework_vconf.h>
#include <dpl/noncopyable.h>
#include <dpl/assert.h>
#include <string>
#include <list>

namespace DPL
{
namespace Event
{

class EventDeliverySystemDetail
    : private Noncopyable
{
private:
    // Generic noti event prefixes
    static const char *NOTI_EVENT_ID_GENERIC_0;
    static const char *NOTI_EVENT_ID_GENERIC_1;
    static const char *NOTI_EVENT_ID_GENERIC_2;
    static const char *NOTI_EVENT_ID_GENERIC_3;
    static const char *NOTI_EVENT_ID_GENERIC_4;

    // Other noti events
    static const char *NOTI_EVENT_ID_NULL;

    // Main noti handle
    int m_notiHandle;

    // Noti signaling class
    class NotiSignal
    {
    private:
        EventDeliverySystemDetail *m_receiver;
        const char *m_gcid;
        int m_sysnoti;
        std::string m_eventName;

    public:
        NotiSignal(EventDeliverySystemDetail *receiver, const char *gcid, const std::string &eventName)
            : m_receiver(receiver),
              m_gcid(gcid),
              m_sysnoti(-1),
              m_eventName(eventName)
        {
            Assert(receiver != NULL);
        }

        NotiSignal(EventDeliverySystemDetail *receiver, int sysnoti)
            : m_receiver(receiver),
              m_gcid(NULL),
              m_sysnoti(sysnoti)
        {
            Assert(receiver != NULL);
        }

        const char *GetGcid() const
        {
            return m_gcid;
        }

        int GetSysNoti() const
        {
            return m_sysnoti;
        }

        std::string GetEventName() const
        {
            return m_eventName;
        }

        EventDeliverySystemDetail *GetReceiver() const
        {
            return m_receiver;
        }
    };

    // Registered noti singal list
    typedef std::list<NotiSignal> NotiSignalList;
    NotiSignalList m_notiSignalList;

    // setting library callbacks
    class SettingSignal
    {
    private:
        EventDeliverySystemDetail *m_receiver;
        std::string m_key;

    public:
        SettingSignal(EventDeliverySystemDetail *receiver, const std::string &key)
            : m_receiver(receiver),
              m_key(key)
        {
        }

        std::string GetKey() const
        {
            return m_key;
        }

        EventDeliverySystemDetail *GetReceiver() const
        {
            return m_receiver;
        }
    };

    typedef std::list<SettingSignal> SettingSignalList;
    SettingSignalList m_settingSignalList;

    // Global noti callbacks
    class GlobalNotiSignal
    {
    private:
        EventDeliverySystemDetail *m_receiver;
        std::string m_key;

    public:
        GlobalNotiSignal(EventDeliverySystemDetail *receiver, const std::string &key)
            : m_receiver(receiver),
              m_key(key)
        {
        }

        std::string GetKey() const
        {
            return m_key;
        }

        EventDeliverySystemDetail *GetReceiver() const
        {
            return m_receiver;
        }
    };

    typedef std::list<GlobalNotiSignal> GlobalNotiSignalList;
    GlobalNotiSignalList m_globalNotiSignalList;

    // Utilities
    std::string ReadFile(const std::string &fileName) const;
    std::string GetGenericNotiFilePath(const std::string &eventName) const;

    //  Noti callback registration/unregistration
    void RegisterFileNotiCallback(const char *gcid, const std::string &eventName);
    void UnregisterFileNotiCallback(const std::string &eventName);

    void RegisterSettingCallback(const std::string &key);
    void UnregisterSettingCallback(const std::string &key);

    void RegisterGlobalNotiCallback(const std::string &key);
    void UnregisterGlobalNotiCallback(const std::string &key);

    // Emit noti signal
    void SignalGenericNoti(const std::string &eventName, const std::string &contents) const;

    // Signals
    static void OnNotiGlobalSignal(void *notiData);
    static void OnSettingSignal(keynode_t *keyNode, void *userParam);
    static void OnGlobalNotiSignal(void *globalNotiData);

    void OnNotiSignal(NotiSignal *notiSignal);

public:
    EventDeliverySystemDetail();
    virtual ~EventDeliverySystemDetail();

    void Listen(const EventMessages::RoamingChanged &event);
    void Listen(const EventMessages::NetworkTypeChanged &event);
    void Listen(const EventMessages::HibernationEnter &event);
    void Listen(const EventMessages::HibernationLeave &event);

    void Unlisten(const EventMessages::RoamingChanged &event);
    void Unlisten(const EventMessages::NetworkTypeChanged &event);
    void Unlisten(const EventMessages::HibernationEnter &event);
    void Unlisten(const EventMessages::HibernationLeave &event);

    void Publish(const EventMessages::Generic0 &event)
    {
        std::string eventName = std::string(NOTI_EVENT_ID_GENERIC_0) + event.GetGcid();
        SignalGenericNoti(eventName, event.SerializeType());
    }

    template<typename Arg0Type>
    void Publish(const EventMessages::Generic1<Arg0Type> &event)
    {
        std::string eventName = std::string(NOTI_EVENT_ID_GENERIC_1) + event.GetGcid();
        SignalGenericNoti(eventName, event.SerializeType());
    }

    template<typename Arg0Type, typename Arg1Type>
    void Publish(const EventMessages::Generic2<Arg0Type, Arg1Type> &event)
    {
        std::string eventName = std::string(NOTI_EVENT_ID_GENERIC_2) + event.GetGcid();
        SignalGenericNoti(eventName, event.SerializeType());
    }

    template<typename Arg0Type, typename Arg1Type, typename Arg2Type>
    void Publish(const EventMessages::Generic3<Arg0Type, Arg1Type, Arg2Type> &event)
    {
        std::string eventName = std::string(NOTI_EVENT_ID_GENERIC_3) + event.GetGcid();
        SignalGenericNoti(eventName, event.SerializeType());
    }

    template<typename Arg0Type, typename Arg1Type, typename Arg2Type, typename Arg3Type>
    void Publish(const EventMessages::Generic4<Arg0Type, Arg1Type, Arg2Type, Arg3Type> &event)
    {
        std::string eventName = std::string(NOTI_EVENT_ID_GENERIC_4) + event.GetGcid();
        SignalGenericNoti(eventName, event.SerializeType());
    }

    void Listen(const EventMessages::Generic0 &event)
    {
        std::string eventName = std::string(NOTI_EVENT_ID_GENERIC_0) + event.GetGcid();
        RegisterFileNotiCallback(event.GetGcid(), eventName);
    }

    template<typename Arg0Type>
    void Listen(const EventMessages::Generic1<Arg0Type> &event)
    {
        std::string eventName = std::string(NOTI_EVENT_ID_GENERIC_1) + event.GetGcid();
        RegisterFileNotiCallback(event.GetGcid(), eventName);
    }

    template<typename Arg0Type, typename Arg1Type>
    void Listen(const EventMessages::Generic2<Arg0Type, Arg1Type> &event)
    {
        std::string eventName = std::string(NOTI_EVENT_ID_GENERIC_2) + event.GetGcid();
        RegisterFileNotiCallback(event.GetGcid(), eventName);
    }

    template<typename Arg0Type, typename Arg1Type, typename Arg2Type>
    void Listen(const EventMessages::Generic3<Arg0Type, Arg1Type, Arg2Type> &event)
    {
        std::string eventName = std::string(NOTI_EVENT_ID_GENERIC_3) + event.GetGcid();
        RegisterFileNotiCallback(event.GetGcid(), eventName);
    }

    template<typename Arg0Type, typename Arg1Type, typename Arg2Type, typename Arg3Type>
    void Listen(const EventMessages::Generic4<Arg0Type, Arg1Type, Arg2Type, Arg3Type> &event)
    {
        std::string eventName = std::string(NOTI_EVENT_ID_GENERIC_4) + event.GetGcid();
        RegisterFileNotiCallback(event.GetGcid(), eventName);
    }

    void Unlisten(const EventMessages::Generic0 &event)
    {
        std::string eventName = std::string(NOTI_EVENT_ID_GENERIC_0) + event.GetGcid();
        UnregisterFileNotiCallback(eventName);
    }

    template<typename Arg0Type>
    void Unlisten(const EventMessages::Generic1<Arg0Type> &event)
    {
        std::string eventName = std::string(NOTI_EVENT_ID_GENERIC_1) + event.GetGcid();
        UnregisterFileNotiCallback(eventName);
    }

    template<typename Arg0Type, typename Arg1Type>
    void Unlisten(const EventMessages::Generic2<Arg0Type, Arg1Type> &event)
    {
        std::string eventName = std::string(NOTI_EVENT_ID_GENERIC_2) + event.GetGcid();
        UnregisterFileNotiCallback(eventName);
    }

    template<typename Arg0Type, typename Arg1Type, typename Arg2Type>
    void Unlisten(const EventMessages::Generic3<Arg0Type, Arg1Type, Arg2Type> &event)
    {
        std::string eventName = std::string(NOTI_EVENT_ID_GENERIC_3) + event.GetGcid();
        UnregisterFileNotiCallback(eventName);
    }

    template<typename Arg0Type, typename Arg1Type, typename Arg2Type, typename Arg3Type>
    void Unlisten(const EventMessages::Generic4<Arg0Type, Arg1Type, Arg2Type, Arg3Type> &event)
    {
        std::string eventName = std::string(NOTI_EVENT_ID_GENERIC_4) + event.GetGcid();
        UnregisterFileNotiCallback(eventName);
    }
};

}
} // namespace DPL

#endif // DPL_EVENT_DELIVERY_DETAIL_EFL_H
