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
 * @file        inter_context_delegate.h
 * @author      Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version     1.0
 * @brief       This file is the header file of inter context delegate
 */

#ifndef DPL_INTER_CONTEXT_DELEGATE_H_
#define DPL_INTER_CONTEXT_DELEGATE_H_

#ifndef __GXX_EXPERIMENTAL_CXX0X__ // C++11 compatibility check
# include <bits/c++0x_warning.h>
#else

#include <dpl/event/event_support.h>
#include <dpl/event/thread_event_dispatcher.h>
#include <dpl/event/main_event_dispatcher.h>
#include <dpl/fast_delegate.h>
#include <dpl/generic_event.h>
#include <dpl/foreach.h>
#include <dpl/recursive_mutex.h>
#include <dpl/mutex.h>
#include <dpl/noncopyable.h>
#include <dpl/log/log.h>
#include <dpl/assert.h>
#include <dpl/apply.h>
#include <tuple>
#include <list>
#include <memory>

/*
 * - Created ICDelegate can be passed freely to other threads.
 * - ICDelegate can be called just once. All following calls will be
 *   silently ignored.
 * - When ICDelegateSupport is destroyed, all its ICDelegates
 *   are invalidated and safetly removed.
 * - ICDelegate can be invalidated by call to disable method on it.
 * - To use ICDelegate you have to do two steps:
 *
 * 1. Class that will be used to create delegate have to derive from templated
 *    class ICDelegateSupport<T>
 *
 * 2. Create instance of ICDelegate by calling
 *    makeICDelegate and passing to it pointer to method
 *
 *    class A : ICDelegateSupport<A>
 *    {
 *    void methodA(int) {}
 *    void createDelegate() {
 *        ICDelegate<int> dlg;
 *        dlg = makeICDelegate(&A::MethodA);
 *    };
 */

namespace DPL {
namespace Event {
//forward declaration
template <typename ... ArgTypesList>
class ICDelegate;

namespace ICD {
// This Type defines whether ICDelegate should be destroyed after the call, or
// could be reused later.
enum class Reuse
{
    Yes, No
};
}

namespace ICDPrivate {
// Base for all ICDSharedDatas. Needed for auto disabling and deleting of
// ICDSharedDatas.
// If ICDSharedData is disabled, delegate won't be called anymore.
class ICDSharedDataBase
{
  public:
    typedef std::shared_ptr<ICDSharedDataBase> ICDSharedDataBasePtr;
    typedef std::list<ICDSharedDataBasePtr> ICDSharedDataBaseList;

    class ScopedLock : DPL::Noncopyable
    {
      public:
        explicit ScopedLock(ICDSharedDataBasePtr helperBase) :
            m_scopedLock(&helperBase->m_mutex)
        {}

      private:
        DPL::RecursiveMutex::ScopedLock m_scopedLock;
    };

    ICDSharedDataBase() : m_disabled(false)
    {}
    virtual ~ICDSharedDataBase()
    {}

    bool isDisabled() const
    {
        return m_disabled;
    }
    virtual void disable()
    {
        m_disabled = true;
    }

    void setIterator(ICDSharedDataBaseList::iterator pos)
    {
        m_position = pos;
    }

    ICDSharedDataBaseList::iterator getIterator() const
    {
        return m_position;
    }

  private:
    bool m_disabled;
    DPL::RecursiveMutex m_mutex;
    ICDSharedDataBaseList::iterator m_position;
};

// Pure Event to remove ICDSharedData.
class DeleteICDSharedDataBaseEventCall : public DPL::Event::AbstractEventCall
{
  public:
    DeleteICDSharedDataBaseEventCall(
        ICDSharedDataBase::ICDSharedDataBasePtr helperBase) :
        m_helperBase(helperBase)
    {}
    virtual void Call()
    {
        m_helperBase.reset();
    }

  private:
    ICDSharedDataBase::ICDSharedDataBasePtr m_helperBase;
};

class ICDelegateSupportInterface
{
  protected:
    virtual ~ICDelegateSupportInterface()
    {}
    virtual void unregisterICDSharedData(
        ICDPrivate::ICDSharedDataBase::ICDSharedDataBasePtr helper) = 0;
    virtual void registerICDSharedData(
        ICDPrivate::ICDSharedDataBase::ICDSharedDataBasePtr helper) = 0;

  private:
    template <typename ... ArgTypesList>
    friend class DPL::Event::ICDelegate;
};
} //ICDPrivate

// Better makeDelegate then DPL::MakeDelegate
template<typename ThisType, typename ... ArgTypesList>
FastDelegate<void (ArgTypesList ...)>
makeDelegate(ThisType* This,
             void (ThisType::*Func)(ArgTypesList ...))
{
    return FastDelegate<void (ArgTypesList ...)>(This, Func);
}

// ICDelegate class represents delegate that can be called from
// any context (thread). The actual calling context (thread) is allways the same
// as the context in which it was created.
template <typename ... ArgTypesList>
class ICDelegate
{
  public:
    ICDelegate()
    {}
    ICDelegate(ICDPrivate::ICDelegateSupportInterface* base,
               DPL::FastDelegate<void (ArgTypesList ...)> outerDelegate,
               ICD::Reuse reuse)
    {
        ICDSharedData* hlp = new ICDSharedData(base, outerDelegate, reuse);
        m_helper.reset(hlp);
    }

    // Calling operator will pass all args passed to it to correct context and
    // will call apropriate method that was registered with.
    void operator()(ArgTypesList ... args)
    {
        Assert(m_helper);
        ICDPrivate::ICDSharedDataBase::ScopedLock lock(
            std::static_pointer_cast<ICDPrivate::ICDSharedDataBase>(m_helper));
        m_helper->CallDelegate(args ...);
    }

    //Disable delegate (it won't be called anymore)
    void disable()
    {
        Assert(m_helper);
        ICDPrivate::ICDSharedDataBase::ScopedLock lock(
            std::static_pointer_cast<ICDPrivate::ICDSharedDataBase>(m_helper));
        m_helper->disable();
    }

  protected:
    ICDPrivate::ICDSharedDataBase::ICDSharedDataBasePtr
    getRelatedICDSharedData() const
    {
        return std::static_pointer_cast<ICDPrivate::ICDSharedDataBase>(m_helper);
    }

  private:
    template<typename ThisType>
    friend class ICDelegateSupport;
    class ICDSharedData;
    typedef std::shared_ptr<ICDSharedData> ICDSharedDataPtr;

    struct PrivateEvent
    {
        PrivateEvent(ICDSharedDataPtr a_helper,
                     ArgTypesList ... arguments) :
            helper(a_helper),
            args(std::make_tuple(arguments ...))
        {}

        ICDSharedDataPtr helper;
        std::tuple<ArgTypesList ...> args;
    };

    typedef DPL::FastDelegate<void (const PrivateEvent&)>
    ICDSharedDataDelegateType;
    class ICDSharedData : private DPL::Event::EventSupport<PrivateEvent>,
        public std::enable_shared_from_this<ICDSharedData>,
        public ICDPrivate::ICDSharedDataBase
    {
      public:
        ICDSharedData(
            ICDPrivate::ICDelegateSupportInterface *base,
            DPL::FastDelegate<void (ArgTypesList ...)> outerDelegate,
            ICD::Reuse reuse) :
            m_base(base),
            m_outerDelegate(outerDelegate),
            m_reuse(reuse)
        {
            Assert(m_base);
            // lock is not needed: this object is not shared at that moment
            m_subDelegate =
                DPL::Event::makeDelegate(this,
                                         &ICDSharedData::delegateForwarder);
            EventSupport<PrivateEvent>::AddListener(m_subDelegate);
        }

        void CallDelegate(ArgTypesList ... args)
        {
            ICDPrivate::ICDSharedDataBase::ScopedLock lock(
                std::static_pointer_cast<ICDPrivate::ICDSharedDataBase>(
                    this->shared_from_this()));
            if (!isDisabled()) {
                EmitEvent(PrivateEvent(this->shared_from_this(),
                                       args ...));
            }
        }

        virtual void disable()
        {
            ICDPrivate::ICDSharedDataBase::ICDSharedDataBasePtr ptr(
                std::static_pointer_cast<ICDSharedDataBase>(
                    this->shared_from_this()));
            ICDPrivate::ICDSharedDataBase::ScopedLock lock(ptr);
            if (!isDisabled()) {
                ICDPrivate::ICDSharedDataBase::disable();
                EventSupport<PrivateEvent>::RemoveListener(m_subDelegate);
                m_base->unregisterICDSharedData(ptr);
            }
        }

      private:
        friend class std::shared_ptr<ICDSharedData>;
        ICDSharedDataDelegateType m_subDelegate;
        ICDPrivate::ICDelegateSupportInterface* m_base;
        DPL::FastDelegate<void (ArgTypesList ...)> m_outerDelegate;
        ICD::Reuse m_reuse;

        void delegateForwarder(const PrivateEvent& event)
        {
            ICDPrivate::ICDSharedDataBase::ICDSharedDataBasePtr ptr(
                std::static_pointer_cast<ICDSharedDataBase>(event.helper));
            ICDPrivate::ICDSharedDataBase::ScopedLock lock(ptr);

            Assert(!m_outerDelegate.empty());
            if (ptr->isDisabled()) {
                LogPedantic("ICDSharedData has been disabled - call is ignored");
            } else {
                DPL::Apply(m_outerDelegate, event.args);

                if (m_reuse == ICD::Reuse::Yes) {
                    return;
                }

                disable();
                deleteICDSharedDataBase(ptr);
            }
        }
    };

    // Schedules helper removal.
    static void deleteICDSharedDataBase(
        ICDPrivate::ICDSharedDataBase::ICDSharedDataBasePtr helper)
    {
        using namespace ICDPrivate;
        ICDSharedDataBase::ScopedLock lock(helper);
        DeleteICDSharedDataBaseEventCall* event =
            new DeleteICDSharedDataBaseEventCall(helper);
        if (DPL::Thread::GetCurrentThread() == NULL) {
            DPL::Event::GetMainEventDispatcherInstance().AddEventCall(event);
        } else {
            DPL::Event::ThreadEventDispatcher dispatcher;
            dispatcher.SetThread(DPL::Thread::GetCurrentThread());
            dispatcher.AddEventCall(event);
        }
    }

    ICDSharedDataPtr m_helper;
};

template <typename ThisType>
class ICDelegateSupport : public ICDPrivate::ICDelegateSupportInterface
{
  protected:
    template<typename ... ArgTypesList>
    ICDelegate<ArgTypesList ...> makeICDelegate(
        void (ThisType::*Func)(ArgTypesList ...),
        ICD::Reuse reuse = ICD::Reuse::No)
    {
        ThisType* This = static_cast<ThisType*>(this);
        ICDelegate<ArgTypesList ...> icdelegate(
            This,
            makeDelegate(This, Func),
            reuse);
        this->registerICDSharedData(icdelegate.getRelatedICDSharedData());
        return icdelegate;
    }

    ICDelegateSupport()
    {}

    ~ICDelegateSupport()
    {
        ICDPrivate::ICDSharedDataBase::ICDSharedDataBaseList list =
            m_ICDSharedDatas;
        FOREACH(helper, list) {
            ICDPrivate::ICDSharedDataBase::ScopedLock lock(
                std::static_pointer_cast<ICDPrivate::ICDSharedDataBase>(*helper));
            (*helper)->disable();
        }
        m_ICDSharedDatas.clear();
    }

  private:
    virtual void unregisterICDSharedData(
        ICDPrivate::ICDSharedDataBase::ICDSharedDataBasePtr helper)
    {
        m_ICDSharedDatas.erase(helper->getIterator());
    }

    virtual void registerICDSharedData(
        ICDPrivate::ICDSharedDataBase::ICDSharedDataBasePtr helper)
    {
        helper->setIterator(
            m_ICDSharedDatas.insert(m_ICDSharedDatas.begin(),
                                    helper));
    }

  private:
    ICDPrivate::ICDSharedDataBase::ICDSharedDataBaseList m_ICDSharedDatas;
};
}
} //namespace

#endif // __GXX_EXPERIMENTAL_CXX0X__

#endif //DPL_INTER_CONTEXT_DELEGATE_H_
