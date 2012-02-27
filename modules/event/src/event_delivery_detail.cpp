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
 * @file        wrt_event_delivery_detail.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of event delivery EFL detail class
 */
#include <dpl/event/event_delivery_detail.h>
#include <dpl/event/event_delivery_injector.h>
#include <dpl/framework_appcore.h>
#include <dpl/framework_vconf.h>
#include <dpl/scoped_array.h>
#include <dpl/assert.h>
#include <dpl/log/log.h>
#include <heynoti.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <cstdio>

namespace // anonymous
{
const char *GENERIC_NOTI_ROOT_PATH = "/tmp/noti/generic";
} // namespace anonymous

namespace DPL
{
namespace Event
{

const char *EventDeliverySystemDetail::NOTI_EVENT_ID_GENERIC_0 = "GenericEvent0_";
const char *EventDeliverySystemDetail::NOTI_EVENT_ID_GENERIC_1 = "GenericEvent1_";
const char *EventDeliverySystemDetail::NOTI_EVENT_ID_GENERIC_2 = "GenericEvent2_";
const char *EventDeliverySystemDetail::NOTI_EVENT_ID_GENERIC_3 = "GenericEvent3_";
const char *EventDeliverySystemDetail::NOTI_EVENT_ID_GENERIC_4 = "GenericEvent4_";

// Global noti signal names
const char *GlobalNotiHibernationEnterSignal = "HIBERNATION_ENTER";
const char *GlobalNotiHibernationLeaveSignal = "HIBERNATION_LEAVE";

EventDeliverySystemDetail::EventDeliverySystemDetail()
{
    // Open noti subsystem
    LogInfo("Initializing core_util_noti subsystem...");

    m_notiHandle = heynoti_init();
    Assert(m_notiHandle != -1);

    // Assume that there is ECORE environment running, attach noti library to it
    LogPedantic("Attaching core_util_noti subsystem to ECORE...");

    int ret = heynoti_attach_handler(m_notiHandle);
    Assert(ret != -1);

    (void)ret;

    LogInfo("core_util_noti subsystem successfuly initialized.");
}

EventDeliverySystemDetail::~EventDeliverySystemDetail()
{
    // All signal handlers should be deattached
    Assert(m_notiSignalList.empty());
    Assert(m_settingSignalList.empty());
    Assert(m_globalNotiSignalList.empty());

    // Detach noti library from ECORE environment
    LogPedantic("Detaching core_util_noti subsystem from ECORE...");

    int ret = heynoti_detach_handler(m_notiHandle);
    Assert(ret != -1);

    (void)ret;

    // Close noti sybsystem
    LogInfo("Deinitializing core_util_noti subsystem...");

    Assert(m_notiHandle != -1);
    heynoti_close(m_notiHandle);

    LogInfo("core_util_noti subsystem successfuly deinitialized.");
}

std::string EventDeliverySystemDetail::ReadFile(const std::string &fileName) const
{
    LogPedantic("Reading event file: " << fileName << "...");

    std::ifstream fileStream(fileName.c_str(), std::ios::binary);

    if (fileStream.fail())
        return std::string("");

    fileStream.seekg (0, std::ios::end);
    std::streamoff length = fileStream.tellg();
    fileStream.seekg (0, std::ios::beg);

    ScopedArray<char> buffer(new char[length]);
    fileStream.read(buffer.Get(), static_cast<size_t>(length));

    if (fileStream.fail())
        return std::string();

    LogPedantic("Successfuly read: " << fileName << ".");

    return std::string(buffer.Get(), buffer.Get() + length);
}

void EventDeliverySystemDetail::OnNotiGlobalSignal(void *notiData)
{
    Assert(notiData);
    NotiSignal *notiSignal = static_cast<NotiSignal *>(notiData);
    notiSignal->GetReceiver()->OnNotiSignal(notiSignal);
}

void EventDeliverySystemDetail::OnSettingSignal(keynode_t *keyNode, void *userParam)
{
    SettingSignal *settingSignal = static_cast<SettingSignal *>(userParam);
    (void)settingSignal;

    LogPedantic("Got setting signal for key: " << settingSignal->GetKey());

    std::string key = vconf_keynode_get_name(keyNode);

    if (key == VCONFKEY_SETAPPL_STATE_DATA_ROAMING_BOOL)
    {
        int value = vconf_keynode_get_bool(keyNode);
        EventMessages::RoamingChanged message(value > 0);
        EventDeliverySystemInjector::Instance().Publish(message);
    }
    else if (key == VCONFKEY_TELEPHONY_SVC_ROAM)
    {
        int result = 0;
        if(vconf_get_int(VCONFKEY_TELEPHONY_SVC_ROAM, &result) != 0)
        {
            LogPedantic("Cannot get current roaming status");
            return;
        }
        else
        {
            bool homeNetwork = (result != VCONFKEY_TELEPHONY_SVC_ROAM_ON);
            EventMessages::NetworkTypeChanged message1(homeNetwork);
            EventDeliverySystemInjector::Instance().Publish(message1);
        }
    }
    else
    {
        LogPedantic("Unexpected setting signal key: " << key << "!");
        return;
    }
}

void EventDeliverySystemDetail::OnGlobalNotiSignal(void *globalNotiData)
{
    GlobalNotiSignal *globalNotiSignal = static_cast<GlobalNotiSignal *>(globalNotiData);

    LogPedantic("Got global noti signal for key: " << globalNotiSignal->GetKey());

    std::string key = globalNotiSignal->GetKey();

    if (key == GlobalNotiHibernationEnterSignal)
    {
        // We need to disconnect from VCONF
        for (SettingSignalList::iterator iterator = globalNotiSignal->GetReceiver()->m_settingSignalList.begin();
             iterator != globalNotiSignal->GetReceiver()->m_settingSignalList.end(); ++iterator)
        {
            // Unegister from VCONF signals
            int ret = vconf_ignore_key_changed(iterator->GetKey().c_str(), OnSettingSignal);

            if (ret == -1)
                LogPedantic("Failed to unlisten setting: " << key << ", errno = " << errno << ".");
        }

        // Publish
        EventMessages::HibernationEnter message;
        EventDeliverySystemInjector::Instance().Publish(message);
    }
    else if (key == GlobalNotiHibernationLeaveSignal)
    {
        // We need to reconnect to VCONF
        for (SettingSignalList::iterator iterator = globalNotiSignal->GetReceiver()->m_settingSignalList.begin();
             iterator != globalNotiSignal->GetReceiver()->m_settingSignalList.end(); ++iterator)
        {
            // Register for VCONF signals
            int ret = vconf_notify_key_changed(iterator->GetKey().c_str(), OnSettingSignal, &*iterator);

            if (ret == -1)
                LogPedantic("Failed to listen setting: key = " << key << ", errno = " << errno << ".");
        }

        // Publish
        EventMessages::HibernationLeave message;
        EventDeliverySystemInjector::Instance().Publish(message);
    }
    else
    {
        LogPedantic("Unexpected global noti signal key: " << key << "!");
        return;
    }
}

void EventDeliverySystemDetail::OnNotiSignal(NotiSignal *notiSignal)
{
    LogPedantic("Got core_util_noti signal.");

    if (notiSignal->GetEventName().substr(0, strlen(NOTI_EVENT_ID_GENERIC_0)) == NOTI_EVENT_ID_GENERIC_0)
    {
        LogPedantic("GeneticEvent0 message signal.");

        EventDeliverySystemInjector::Instance().PublishMetaType(notiSignal->GetGcid(), ReadFile(GetGenericNotiFilePath(notiSignal->GetEventName())));
    }
    else if (notiSignal->GetEventName().substr(0, strlen(NOTI_EVENT_ID_GENERIC_1)) == NOTI_EVENT_ID_GENERIC_1)
    {
        LogPedantic("GeneticEvent1 message signal.");

        EventDeliverySystemInjector::Instance().PublishMetaType(notiSignal->GetGcid(), ReadFile(GetGenericNotiFilePath(notiSignal->GetEventName())));
    }
    else if (notiSignal->GetEventName().substr(0, strlen(NOTI_EVENT_ID_GENERIC_2)) == NOTI_EVENT_ID_GENERIC_2)
    {
        LogPedantic("GeneticEvent2 message signal.");

        EventDeliverySystemInjector::Instance().PublishMetaType(notiSignal->GetGcid(), ReadFile(GetGenericNotiFilePath(notiSignal->GetEventName())));
    }
    else if (notiSignal->GetEventName().substr(0, strlen(NOTI_EVENT_ID_GENERIC_3)) == NOTI_EVENT_ID_GENERIC_3)
    {
        LogPedantic("GeneticEvent3 message signal.");

        EventDeliverySystemInjector::Instance().PublishMetaType(notiSignal->GetGcid(), ReadFile(GetGenericNotiFilePath(notiSignal->GetEventName())));
    }
    else if (notiSignal->GetEventName().substr(0, strlen(NOTI_EVENT_ID_GENERIC_4)) == NOTI_EVENT_ID_GENERIC_4)
    {
        LogPedantic("GeneticEvent4 message signal.");

        EventDeliverySystemInjector::Instance().PublishMetaType(notiSignal->GetGcid(), ReadFile(GetGenericNotiFilePath(notiSignal->GetEventName())));
    }
}

void EventDeliverySystemDetail::RegisterFileNotiCallback(const char *gcid, const std::string &eventName)
{
    LogPedantic("Registering core_util_noti callback: gcid = " << (gcid ? gcid : "<NULL>") << ", eventName = " << eventName << ".");
    Assert(m_notiHandle != -1);

    // Check if already registered for signal
    NotiSignalList::const_iterator iterator;

    for (iterator = m_notiSignalList.begin(); iterator != m_notiSignalList.end(); ++iterator)
    {
        if (iterator->GetEventName() == eventName)
            return;
    }

    // Make real noti path
    std::string filePath = GetGenericNotiFilePath(eventName);

    LogPedantic("Listening file: " << filePath << ".");

    // New noti signal
    m_notiSignalList.push_back(NotiSignal(this, gcid, eventName));
    NotiSignal *notiSignal = &m_notiSignalList.back();

    // Register for noti signals
    int ret = heynoti_subscribe_file(m_notiHandle, filePath.c_str(), OnNotiGlobalSignal, notiSignal, IN_CLOSE_WRITE);

    if (ret == -1)
    {
        LogPedantic("Failed to listen file: " << filePath << ".");
        return;
    }

    LogPedantic("Successfuly registered listening file: " << filePath << ".");
}

void EventDeliverySystemDetail::UnregisterFileNotiCallback(const std::string &eventName)
{
    LogPedantic("Unegistering core_util_noti callback: eventName = " << eventName << ".");

    Assert(m_notiHandle != -1);

    NotiSignalList::iterator iterator;

    for (iterator = m_notiSignalList.begin(); iterator != m_notiSignalList.end(); ++iterator)
    {
        if (iterator->GetEventName() == eventName)
            break;
    }

    if (iterator == m_notiSignalList.end())
        return;

    // Remove registered slot
    m_notiSignalList.erase(iterator);

    // Make real noti path
    std::string filePath = GetGenericNotiFilePath(eventName);

    LogPedantic("Unlistening file: " << filePath << ".");

    // Register for noti signals
    int ret = heynoti_unsubscribe_file(m_notiHandle, filePath.c_str(), OnNotiGlobalSignal);

    if (ret == -1)
    {
        LogPedantic("Failed to unlisten file: " << filePath << ".");
        return;
    }
}

void EventDeliverySystemDetail::RegisterSettingCallback(const std::string &key)
{
    LogPedantic("Registering setting: key = " << key << ".");

    // Check if already registered for signal
    SettingSignalList::const_iterator iterator;

    for (iterator = m_settingSignalList.begin(); iterator != m_settingSignalList.end(); ++iterator)
    {
        if (iterator->GetKey() == key)
            return;
    }

    // New noti signal
    m_settingSignalList.push_back(SettingSignal(this, key));
    SettingSignal *settingSignal = &m_settingSignalList.back();

    // Register for setting signals
    int ret = vconf_notify_key_changed(key.c_str(), OnSettingSignal, settingSignal);

    if (ret == -1)
    {
        LogPedantic("Failed to listen setting: key = " << key << ", errno = " << errno << ".");
        return;
    }

    LogPedantic("Successfuly registered setting: key = " << key << ".");
}

void EventDeliverySystemDetail::UnregisterSettingCallback(const std::string &key)
{
    LogPedantic("Unegistering setting: key = " << key << ".");

    SettingSignalList::iterator iterator;

    for (iterator = m_settingSignalList.begin(); iterator != m_settingSignalList.end(); ++iterator)
    {
        if (iterator->GetKey() == key)
            break;
    }

    if (iterator == m_settingSignalList.end())
        return;

    // Remove registered slot
    m_settingSignalList.erase(iterator);

    // Register for noti signals
    int ret = vconf_ignore_key_changed(key.c_str(), OnSettingSignal);

    if (ret == -1)
    {
        LogPedantic("Failed to unlisten setting: " << key << ", errno = " << errno << ".");
        return;
    }
}

void EventDeliverySystemDetail::RegisterGlobalNotiCallback(const std::string &key)
{
    LogPedantic("Registering global noti: key = " << key << ".");

    // Check if already registered for signal
    GlobalNotiSignalList::const_iterator iterator;

    for (iterator = m_globalNotiSignalList.begin(); iterator != m_globalNotiSignalList.end(); ++iterator)
    {
        if (iterator->GetKey() == key)
            return;
    }

    // New noti signal
    m_globalNotiSignalList.push_back(GlobalNotiSignal(this, key));
    GlobalNotiSignal *globalNotiSignal = &m_globalNotiSignalList.back();

    // Register for global noti signals
    int ret = heynoti_subscribe(m_notiHandle, key.c_str(), OnGlobalNotiSignal, globalNotiSignal);

    if (ret == -1)
    {
        LogPedantic("Failed to listen global noti: key = " << key << ", errno = " << errno << ".");
        return;
    }

    LogPedantic("Successfuly registered global noti: key = " << key << ".");
}

void EventDeliverySystemDetail::UnregisterGlobalNotiCallback(const std::string &key)
{
    LogPedantic("Unegistering global noti: key = " << key << ".");

    GlobalNotiSignalList::iterator iterator;

    for (iterator = m_globalNotiSignalList.begin(); iterator != m_globalNotiSignalList.end(); ++iterator)
    {
        if (iterator->GetKey() == key)
            break;
    }

    if (iterator == m_globalNotiSignalList.end())
        return;

    // Remove registered slot
    m_globalNotiSignalList.erase(iterator);

    // Register for noti signals
    int ret = heynoti_unsubscribe(m_notiHandle, key.c_str(), OnGlobalNotiSignal);

    if (ret == -1)
    {
        LogPedantic("Failed to unlisten global noti: " << key << ", errno = " << errno << ".");
        return;
    }
}

void EventDeliverySystemDetail::SignalGenericNoti(const std::string &eventName, const std::string &contents) const
{
    LogPedantic("Signaling core_util_noti with: eventName = " << eventName << ", buffer = " << contents << ".");

    Assert(m_notiHandle != -1);

    // Make real noto path
    std::string filePath = GetGenericNotiFilePath(eventName);

    LogPedantic("Signaling file: " << filePath << ".");

    // Emit noti signal with user data
    int fd = open(filePath.c_str(), O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

    if (fd == -1)
    {
        LogPedantic("Failed to signal file: " << filePath << ".");
        return;
    }

    // Write contents of signal to file
    std::string::size_type written = 0;

    while (written < contents.size())
    {
        int done = TEMP_FAILURE_RETRY(write(fd, static_cast<const void *>(&contents[written]), contents.size() - written));

        if (done <= 0)
        {
            LogPedantic("File stream broken: " << filePath << ".");
            break;
        }

        written += done;
    }

    // Close signal file
    int ret = close(fd);

    if (ret == -1)
    {
        LogPedantic("Failed to close signal file: " << filePath << ".");
        return;
    }
}

std::string EventDeliverySystemDetail::GetGenericNotiFilePath(const std::string &eventName) const
{
    // Make sure that the path exists
    if (access(GENERIC_NOTI_ROOT_PATH, F_OK) == -1)
    {
        LogPedantic("No root path found. Creating root path...");

        // Create path
        if (mkdir(GENERIC_NOTI_ROOT_PATH, 0755) == -1)
        {
            LogPedantic("Failed to create root path!");
            return std::string("~") + eventName;
        }

        LogPedantic("Root path created.");
    }

    return std::string(GENERIC_NOTI_ROOT_PATH) + "/" + eventName;
}

void EventDeliverySystemDetail::Listen(const EventMessages::RoamingChanged &event)
{
    LogPedantic("RoamingChanged message listen.");

    (void)event;
    RegisterSettingCallback(VCONFKEY_SETAPPL_STATE_DATA_ROAMING_BOOL);
}

void EventDeliverySystemDetail::Listen(const EventMessages::NetworkTypeChanged &event)
{
    LogPedantic("NetworkTypeChanged message listen.");

    (void)event;
    RegisterSettingCallback(VCONFKEY_TELEPHONY_SVC_ROAM);
}

void EventDeliverySystemDetail::Listen(const EventMessages::HibernationEnter &event)
{
    LogPedantic("HibernationEnter message listen.");

    (void)event;
    RegisterGlobalNotiCallback(GlobalNotiHibernationEnterSignal);
}

void EventDeliverySystemDetail::Listen(const EventMessages::HibernationLeave &event)
{
    LogPedantic("HibernationLeave message listen.");

    (void)event;
    RegisterGlobalNotiCallback(GlobalNotiHibernationLeaveSignal);
}

void EventDeliverySystemDetail::Unlisten(const EventMessages::RoamingChanged &event)
{
    LogPedantic("RoamingChanged message unlisten.");

    (void)event;
    UnregisterSettingCallback(VCONFKEY_SETAPPL_STATE_DATA_ROAMING_BOOL);
}

void EventDeliverySystemDetail::Unlisten(const EventMessages::NetworkTypeChanged &event)
{
    LogPedantic("NetworkTypeChanged message unlisten.");

    (void)event;
    UnregisterSettingCallback(VCONFKEY_TELEPHONY_SVC_ROAM);
}

void EventDeliverySystemDetail::Unlisten(const EventMessages::HibernationEnter &event)
{
    LogPedantic("HibernationEnter message unlisten.");

    (void)event;
    UnregisterGlobalNotiCallback(GlobalNotiHibernationEnterSignal);
}

void EventDeliverySystemDetail::Unlisten(const EventMessages::HibernationLeave &event)
{
    LogPedantic("HibernationLeave message unlisten.");

    (void)event;
    UnregisterGlobalNotiCallback(GlobalNotiHibernationLeaveSignal);
}

}
} // namespace DPL
