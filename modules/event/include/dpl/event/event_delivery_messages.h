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
 * @file        event_delivery_messages.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file of event delivery messages
 */
#ifndef DPL_EVENT_DELIVERY_MESSAGES_H
#define DPL_EVENT_DELIVERY_MESSAGES_H

#include <cstring>
#include <sstream>
#include <string>
#include <vector>

namespace DPL
{
namespace Event
{

namespace EventMessages
{
/**
 * Generic event message base
 *
 * Custom events are derived from these templates
 * Only template GenericN members are serialized
 *
 * Assumptions:
 *
 * 1. Arg* must be default constructible
 * 2. For Arg* there must be defined << and >> operators
 */
class GenericBase
{
private:
    const char *m_gcid; ///< Global class ID
    static const char SEPARATOR = '\254'; ///< Unique serialized data separator
    static const size_t DESERIALIZE_STRING_FIELD_CHUNK_SIZE = 128; ///< Chunk size while parsing serialized event data

protected:
    template<class Type>
    void SerializeField(std::ostream &stream, const Type &type) const
    {
        stream << type;
    }

    void SerializeSeparator(std::ostream &stream) const
    {
        stream << SEPARATOR;
    }

    std::vector<std::string> SplitStream(const std::string &stream) const
    {
        char separator[2] = {};
        separator[0] = SEPARATOR;
        std::vector<std::string> splitVector;

        char *splitTab = new char[stream.size() + 1];
        strcpy(splitTab, stream.c_str());

        // Split serialized stream
        char *savePtr;
        char *splitPtr = strtok_r(splitTab, separator, &savePtr);

        while (splitPtr != NULL)
        {
            splitVector.push_back(std::string(splitPtr));
            splitPtr = strtok_r(NULL, separator, &savePtr);
        }

        delete [] splitTab;

        return splitVector;
    }

    template<class Type>
    Type DeserializeField(std::istream &stream, const Type &) const
    {
        Type type;
        stream >> type;
        return type;
    }

    std::string DeserializeField(std::istream &stream, const std::string &) const
    {
        std::string result;
        char buffer[DESERIALIZE_STRING_FIELD_CHUNK_SIZE];

        for (;;)
        {
            stream.get(buffer, DESERIALIZE_STRING_FIELD_CHUNK_SIZE);

            if (stream.gcount() < 1)
                break;

            result += std::string(buffer, buffer + stream.gcount());
        }

        return result;
    }

public:
    /**
     * Init constructor
     */
    explicit GenericBase(const char *gcid)
        : m_gcid(gcid)
    {
    }

    /**
     * Destructor
     */
    virtual ~GenericBase()
    {
    }

    const char *GetGcid() const { return m_gcid; }

    /**
     * Serialize event n-tuple
     *
     * @return Ouput serialized stream
     * @warning Internal use only
     */
    virtual std::string SerializeType() const = 0;

    /**
     * Deserialize event n-tuple
     *
     * @param[in] stream Input stream to deserialize from
     * @warning Internal use only
     */
    virtual void DeserializeType(const std::string &stream) = 0;
};

class Generic0
    : public GenericBase
{
public:
    /**
     * Init constructor
     */
    explicit Generic0(const char *Gcid)
        : GenericBase(Gcid)
    {
    }

    /**
     * Destructor
     */
    virtual ~Generic0()
    {
    }

    /**
     * Serialize event 0-tuple
     *
     * @return Ouput serialized stream
     * @warning Internal use only
     */
    virtual std::string SerializeType() const
    {
        return std::string();
    }

    /**
     * Deserialize event 0-tuple
     *
     * @param[in] stream Input stream to deserialize from
     * @warning Internal use only
     */
    virtual void DeserializeType(const std::string &stream)
    {
        (void)stream;
    }
};

template<typename Arg0Type>
class Generic1
    : public GenericBase
{
public:
    typedef Arg0Type Arg0;

private:
    Arg0 m_arg0;

public:
    /**
     * Init constructor
     */
    explicit Generic1(const char *Gcid)
        : GenericBase(Gcid),
          m_arg0()
    {
    }

    /**
     * Full init constructor
     */
    explicit Generic1(const char *Gcid, const Arg0 &arg0)
        : GenericBase(Gcid),
          m_arg0(arg0)
    {
    }

    /**
     * Destructor
     */
    virtual ~Generic1()
    {
    }

    /**
     * Retrieve value of argument 0 from tuple
     *
     * @return Argument value
     */
    const Arg0 &GetArg0() const
    {
        return m_arg0;
    }

    /**
     * Set value of argument 0 in tuple
     *
     * @param[in] value Argument value
     */
    void SetArg0(const Arg0 &value)
    {
        m_arg0 = value;
    }

    /**
     * Serialize event 1-tuple
     *
     * @return Ouput serialized stream
     * @warning Internal use only
     */
    virtual std::string SerializeType() const
    {
        std::ostringstream s;
        SerializeField(s, m_arg0);
        return s.str();
    }

    /**
     * Deserialize event 1-tuple
     *
     * @param[in] stream Input stream to deserialize from
     * @warning Internal use only
     */
    virtual void DeserializeType(const std::string &stream)
    {
        std::vector<std::string> splitVector = SplitStream(stream);
        if (splitVector.size() != 1)
            return;
        std::istringstream s0(splitVector[0]);
        m_arg0 = DeserializeField(s0, Arg0());
    }
};

template<typename Arg0Type, typename Arg1Type>
class Generic2
    : public GenericBase
{
public:
    typedef Arg0Type Arg0;
    typedef Arg1Type Arg1;

private:
    Arg0 m_arg0;
    Arg1 m_arg1;

public:
    /**
     * Init constructor
     */
    explicit Generic2(const char *Gcid)
        : GenericBase(Gcid),
          m_arg0(),
          m_arg1()
    {
    }

    /**
     * Full init constructor
     */
    explicit Generic2(const char *Gcid, const Arg0 &arg0, const Arg1 &arg1)
        : GenericBase(Gcid),
          m_arg0(arg0),
          m_arg1(arg1)
    {
    }

    /**
     * Destructor
     */
    virtual ~Generic2()
    {
    }

    /**
     * Retrieve value of argument 0 from tuple
     *
     * @return Argument value
     */
    const Arg0 &GetArg0() const
    {
        return m_arg0;
    }

    /**
     * Set value of argument 0 in tuple
     *
     * @param[in] value Argument value
     */
    void SetArg0(const Arg0 &value)
    {
        m_arg0 = value;
    }

    /**
     * Retrieve value of argument 1 from tuple
     *
     * @return Argument value
     */
    const Arg1 &GetArg1() const
    {
        return m_arg1;
    }

    /**
     * Set value of argument 1 in tuple
     *
     * @param[in] value Argument value
     */
    void SetArg1(const Arg1 &value)
    {
        m_arg1 = value;
    }

    /**
     * Serialize event 2-tuple
     *
     * @return Ouput serialized stream
     * @warning Internal use only
     */
    virtual std::string SerializeType() const
    {
        std::ostringstream s;
        SerializeField(s, m_arg0);
        SerializeSeparator(s);
        SerializeField(s, m_arg1);
        return s.str();
    }

    /**
     * Deserialize event 2-tuple
     *
     * @param[in] stream Input stream to deserialize from
     * @warning Internal use only
     */
    virtual void DeserializeType(const std::string &stream)
    {
        std::vector<std::string> splitVector = SplitStream(stream);
        if (splitVector.size() != 2)
            return;
        std::istringstream s0(splitVector[0]);
        std::istringstream s1(splitVector[1]);
        m_arg0 = DeserializeField(s0, Arg0());
        m_arg1 = DeserializeField(s1, Arg1());
    }
};

template<typename Arg0Type, typename Arg1Type, typename Arg2Type>
class Generic3
    : public GenericBase
{
public:
    typedef Arg0Type Arg0;
    typedef Arg1Type Arg1;
    typedef Arg2Type Arg2;

private:
    Arg0 m_arg0;
    Arg1 m_arg1;
    Arg2 m_arg2;

public:
    /**
     * Init constructor
     */
    explicit Generic3(const char *Gcid)
        : GenericBase(Gcid),
          m_arg0(),
          m_arg1(),
          m_arg2()
    {
    }

    /**
     * Full init constructor
     */
    explicit Generic3(const char *Gcid, const Arg0 &arg0, const Arg1 &arg1, const Arg2 &arg2)
        : GenericBase(Gcid),
          m_arg0(arg0),
          m_arg1(arg1),
          m_arg2(arg2)
    {
    }

    /**
     * Destructor
     */
    virtual ~Generic3()
    {
    }

    /**
     * Retrieve value of argument 0 from tuple
     *
     * @return Argument value
     */
    const Arg0 &GetArg0() const
    {
        return m_arg0;
    }

    /**
     * Set value of argument 0 in tuple
     *
     * @param[in] value Argument value
     */
    void SetArg0(const Arg0 &value)
    {
        m_arg0 = value;
    }

    /**
     * Retrieve value of argument 1 from tuple
     *
     * @return Argument value
     */
    const Arg1 &GetArg1() const
    {
        return m_arg1;
    }

    /**
     * Set value of argument 1 in tuple
     *
     * @param[in] value Argument value
     */
    void SetArg1(const Arg1 &value)
    {
        m_arg1 = value;
    }

    /**
     * Retrieve value of argument 2 from tuple
     *
     * @return Argument value
     */
    const Arg2 &GetArg2() const
    {
        return m_arg2;
    }

    /**
     * Set value of argument 2 in tuple
     *
     * @param[in] value Argument value
     */
    void SetArg2(const Arg2 &value)
    {
        m_arg2 = value;
    }

    /**
     * Serialize event 3-tuple
     *
     * @return Ouput serialized stream
     * @warning Internal use only
     */
    virtual std::string SerializeType() const
    {
        std::ostringstream s;
        SerializeField(s, m_arg0);
        SerializeSeparator(s);
        SerializeField(s, m_arg1);
        SerializeSeparator(s);
        SerializeField(s, m_arg2);
        return s.str();
    }

    /**
     * Deserialize event 3-tuple
     *
     * @param[in] stream Input stream to deserialize from
     * @warning Internal use only
     */
    virtual void DeserializeType(const std::string &stream)
    {
        std::vector<std::string> splitVector = SplitStream(stream);
        if (splitVector.size() != 3)
            return;
        std::istringstream s0(splitVector[0]);
        std::istringstream s1(splitVector[1]);
        std::istringstream s2(splitVector[2]);
        m_arg0 = DeserializeField(s0, Arg0());
        m_arg1 = DeserializeField(s1, Arg1());
        m_arg2 = DeserializeField(s2, Arg2());
    }
};

template<typename Arg0Type, typename Arg1Type, typename Arg2Type, typename Arg3Type>
class Generic4
    : public GenericBase
{
public:
    typedef Arg0Type Arg0;
    typedef Arg1Type Arg1;
    typedef Arg2Type Arg2;
    typedef Arg3Type Arg3;

private:
    Arg0 m_arg0;
    Arg1 m_arg1;
    Arg2 m_arg2;
    Arg3 m_arg3;

public:
    /**
     * Init constructor
     */
    explicit Generic4(const char *Gcid)
        : GenericBase(Gcid),
          m_arg0(),
          m_arg1(),
          m_arg2(),
          m_arg3()
    {
    }

    /**
     * Full init constructor
     */
    explicit Generic4(const char *Gcid, const Arg0 &arg0, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
        : GenericBase(Gcid),
          m_arg0(arg0),
          m_arg1(arg1),
          m_arg2(arg2),
          m_arg3(arg3)
    {
    }

    /**
     * Destructor
     */
    virtual ~Generic4()
    {
    }

    /**
     * Retrieve value of argument 0 from tuple
     *
     * @return Argument value
     */
    const Arg0 &GetArg0() const
    {
        return m_arg0;
    }

    /**
     * Set value of argument 0 in tuple
     *
     * @param[in] value Argument value
     */
    void SetArg0(const Arg0 &value)
    {
        m_arg0 = value;
    }

    /**
     * Retrieve value of argument 1 from tuple
     *
     * @return Argument value
     */
    const Arg1 &GetArg1() const
    {
        return m_arg1;
    }

    /**
     * Set value of argument 1 in tuple
     *
     * @param[in] value Argument value
     */
    void SetArg1(const Arg1 &value)
    {
        m_arg1 = value;
    }

    /**
     * Retrieve value of argument 2 from tuple
     *
     * @return Argument value
     */
    const Arg2 &GetArg2() const
    {
        return m_arg2;
    }

    /**
     * Set value of argument 2 in tuple
     *
     * @param[in] value Argument value
     */
    void SetArg2(const Arg2 &value)
    {
        m_arg2 = value;
    }

    /**
     * Retrieve value of argument 3 from tuple
     *
     * @return Argument value
     */
    const Arg3 &GetArg3() const
    {
        return m_arg3;
    }

    /**
     * Set value of argument 3 in tuple
     *
     * @param[in] value Argument value
     */
    void SetArg3(const Arg3 &value)
    {
        m_arg3 = value;
    }

    /**
     * Serialize event 4-tuple
     *
     * @return Ouput serialized stream
     * @warning Internal use only
     */
    virtual std::string SerializeType() const
    {
        std::ostringstream s;
        SerializeField(s, m_arg0);
        SerializeSeparator(s);
        SerializeField(s, m_arg1);
        SerializeSeparator(s);
        SerializeField(s, m_arg2);
        SerializeSeparator(s);
        SerializeField(s, m_arg3);
        return s.str();
    }

    /**
     * Deserialize event 4-tuple
     *
     * @param[in] stream Input stream to deserialize from
     * @warning Internal use only
     */
    virtual void DeserializeType(const std::string &stream)
    {
        std::vector<std::string> splitVector = SplitStream(stream);
        if (splitVector.size() != 4)
            return;
        std::istringstream s0(splitVector[0]);
        std::istringstream s1(splitVector[1]);
        std::istringstream s2(splitVector[2]);
        std::istringstream s3(splitVector[3]);
        m_arg0 = DeserializeField(s0, Arg0());
        m_arg1 = DeserializeField(s1, Arg1());
        m_arg2 = DeserializeField(s2, Arg2());
        m_arg3 = DeserializeField(s3, Arg3());
    }
};

// Event declarators
#define EVENT_DELIVERY_DECLARE_EVENT_MESSAGE_0(EventType) \
    void EventType##_Inject(const std::string &metaData); \
    struct EventType##_StaticDeclarator \
    { \
        static char Name[0xFF]; \
        EventType##_StaticDeclarator(); \
    }; \
    class EventType : public DPL::Event::EventMessages::Generic0 \
    { \
    public: \
        EventType() : DPL::Event::EventMessages::Generic0(EventType##_StaticDeclarator::Name) {} \
    };

#define EVENT_DELIVERY_DECLARE_EVENT_MESSAGE_1(EventType, EventArg0Type) \
    void EventType##_Inject(const std::string &metaData); \
    struct EventType##_StaticDeclarator \
    { \
        static char Name[0xFF]; \
        EventType##_StaticDeclarator(); \
    }; \
    class EventType : public DPL::Event::EventMessages::Generic1<EventArg0Type> \
    { \
    public: \
        EventType() : DPL::Event::EventMessages::Generic1<EventArg0Type>(EventType##_StaticDeclarator::Name) {} \
        EventType(const Arg0 &arg0) : DPL::Event::EventMessages::Generic1<EventArg0Type>(EventType##_StaticDeclarator::Name, arg0) {} \
    };

#define EVENT_DELIVERY_DECLARE_EVENT_MESSAGE_2(EventType, EventArg0Type, EventArg1Type) \
    void EventType##_Inject(const std::string &metaData); \
    struct EventType##_StaticDeclarator \
    { \
        static char Name[0xFF]; \
        EventType##_StaticDeclarator(); \
    }; \
    class EventType : public DPL::Event::EventMessages::Generic2<EventArg0Type, EventArg1Type> \
    { \
    public: \
        EventType() : DPL::Event::EventMessages::Generic2<EventArg0Type, EventArg1Type>(EventType##_StaticDeclarator::Name) {} \
        EventType(const Arg0 &arg0, const Arg1 &arg1) : DPL::Event::EventMessages::Generic2<EventArg0Type, EventArg1Type>(EventType##_StaticDeclarator::Name, arg0, arg1) {} \
    };

#define EVENT_DELIVERY_DECLARE_EVENT_MESSAGE_3(EventType, EventArg0Type, EventArg1Type, EventArg2Type) \
    void EventType##_Inject(const std::string &metaData); \
    struct EventType##_StaticDeclarator \
    { \
        static char Name[0xFF]; \
        EventType##_StaticDeclarator(); \
    }; \
    class EventType : public DPL::Event::EventMessages::Generic3<EventArg0Type, EventArg1Type, EventArg2Type> \
    { \
    public: \
        EventType() : DPL::Event::EventMessages::Generic3<EventArg0Type, EventArg1Type, EventArg2Type>(EventType##_StaticDeclarator::Name) {} \
        EventType(const Arg0 &arg0, const Arg1 &arg1, const Arg2 &arg2) : DPL::Event::EventMessages::Generic3<EventArg0Type, EventArg1Type, EventArg2Type>(EventType##_StaticDeclarator::Name, arg0, arg1, arg2) {} \
    };

#define EVENT_DELIVERY_DECLARE_EVENT_MESSAGE_4(EventType, EventArg0Type, EventArg1Type, EventArg2Type, EventArg3Type) \
    void EventType##_Inject(const std::string &metaData); \
    struct EventType##_StaticDeclarator \
    { \
        static char Name[0xFF]; \
        EventType##_StaticDeclarator(); \
    }; \
    class EventType : public DPL::Event::EventMessages::Generic4<EventArg0Type, EventArg1Type, EventArg2Type, EventArg3Type> \
    { \
    public: \
        EventType() : DPL::Event::EventMessages::Generic4<EventArg0Type, EventArg1Type, EventArg2Type, EventArg3Type>(EventType##_StaticDeclarator::Name) {} \
        EventType(const Arg0 &arg0, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3) : DPL::Event::EventMessages::Generic4<EventArg0Type, EventArg1Type, EventArg2Type, EventArg3Type>(EventType##_StaticDeclarator::Name, arg0, arg1, arg2, arg3) {} \
    };

#define EVENT_DELIVERY_IMPLEMENT_EVENT_MESSAGE(EventType) \
    char EventType##_StaticDeclarator::Name[0xFF] = {}; \
    void EventType##_Inject(const std::string &metaData) \
    { \
        EventType newType; \
        newType.DeserializeType(metaData); \
        DPL::Event::EventDeliverySystemInjector::Instance().Publish(newType); \
    } \
    EventType##_StaticDeclarator::EventType##_StaticDeclarator() \
    { \
        strcpy(Name, #EventType); \
        DPL::Event::EventDeliverySystemInjector::Instance().RegisterMetaCall(EventType##_StaticDeclarator::Name, EventType##_Inject); \
    } \
    namespace { EventType##_StaticDeclarator dummyInitializer_##EventType##_StaticDeclarator; }

// BUILT-IN EVENTS
class RoamingChanged
{
protected:
    bool m_enabled; ///< Status whether roaming is enabled during call

public:
    /**
     * Constructor
     */
    RoamingChanged()
        : m_enabled(false)
    {
    }

    /**
     * Init constructor
     */
    RoamingChanged(bool enabled)
        : m_enabled(enabled)
    {
    }

    /**
     * Destructor
     */
    virtual ~RoamingChanged()
    {
    }

    /**
     * Retrieve status whether roaming is enabled
     *
     * @return Current roaming status
     */
    bool GetEnabled() const
    {
        return m_enabled;
    }
};

class NetworkTypeChanged
{
protected:
   /**
    * Status whether phone is working in:
    * home network (true)
    * roaming (false)
    */
    bool m_homeNetwork;

public:
    /**
     * Constructor
     */
    NetworkTypeChanged()
        : m_homeNetwork(true)
    {
    }

    /**
     * Init constructor
     */
    NetworkTypeChanged(bool homeNetwork)
        : m_homeNetwork(homeNetwork)
    {
    }

    /**
     * Destructor
     */
    virtual ~NetworkTypeChanged()
    {
    }

    /**
     * Retrieve status whether home network is active
     *
     * @return Current network status
     */
    bool IsHomeNetwork() const
    {
        return m_homeNetwork;
    }

    /**
     * Retrieve status whether home network is active
     *
     * @return Current network status
     */
    bool IsRoaming() const
    {
        return !m_homeNetwork;
    }
};

class HibernationEnter
{
public:
    /**
     * Constructor
     */
    HibernationEnter()
    {
    }

    /**
     * Destructor
     */
    virtual ~HibernationEnter()
    {
    }
};

class HibernationLeave
{
public:
    /**
     * Constructor
     */
    HibernationLeave()
    {
    }

    /**
     * Destructor
     */
    virtual ~HibernationLeave()
    {
    }
};

} // namespace EventMessages
}
} // namespace DPL

#endif // DPL_EVENT_DELIVERY_MESSAGES_H
