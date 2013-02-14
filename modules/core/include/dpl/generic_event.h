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
 * @file        generic_event.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of MVC generic event
 */
#ifndef DPL_GENERIC_EVENT_H
#define DPL_GENERIC_EVENT_H

namespace DPL {
class EventSender
{
  public:
    explicit EventSender(void *sender) :
        m_sender(sender)
    {}

    void* GetSender() const
    {
        return m_sender;
    }

  private:
    void *m_sender;
};

class GenericEvent
{
  protected:
    void *m_sender;

  public:
    explicit GenericEvent(const EventSender &sender) :
        m_sender(sender.GetSender())
    {}

    virtual ~GenericEvent()
    {}

    void *GetSender() const
    {
        return m_sender;
    }
};

class GenericEvent0 :
    public GenericEvent
{
  public:
    explicit GenericEvent0(const EventSender &sender) :
        GenericEvent(sender)
    {}

    virtual ~GenericEvent0()
    {}
};

template<typename Arg0Type>
class GenericEvent1 :
    public GenericEvent0
{
  public:
    typedef Arg0Type Arg0;

  protected:
    Arg0 m_arg0;

  public:
    explicit GenericEvent1(const EventSender &sender) :
        GenericEvent0(sender)
    {}

    GenericEvent1(Arg0 arg0, const EventSender &sender) :
        GenericEvent0(sender),
        m_arg0(arg0)
    {}

    virtual ~GenericEvent1()
    {}

    Arg0 GetArg0() const
    {
        return m_arg0;
    }
};

template<typename Arg0Type, typename Arg1Type>
class GenericEvent2 :
    public GenericEvent1<Arg0Type>
{
  public:
    typedef Arg0Type Arg0;
    typedef Arg1Type Arg1;

  protected:
    Arg1 m_arg1;

  public:
    explicit GenericEvent2(const EventSender &sender) :
        GenericEvent1<Arg0Type>(sender)
    {}

    GenericEvent2(Arg0 arg0, Arg1 arg1, const EventSender &sender) :
        GenericEvent1<Arg0Type>(arg0, sender),
        m_arg1(arg1)
    {}

    virtual ~GenericEvent2()
    {}

    Arg1 GetArg1() const
    {
        return m_arg1;
    }
};

template<typename Arg0Type, typename Arg1Type, typename Arg2Type>
class GenericEvent3 :
    public GenericEvent2<Arg0Type, Arg1Type>
{
  public:
    typedef Arg0Type Arg0;
    typedef Arg1Type Arg1;
    typedef Arg2Type Arg2;

  protected:
    Arg2 m_arg2;

  public:
    explicit GenericEvent3(const EventSender &sender) :
        GenericEvent2<Arg0Type, Arg1Type>(sender)
    {}

    GenericEvent3(Arg0 arg0, Arg1 arg1, Arg2 arg2, const EventSender &sender) :
        GenericEvent2<Arg0Type, Arg1Type>(arg0, arg1, sender),
        m_arg2(arg2)
    {}

    virtual ~GenericEvent3()
    {}

    Arg2 GetArg2() const
    {
        return m_arg2;
    }
};

template<typename Arg0Type, typename Arg1Type, typename Arg2Type,
         typename Arg3Type>
class GenericEvent4 :
    public GenericEvent3<Arg0Type, Arg1Type, Arg2Type>
{
  public:
    typedef Arg0Type Arg0;
    typedef Arg1Type Arg1;
    typedef Arg2Type Arg2;
    typedef Arg3Type Arg3;

  protected:
    Arg3 m_arg3;

  public:
    explicit GenericEvent4(const EventSender &sender) :
        GenericEvent3<Arg0Type, Arg1Type, Arg2Type>(sender)
    {}

    GenericEvent4(Arg0 arg0,
                  Arg1 arg1,
                  Arg2 arg2,
                  Arg3 arg3,
                  const EventSender &sender) :
        GenericEvent3<Arg0Type, Arg1Type, Arg2Type>(arg0, arg1, arg2, sender),
        m_arg3(arg3)
    {}

    virtual ~GenericEvent4()
    {}

    Arg3 GetArg3() const
    {
        return m_arg3;
    }
};

template<typename Arg0Type, typename Arg1Type, typename Arg2Type,
         typename Arg3Type, typename Arg4Type>
class GenericEvent5 :
    public GenericEvent4<Arg0Type, Arg1Type, Arg2Type, Arg3Type>
{
  public:
    typedef Arg0Type Arg0;
    typedef Arg1Type Arg1;
    typedef Arg2Type Arg2;
    typedef Arg3Type Arg3;
    typedef Arg4Type Arg4;

  protected:
    Arg4 m_arg4;

  public:
    explicit GenericEvent5(const EventSender &sender) :
        GenericEvent4<Arg0Type, Arg1Type, Arg2Type, Arg3Type>(sender)
    {}

    GenericEvent5(Arg0 arg0,
                  Arg1 arg1,
                  Arg2 arg2,
                  Arg3 arg3,
                  Arg4 arg4,
                  const EventSender &sender) :
        GenericEvent4<Arg0Type, Arg1Type, Arg2Type, Arg3Type>(arg0, arg1, arg2,
                                                              arg3, sender),
        m_arg4(arg4)
    {}

    virtual ~GenericEvent5()
    {}

    Arg4 GetArg4() const
    {
        return m_arg4;
    }
};

template<typename Arg0Type, typename Arg1Type, typename Arg2Type,
         typename Arg3Type, typename Arg4Type, typename Arg5Type>
class GenericEvent6 :
    public GenericEvent5<Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type>
{
  public:
    typedef Arg0Type Arg0;
    typedef Arg1Type Arg1;
    typedef Arg2Type Arg2;
    typedef Arg3Type Arg3;
    typedef Arg4Type Arg4;
    typedef Arg5Type Arg5;

  protected:
    Arg5 m_arg5;

  public:
    explicit GenericEvent6(const EventSender &sender) :
        GenericEvent5<Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type>(sender)
    {}

    GenericEvent6(Arg0 arg0,
                  Arg1 arg1,
                  Arg2 arg2,
                  Arg3 arg3,
                  Arg4 arg4,
                  Arg5 arg5,
                  const EventSender &sender) :
        GenericEvent5<Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type>(arg0,
                                                                        arg1,
                                                                        arg2,
                                                                        arg3,
                                                                        arg4,
                                                                        sender),
        m_arg5(arg5)
    {}

    virtual ~GenericEvent6()
    {}

    Arg5 GetArg5() const
    {
        return m_arg5;
    }
};

template<typename Arg0Type, typename Arg1Type, typename Arg2Type,
         typename Arg3Type, typename Arg4Type, typename Arg5Type,
         typename Arg6Type>
class GenericEvent7 :
    public GenericEvent6<Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type,
                         Arg5Type>
{
  public:
    typedef Arg0Type Arg0;
    typedef Arg1Type Arg1;
    typedef Arg2Type Arg2;
    typedef Arg3Type Arg3;
    typedef Arg4Type Arg4;
    typedef Arg5Type Arg5;
    typedef Arg6Type Arg6;

  protected:
    Arg6 m_arg6;

  public:
    explicit GenericEvent7(const EventSender &sender) :
        GenericEvent6<Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type,
                      Arg5Type>(sender)
    {}

    GenericEvent7(Arg0 arg0,
                  Arg1 arg1,
                  Arg2 arg2,
                  Arg3 arg3,
                  Arg4 arg4,
                  Arg5 arg5,
                  Arg6 arg6,
                  const EventSender &sender) :
        GenericEvent6<Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type,
                      Arg5Type>(arg0, arg1, arg2, arg3, arg4, arg5, sender),
        m_arg6(arg6)
    {}

    virtual ~GenericEvent7()
    {}

    Arg6 GetArg6() const
    {
        return m_arg6;
    }
};

template<typename Arg0Type, typename Arg1Type, typename Arg2Type,
         typename Arg3Type, typename Arg4Type, typename Arg5Type,
         typename Arg6Type,
         typename Arg7Type>
class GenericEvent8 :
    public GenericEvent7<Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type,
                         Arg5Type, Arg6Type>
{
  public:
    typedef Arg0Type Arg0;
    typedef Arg1Type Arg1;
    typedef Arg2Type Arg2;
    typedef Arg3Type Arg3;
    typedef Arg4Type Arg4;
    typedef Arg5Type Arg5;
    typedef Arg6Type Arg6;
    typedef Arg7Type Arg7;

  protected:
    Arg7 m_arg7;

  public:
    explicit GenericEvent8(const EventSender &sender) :
        GenericEvent7<Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type,
                      Arg5Type, Arg6Type>(sender)
    {}

    GenericEvent8(Arg0 arg0,
                  Arg1 arg1,
                  Arg2 arg2,
                  Arg3 arg3,
                  Arg4 arg4,
                  Arg5 arg5,
                  Arg6 arg6,
                  Arg7 arg7,
                  const EventSender &sender) :
        GenericEvent7<Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type,
                      Arg5Type, Arg6Type>(arg0, arg1, arg2, arg3, arg4, arg5,
                                          arg6, sender),
        m_arg7(arg7)
    {}

    virtual ~GenericEvent8()
    {}

    Arg7 GetArg7() const
    {
        return m_arg7;
    }
};
} // namespace DPL

#define DECLARE_GENERIC_EVENT_0(ClassName)                                  \
    class ClassName :                                                                   \
        public DPL::GenericEvent0                                         \
    {                                                                       \
      public:                                                                 \
        explicit ClassName(const DPL::EventSender & sender = \
                               DPL::EventSender(NULL)) :                                                                       \
            DPL::GenericEvent0(sender)                                    \
        {                                                                   \
        }                                                                   \
    };

#define DECLARE_GENERIC_EVENT_1(ClassName, Arg0Type)                          \
    class ClassName :                                                                     \
        public DPL::GenericEvent1<Arg0Type>                                 \
    {                                                                         \
      public:                                                                   \
        explicit ClassName(const DPL::EventSender & sender = \
                               DPL::EventSender(NULL)) :                                                                       \
            DPL::GenericEvent1<Arg0Type>(sender)                            \
        {                                                                     \
        }                                                                     \
                                                                              \
        explicit ClassName(Arg0Type arg0,                                     \
                           const DPL::EventSender & sender = \
                               DPL::EventSender(NULL)) :                                                                 \
            DPL::GenericEvent1<Arg0Type>(arg0, sender)                      \
        {                                                                     \
        }                                                                     \
    };

#define DECLARE_GENERIC_EVENT_2(ClassName, Arg0Type, Arg1Type)                \
    class ClassName :                                                                     \
        public DPL::GenericEvent2<Arg0Type, Arg1Type>                       \
    {                                                                         \
      public:                                                                   \
        explicit ClassName(const DPL::EventSender & sender = \
                               DPL::EventSender(NULL)) :                                                                       \
            DPL::GenericEvent2<Arg0Type, Arg1Type>(sender)                  \
        {                                                                     \
        }                                                                     \
                                                                              \
        ClassName(Arg0Type arg0, Arg1Type arg1,                               \
                  const DPL::EventSender & sender = DPL::EventSender(NULL)) :                                                                 \
            DPL::GenericEvent2<Arg0Type, Arg1Type>(arg0, arg1, sender)      \
        {                                                                     \
        }                                                                     \
    };

#define DECLARE_GENERIC_EVENT_3(ClassName, Arg0Type, Arg1Type, Arg2Type)                      \
    class ClassName :                                                                                     \
        public DPL::GenericEvent3<Arg0Type, Arg1Type, Arg2Type>                             \
    {                                                                                         \
      public:                                                                                   \
        explicit ClassName(const DPL::EventSender & sender = \
                               DPL::EventSender(NULL)) :                                                                                 \
            DPL::GenericEvent3<Arg0Type, Arg1Type, Arg2Type>(sender)                        \
        {                                                                                     \
        }                                                                                     \
                                                                                              \
        ClassName(Arg0Type arg0, Arg1Type arg1, Arg2Type arg2,                                \
                  const DPL::EventSender & sender = DPL::EventSender(NULL)) :                                                                                 \
            DPL::GenericEvent3<Arg0Type, Arg1Type, Arg2Type>(arg0, \
                                                             arg1, \
                                                             arg2, \
                                                             sender)      \
        {                                                                                     \
        }                                                                                     \
    };

#define DECLARE_GENERIC_EVENT_4(ClassName, \
                                Arg0Type, \
                                Arg1Type, \
                                Arg2Type, \
                                Arg3Type)                            \
    class ClassName :                                                                                                     \
        public DPL::GenericEvent4<Arg0Type, Arg1Type, Arg2Type, Arg3Type>                                   \
    {                                                                                                         \
      public:                                                                                                   \
        explicit ClassName(const DPL::EventSender & sender = \
                               DPL::EventSender(NULL)) :                                                                                                 \
            DPL::GenericEvent4<Arg0Type, Arg1Type, Arg2Type, Arg3Type>(sender)                              \
        {                                                                                                     \
        }                                                                                                     \
                                                                                                              \
        ClassName(Arg0Type arg0, Arg1Type arg1, Arg2Type arg2, Arg3Type arg3,                                 \
                  const DPL::EventSender & sender = DPL::EventSender(NULL)) :                                                                                                 \
            DPL::GenericEvent4<Arg0Type, Arg1Type, Arg2Type, Arg3Type>(arg0, \
                                                                       arg1, \
                                                                       arg2, \
                                                                       arg3, \
                                                                       sender)      \
        {                                                                                                     \
        }                                                                                                     \
    };

#define DECLARE_GENERIC_EVENT_5(ClassName, \
                                Arg0Type, \
                                Arg1Type, \
                                Arg2Type, \
                                Arg3Type, \
                                Arg4Type)                                  \
    class ClassName :                                                                                                                     \
        public DPL::GenericEvent5<Arg0Type, Arg1Type, Arg2Type, Arg3Type, \
                                  Arg4Type>                                         \
    {                                                                                                                         \
      public:                                                                                                                   \
        explicit ClassName(const DPL::EventSender & sender = \
                               DPL::EventSender(NULL)) :                                                                                                                 \
            DPL::GenericEvent5<Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type>( \
                sender)                                    \
        {                                                                                                                     \
        }                                                                                                                     \
                                                                                                                              \
        ClassName(Arg0Type arg0, \
                  Arg1Type arg1, \
                  Arg2Type arg2, \
                  Arg3Type arg3, \
                  Arg4Type arg4,                                  \
                  const DPL::EventSender & sender = DPL::EventSender(NULL)) :                                                                                                                 \
            DPL::GenericEvent5<Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type>( \
                arg0, \
                arg1, \
                arg2, \
                arg3, \
                arg4, \
                sender)      \
        {                                                                                                                     \
        }                                                                                                                     \
    };

#define DECLARE_GENERIC_EVENT_6(ClassName, \
                                Arg0Type, \
                                Arg1Type, \
                                Arg2Type, \
                                Arg3Type, \
                                Arg4Type, \
                                Arg5Type)                                        \
    class ClassName :                                                                                                                                     \
        public DPL::GenericEvent6<Arg0Type, Arg1Type, Arg2Type, Arg3Type, \
                                  Arg4Type, Arg5Type>                                               \
    {                                                                                                                                         \
      public:                                                                                                                                   \
        explicit ClassName(const DPL::EventSender & sender = \
                               DPL::EventSender(NULL)) :                                                                                                                                 \
            DPL::GenericEvent6<Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type, \
                               Arg5Type>(sender)                                          \
        {                                                                                                                                     \
        }                                                                                                                                     \
                                                                                                                                              \
        ClassName(Arg0Type arg0, \
                  Arg1Type arg1, \
                  Arg2Type arg2, \
                  Arg3Type arg3, \
                  Arg4Type arg4, \
                  Arg5Type arg5,                                   \
                  const DPL::EventSender & sender = DPL::EventSender(NULL)) :                                                                                                                                 \
            DPL::GenericEvent6<Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type, \
                               Arg5Type>(arg0, \
                                         arg1, \
                                         arg2, \
                                         arg3, \
                                         arg4, \
                                         arg5, \
                                         sender)      \
        {                                                                                                                                     \
        }                                                                                                                                     \
    };

#define DECLARE_GENERIC_EVENT_7(ClassName, \
                                Arg0Type, \
                                Arg1Type, \
                                Arg2Type, \
                                Arg3Type, \
                                Arg4Type, \
                                Arg5Type, \
                                Arg6Type)                                              \
    class ClassName :                                                                                                                                                     \
        public DPL::GenericEvent7<Arg0Type, Arg1Type, Arg2Type, Arg3Type, \
                                  Arg4Type, Arg5Type, Arg6Type>                                                     \
    {                                                                                                                                                         \
      public:                                                                                                                                                   \
        explicit ClassName(const DPL::EventSender & sender = \
                               DPL::EventSender(NULL)) :                                                                                                                                                 \
            DPL::GenericEvent7<Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type, \
                               Arg5Type, Arg6Type>(sender)                                                \
        {                                                                                                                                                     \
        }                                                                                                                                                     \
                                                                                                                                                              \
        ClassName(Arg0Type arg0, \
                  Arg1Type arg1, \
                  Arg2Type arg2, \
                  Arg3Type arg3, \
                  Arg4Type arg4, \
                  Arg5Type arg5, \
                  Arg6Type arg6,                                    \
                  const DPL::EventSender & sender = DPL::EventSender(NULL)) :                                                                                                                                                 \
            DPL::GenericEvent7<Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type, \
                               Arg5Type, Arg6Type>(arg0, \
                                                   arg1, \
                                                   arg2, \
                                                   arg3, \
                                                   arg4, \
                                                   arg5, \
                                                   arg6, \
                                                   sender)      \
        {                                                                                                                                                     \
        }                                                                                                                                                     \
    };

#define DECLARE_GENERIC_EVENT_8(ClassName, \
                                Arg0Type, \
                                Arg1Type, \
                                Arg2Type, \
                                Arg3Type, \
                                Arg4Type, \
                                Arg5Type, \
                                Arg6Type, \
                                Arg7Type)                                                    \
    class ClassName :                                                                                                                                                                     \
        public DPL::GenericEvent8<Arg0Type, Arg1Type, Arg2Type, Arg3Type, \
                                  Arg4Type, Arg5Type, Arg6Type, Arg7Type>                                                           \
    {                                                                                                                                                                         \
      public:                                                                                                                                                                   \
        explicit ClassName(const DPL::EventSender & sender = \
                               DPL::EventSender(NULL)) :                                                                                                                                                                 \
            DPL::GenericEvent8<Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type, \
                               Arg5Type, Arg6Type, Arg7Type>(sender)                                                      \
        {                                                                                                                                                                     \
        }                                                                                                                                                                     \
                                                                                                                                                                              \
        ClassName(Arg0Type arg0, \
                  Arg1Type arg1, \
                  Arg2Type arg2, \
                  Arg3Type arg3, \
                  Arg4Type arg4, \
                  Arg5Type arg5, \
                  Arg6Type arg6, \
                  Arg7Type arg7,                                     \
                  const DPL::EventSender & sender = DPL::EventSender(NULL)) :                                                                                                                                                                 \
            DPL::GenericEvent8<Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type, \
                               Arg5Type, Arg6Type, Arg7Type>(arg0, \
                                                             arg1, \
                                                             arg2, \
                                                             arg3, \
                                                             arg4, \
                                                             arg5, \
                                                             arg6, \
                                                             arg7, \
                                                             sender)      \
        {                                                                                                                                                                     \
        }                                                                                                                                                                     \
    };

#endif // DPL_GENERIC_EVENT_H
