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
 * @file    test_shm.h
 * @author  Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @version 1.0
 * @brief   Implementation file for test cases for shared data framework
 */

#include <stdlib.h>
#include <ctime>
#include <sys/shm.h>
#include <dpl/semaphore.h>
#include <dpl/test_runner.h>
#include <dpl/thread.h>
#include <dpl/controller.h>
#include <dpl/generic_event.h>
#include <dpl/log.h>
#include <dpl/shared_object.h>
#include <dpl/shared_property.h>
#include <memory>

RUNNER_TEST_GROUP_INIT(DPL)

using namespace DPL;

namespace {
const SharedMemory::Key SHM_KEY = 12345;
const char* SEM_NAME = "/wrt_engine_shared_object_semaphore";
const size_t VERSION = 1;

const size_t MAX_THREADS = 10;
const size_t TEST_AND_SET_REPEATS = 100;

const size_t SHARED_PROP_REPEATS = 3;

const size_t SINGLETON_TEST_REPEATS = 3;

// maximum random delay in singleton listener addition/removal
const size_t MAX_SINGLETON_LISTENER_DELAY = 50;

const int SINGLE_PROCESS_REPEATS = 50;

/*
 * 5 seconds expected timeout for waitable events
 * 30 seconds unexpected timeout for waitable events
 * We don't want to block tests
 */
const size_t EXPECTED_WAITABLE_TIMEOUT = 5 * 1000;
const size_t UNEXPECTED_WAITABLE_TIMEOUT = 30 * 1000;

bool g_enumTestCorrect = false;
bool g_enumTestIncorrect = false;
size_t g_delegateCalls = 0;

void Wait(DPL::WaitableEvent& event, bool expectedTimeout = false)
{
    LogDebug("WaitForSingleHandle...");
    DPL::WaitableHandleIndexList list = DPL::WaitForSingleHandle(
            event.GetHandle(),
            expectedTimeout ?
            EXPECTED_WAITABLE_TIMEOUT : UNEXPECTED_WAITABLE_TIMEOUT);
    if (list.size() == 0) {
        LogDebug("...timeout.");
    } else {
        LogDebug("...signaled.");
        event.Reset();
    }

    if (expectedTimeout) {
        RUNNER_ASSERT(list.size() == 0);
    } else {
        RUNNER_ASSERT(list.size() == 1);
    }
}

void RemoveIpcs()
{
    Try {
        SharedMemory::Remove(SHM_KEY);
    }
    Catch(SharedMemory::Exception::RemoveFailed) {
        // ignore
    }

    Try {
        DPL::Semaphore::Remove(SEM_NAME);
    }
    Catch(DPL::Semaphore::Exception::RemoveFailed) {
        // ignore
    }
}

typedef DPL::TypeListDecl<int, int, char, int[64]>::Type TestTypeList;
typedef DPL::TypeListDecl<int, int, char, int[63]>::Type TestTypeList2;
typedef DPL::TypeListDecl<int, int, char, int[63], int>::Type TestTypeList3;

typedef SharedObject<TestTypeList> TestSharedObject;
typedef SharedObject<TestTypeList2> TestSharedObject2;
typedef SharedObject<TestTypeList3> TestSharedObject3;

typedef std::shared_ptr<TestSharedObject> TestSharedObjectPtr;

const int INIT_EVENT = 0;
const int DESTROY_EVENT = 1;

int g_values[TestTypeList::Size];

/*
 * helper listening controller
 */
template <typename SharedType>
class ListeningController :
    public DPL::Controller<DPL::TypeListDecl<int>::Type>
{
  public:
    explicit ListeningController(DPL::WaitableEvent* waitable);
    ~ListeningController();

    virtual void OnEventReceived(const int &event);

    virtual void OnEvent(const int /*event*/) {}

  protected:
    std::shared_ptr<SharedType> m_so;
    DPL::Thread m_thread;
    DPL::WaitableEvent* m_waitable;
};

template <typename SharedType>
ListeningController<SharedType>::ListeningController(
    DPL::WaitableEvent* waitable) :
    m_waitable(waitable)
{
    Touch();
    m_thread.Run();
    SwitchToThread(&m_thread);
    PostEvent(INIT_EVENT);
}

template <typename SharedType>
ListeningController<SharedType>::~ListeningController()
{
    m_thread.Quit();
}

template <typename SharedType>
void ListeningController<SharedType>::OnEventReceived(const int& event)
{
    if (event == INIT_EVENT) {
        m_so = SharedObjectFactory<SharedType>::Create(SHM_KEY, SEM_NAME);
        OnEvent(event);
        m_waitable->Signal();
    } else if (event == DESTROY_EVENT) {
        LogDebug("Destroying shared object");
        OnEvent(event);

        // deregister, destroy ad notify main thread
        m_so.Reset();
        LogDebug("4");
        m_waitable->Signal();
        LogDebug("5");
    } else {
        OnEvent(event);
    }
}

typedef DPL::TypeListDecl<size_t, bool>::Type SharedTypeList;

class TestSharedObject4;
typedef std::shared_ptr<TestSharedObject4> TestSharedObject4Ptr;

class TestSharedObject4 : public SharedObject<SharedTypeList>
{
  public:
    enum
    {
        SIZE_T,
        BOOLEAN
    };

    static TestSharedObject4Ptr Create()
    {
        return SharedObjectFactory<TestSharedObject4>::Create(SHM_KEY, SEM_NAME);
    }

    ~TestSharedObject4()
    {
        LogDebug("dtor");
    }

  protected:
    explicit TestSharedObject4(const std::string& semaphore) :
        SharedObject<SharedTypeList>(semaphore)
    {}

  private:
    void Init()
    {
        SetPropertyInternal<BOOLEAN>(false);
    }
    friend class SharedObjectFactory<TestSharedObject4>;
};
} // anonymus namespace

//////////////////////////////////////////////

RUNNER_TEST(SharedMemory_002_AccessByType)
{
    RemoveIpcs();

    SharedData<TestTypeList> str;

    // access by type
    str.Embedded<0, int>::value = 4;
    str.Embedded<1, int>::value = 5;
    str.Embedded<2, char>::value = 'd';
    str.Embedded<3, int[64]>::value[0] = 1;
    str.Embedded<3, int[64]>::value[1] = 20;

    RUNNER_ASSERT((str.Embedded<0, int>::value) == 4);
    RUNNER_ASSERT((str.Embedded<1, int>::value) == 5);
    RUNNER_ASSERT((str.Embedded<2, char>::value) == 'd');
    RUNNER_ASSERT((str.Embedded<3, int[64]>::value[0]) == 1);
    RUNNER_ASSERT((str.Embedded<3, int[64]>::value[1]) == 20);
}

//////////////////////////////////////////////

RUNNER_TEST(SharedMemory_003_AccessByIndex)
{
    RemoveIpcs();

    SharedData<TestTypeList> str;
    // access by enum
    str.Embedded<0, TestTypeList::Element<0>::Type>::value = 4;
    str.Embedded<1, TestTypeList::Element<1>::Type>::value = 5;
    str.Embedded<2, TestTypeList::Element<2>::Type>::value = 'd';
    str.Embedded<3, TestTypeList::Element<3>::Type>::value[0] = 1;
    str.Embedded<3, TestTypeList::Element<3>::Type>::value[1] = 20;

    RUNNER_ASSERT(
        (str.Embedded<0, TestTypeList::Element<0>::Type>::value) == 4);
    RUNNER_ASSERT(
        (str.Embedded<1, TestTypeList::Element<1>::Type>::value) == 5);
    RUNNER_ASSERT(
        (str.Embedded<2, TestTypeList::Element<2>::Type>::value) == 'd');
    RUNNER_ASSERT(
        (str.Embedded<3, TestTypeList::Element<3>::Type>::value[0]) == 1);
    RUNNER_ASSERT(
        (str.Embedded<3, TestTypeList::Element<3>::Type>::value[1]) == 20);
}

//////////////////////////////////////////////

RUNNER_TEST(SharedMemory_004_SimplifiedAccess)
{
    RemoveIpcs();

    SharedData<TestTypeList> str;

    // access via PropertyRef
    str.PropertyRef<1>() = 3;
    RUNNER_ASSERT(str.PropertyRef<1>() == 3);

    int (&array)[64] = str.PropertyRef<3>();
    array[0] = 2;
    RUNNER_ASSERT(str.PropertyRef<3>()[0] == 2);

    str.PropertyRef<3>()[1] = 19;
    RUNNER_ASSERT(str.PropertyRef<3>()[1] == 19);

    // access via macro
    str.SHARED_PROPERTY(0) = 2;
    RUNNER_ASSERT(str.SHARED_PROPERTY(0) == 2);

    str.SHARED_PROPERTY(2) = 'c';
    RUNNER_ASSERT(str.SHARED_PROPERTY(2) == 'c');

    str.SHARED_PROPERTY(3)[2] = 10;
    RUNNER_ASSERT(str.SHARED_PROPERTY(3)[2] == 10);

    // old style check
    RUNNER_ASSERT((str.Embedded<0, int>::value) == 2);
    RUNNER_ASSERT((str.Embedded<1, int>::value) == 3);
    RUNNER_ASSERT((str.Embedded<2, char>::value) == 'c');
    RUNNER_ASSERT((str.Embedded<3, int[64]>::value[0]) == 2);
    RUNNER_ASSERT((str.Embedded<3, int[64]>::value[1]) == 19);
    RUNNER_ASSERT((str.Embedded<3, int[64]>::value[2]) == 10);
}

//////////////////////////////////////////////

struct SharedStruct
{
    int a;
    int b;
    char c;
    int d[64];
};

typedef std::shared_ptr<SharedMemory::Segment<SharedStruct> > SharedStructPtr;

RUNNER_TEST(SharedMemory_010_BaseShmTest)
{
    RemoveIpcs();

    typedef std::unique_ptr<SharedMemory> SharedMemoryPtr;

    // write
    SharedMemoryPtr shm;
    Try {
        shm.Reset(SharedMemory::Create<SharedStruct>(SHM_KEY, false));
    }
    Catch(SharedMemory::Exception::NotFound) {
        shm.Reset(SharedMemory::Create<SharedStruct>(SHM_KEY, true, true));
    }

    SharedStructPtr str = shm->Attach<SharedStruct>();

    str->Data()->a = 1;
    str->Data()->b = 2;
    str->Data()->c = '3';
    str->Data()->d[0] = 4;
    str->Data()->d[1] = 5;

    // read
    SharedMemoryPtr shm2;
    Try {
        shm2.Reset(SharedMemory::Create<SharedStruct>(SHM_KEY, false));
    }
    Catch(SharedMemory::Exception::NotFound) {
        shm2.Reset(SharedMemory::Create<SharedStruct>(SHM_KEY, true, true));
    }

    SharedStructPtr str2 = shm2->Attach<SharedStruct>();
    SharedStructPtr str3 = shm2->Attach<SharedStruct>();

    RUNNER_ASSERT(str2->Data()->a == 1);
    RUNNER_ASSERT(str2->Data()->b == 2);
    RUNNER_ASSERT(str2->Data()->c == '3');
    RUNNER_ASSERT(str2->Data()->d[0] == 4);
    RUNNER_ASSERT(str2->Data()->d[1] == 5);

    RUNNER_ASSERT(str3->Data()->a == 1);
    RUNNER_ASSERT(str3->Data()->b == 2);
    RUNNER_ASSERT(str3->Data()->c == '3');
    RUNNER_ASSERT(str3->Data()->d[0] == 4);
    RUNNER_ASSERT(str3->Data()->d[1] == 5);

    str2->Data()->b = 4;
    str2->Data()->c = 'c';
    str2->Data()->d[0] = 0;
    RUNNER_ASSERT(str3->Data()->a == 1);
    RUNNER_ASSERT(str3->Data()->b == 4);
    RUNNER_ASSERT(str3->Data()->c == 'c');
    RUNNER_ASSERT(str3->Data()->d[0] == 0);
    RUNNER_ASSERT(str3->Data()->d[1] == 5);
}

//////////////////////////////////////////////

RUNNER_TEST(SharedMemory_020_SharedObjectTest)
{
    RemoveIpcs();

    typedef SharedObject<SharedTypeList> MySharedObj;

    MySharedObj::Ptr so =
        SharedObjectFactory<MySharedObj>::Create(SHM_KEY, SEM_NAME);

    RUNNER_ASSERT((so->GetProperty<0, size_t>()) == 0);
    so->SetProperty<0, size_t>(4);
    RUNNER_ASSERT((so->GetProperty<0, size_t>()) == 4);
}

//////////////////////////////////////////////

class InitTestSharedObject : public TestSharedObject
{
  protected:
    explicit InitTestSharedObject(const std::string& semaphore) :
        TestSharedObject(semaphore) {}

    virtual void Init();    // from SharedObject

  private:
    friend class SharedObjectFactory<InitTestSharedObject>;
};

void InitTestSharedObject::Init()
{
    SetPropertyInternal<0>(1);
    SetPropertyInternal<1>(2);
    SetPropertyInternal<2>('c');
}

RUNNER_TEST(SharedMemory_021_InitTest)
{
    RemoveIpcs();   // we need non existing shm

    std::shared_ptr<InitTestSharedObject> sho =
        SharedObjectFactory<InitTestSharedObject>::Create(
            SHM_KEY, SEM_NAME);
    RUNNER_ASSERT((sho->GetProperty<0, int>()) == 1);
    RUNNER_ASSERT((sho->GetProperty<1, int>()) == 2);
    RUNNER_ASSERT((sho->GetProperty<2, char>()) == 'c');
}

//////////////////////////////////////////////

class VersionTestSO1 : public TestSharedObject
{
  protected:
    explicit VersionTestSO1(const std::string& semaphore) :
        TestSharedObject(semaphore) {}

    virtual SizeType GetVersion() const
    {
        return 1;
    }                                                    // from SharedObject

  private:
    friend class SharedObjectFactory<VersionTestSO1>;
};

class VersionTestSO2 : public TestSharedObject
{
  protected:
    explicit VersionTestSO2(const std::string& semaphore) :
        TestSharedObject(semaphore) {}

    virtual SizeType GetVersion() const
    {
        return 2;
    }                                                    // from SharedObject

  private:
    friend class SharedObjectFactory<VersionTestSO2>;
};

RUNNER_TEST(SharedMemory_022_InvalidVersionTest)
{
    RemoveIpcs();   // we need non existing shm

    std::shared_ptr<VersionTestSO1> sho =
        SharedObjectFactory<VersionTestSO1>::Create(SHM_KEY, SEM_NAME);

    Try {
        std::shared_ptr<VersionTestSO2> sho2 =
            SharedObjectFactory<VersionTestSO2>::Create(SHM_KEY, SEM_NAME);

        RUNNER_ASSERT_MSG(false, "Invalid shm version has been accepted");
    }
    Catch(SharedObjectBase::Exception::InvalidVersion) {
        RUNNER_ASSERT(true);
    }
}

//////////////////////////////////////////////

RUNNER_TEST(SharedMemory_023_InvalidSizeTest)
{
    RemoveIpcs();   // we need non existing shm

    typedef SharedObject<TestTypeList> SO1;
    typedef SharedObject<TestTypeList2> SO2;

    SO1::Ptr sho = SharedObjectFactory<SO1>::Create(SHM_KEY, SEM_NAME);

    Try {
        SO2::Ptr sho2 = SharedObjectFactory<SO2>::Create(SHM_KEY, SEM_NAME);

        RUNNER_ASSERT_MSG(false, "Invalid shm size has been accepted");
    }
    Catch(SharedObjectBase::Exception::InvalidSize) {
        RUNNER_ASSERT(true);
    }
}

//////////////////////////////////////////////

class MagicTestSO1 : public TestSharedObject
{
  protected:
    explicit MagicTestSO1(const std::string& semaphore) :
        TestSharedObject(semaphore) {}

    // from SharedObject
    virtual MagicType GetMagicNumber() const
    {
        return 661;
    }

  private:
    friend class SharedObjectFactory<MagicTestSO1>;
};

class MagicTestSO2 : public TestSharedObject
{
  protected:
    explicit MagicTestSO2(const std::string& semaphore) :
        TestSharedObject(semaphore) {}

    // from SharedObject
    virtual MagicType GetMagicNumber() const
    {
        return 662;
    }

  private:
    friend class SharedObjectFactory<MagicTestSO2>;
};

RUNNER_TEST(SharedMemory_024_InvalidMagicTest)
{
    RemoveIpcs();   // we need non existing shm

    std::shared_ptr<MagicTestSO1> sho =
        SharedObjectFactory<MagicTestSO1>::Create(SHM_KEY, SEM_NAME);

    Try {
        std::shared_ptr<MagicTestSO2> sho2 =
            SharedObjectFactory<MagicTestSO2>::Create(SHM_KEY, SEM_NAME);

        RUNNER_ASSERT_MSG(false, "Invalid shm magic number has been accepted");
    }
    Catch(SharedObjectBase::Exception::InvalidMagicNumber) {
        RUNNER_ASSERT(true);
    }
}

//////////////////////////////////////////////

/*
 * Listening shared object
 */
class EnumTestSO1 : public TestSharedObject
{
  public:
    void SetWaitable(DPL::WaitableEvent* waitable)
    {
        m_waitable = waitable;
    }

  protected:
    explicit EnumTestSO1(const std::string& semaphore) :
        TestSharedObject(semaphore),
        m_waitable(NULL)
    {}


    virtual void PropertyChanged(size_t propertyEnum);

  private:
    friend class SharedObjectFactory<EnumTestSO1>;

    DPL::WaitableEvent* m_waitable;
};

void EnumTestSO1::PropertyChanged(size_t propertyEnum)
{
    if (propertyEnum == 1) {
        LogDebug("Property enum " << propertyEnum << " correctly set");
        g_enumTestCorrect = true;
    }
    if (propertyEnum == 4) {
        // This is bad. We only have 4 types
        LogError("Property enum " << propertyEnum << " should be skipped");
        g_enumTestIncorrect = true;
    }
    // confirm property change notification
    m_waitable->Signal();
}

class EnumController : public ListeningController<EnumTestSO1>
{
  public:
    explicit EnumController(DPL::WaitableEvent* waitable) :
        ListeningController<EnumTestSO1>(waitable) {}

    virtual void OnEvent(const int event);
};

void EnumController::OnEvent(const int event)
{
    if (event == INIT_EVENT) {
        m_so->SetWaitable(m_waitable);
    }
}

/*
 * Writing shared object with correct size but different number of types
 */
class EnumTestSO2 : public TestSharedObject3
{
  protected:
    explicit EnumTestSO2(const std::string& semaphore) :
        TestSharedObject3(semaphore) {}

  private:
    friend class SharedObjectFactory<EnumTestSO2>;
};

RUNNER_TEST(SharedMemory_025_InvalidEnumTest)
{
    RemoveIpcs();   // we need non existing shm

    g_enumTestCorrect = false;
    g_enumTestIncorrect = false;

    DPL::WaitableEvent waitable;

    // create listening controller and wait until it registers
    EnumController controller(&waitable);
    Wait(waitable);
    LogDebug("Listening controller created");

    // create writing shared object
    std::shared_ptr<EnumTestSO2> sho2 =
        SharedObjectFactory<EnumTestSO2>::Create(SHM_KEY, SEM_NAME);
    DPL::WaitableHandleIndexList list;

    // write property and wait for confirmation
    sho2->SetProperty<1>(2);
    Wait(waitable);

    // write incorrect property and wait for confirmation
    // we expect timeout
    sho2->SetProperty<4>(2);
    Wait(waitable, true);

    // schedule listener deregistration and wait for confirmation
    controller.PostEvent(DESTROY_EVENT);
    Wait(waitable);

    // check results
    RUNNER_ASSERT(g_enumTestCorrect == true);
    RUNNER_ASSERT(g_enumTestIncorrect == false);
}

//////////////////////////////////////////////

class MultiThreadSO : public TestSharedObject
{
  public:
    void TestAndSetProperty();

  protected:
    explicit MultiThreadSO(const std::string& semaphore) :
        TestSharedObject(semaphore) {}

  private:
    friend class SharedObjectFactory<MultiThreadSO>;
};

void MultiThreadSO::TestAndSetProperty()
{
    ScopedFlaggedLock lock(*this);

    int value = PropertyRef<0, int>();
    DPL::Thread::MicroSleep(100);
    SetPropertyInternal<0>(value + 1);
}

class ShmController : public ListeningController<MultiThreadSO>
{
  public:
    explicit ShmController(DPL::WaitableEvent* event) :
        ListeningController<MultiThreadSO>(event), m_counter(0)
    {}

    virtual void OnEventReceived(const int& event);

  private:
    size_t m_counter;
};

void ShmController::OnEventReceived(const int& event)
{
    if (event == INIT_EVENT) {
        m_so = SharedObjectFactory<MultiThreadSO>::Create(SHM_KEY, SEM_NAME);
        PostEvent(2);
    } else if (event == DESTROY_EVENT) {
        LogDebug("Destroying shared object");
        // deregister, destroy ad notify main thread
        m_so.Reset();
        m_waitable->Signal();
    } else if (event == 2) {
        m_so->TestAndSetProperty();
        m_counter++;
        if (m_counter >= TEST_AND_SET_REPEATS) {
            LogDebug("Max tests reached. Finishing thread");
            PostEvent(DESTROY_EVENT);
            return;
        }
        PostEvent(2);
    }
}

RUNNER_TEST(SharedMemory_030_MultithreadTest)
{
    RemoveIpcs();   // we need non existing shm

    typedef SharedObject<TestTypeList> SHO;
    SHO::Ptr sho = SharedObjectFactory<SHO>::Create(SHM_KEY, SEM_NAME);

    ShmController* controller[MAX_THREADS];
    DPL::WaitableEvent finalEvent[MAX_THREADS];

    for (size_t i = 0; i < MAX_THREADS; ++i) {
        controller[i] = new ShmController(&finalEvent[i]);
    }

    for (size_t i = 0; i < MAX_THREADS; ++i) {
        Wait(finalEvent[i]);
    }

    for (size_t i = 0; i < MAX_THREADS; ++i) {
        delete controller[i];
        controller[i] = NULL;
    }

    int value = sho->GetProperty<0, int>();
    LogDebug("Final value is " << value << ", expected " <<
             MAX_THREADS * TEST_AND_SET_REPEATS);
    RUNNER_ASSERT(value == MAX_THREADS * TEST_AND_SET_REPEATS);
}

//////////////////////////////////////////////

class MyModel10 : public DPL::Model
{
  public:
    explicit MyModel10(const TestSharedObject4Ptr& shared_object) :
        DPL::Model(), boolValue(this, shared_object) {}

    SharedProperty<bool, TestSharedObject4::BOOLEAN, TestSharedObject4>
    boolValue;
};

/*
 * Listening controller
 */
class ShmController3 : public ListeningController<TestSharedObject4>
{
  public:
    explicit ShmController3(DPL::WaitableEvent* event) :
        ListeningController<TestSharedObject4>(event)
    {}

    virtual void OnEvent(const int event);

    void OnValueChanged(const DPL::PropertyEvent<bool>& event);

  private:
    typedef std::unique_ptr<MyModel10> MyModelPtr;

    // model with property bound to shared object
    MyModelPtr m_model;
};

void ShmController3::OnEvent(const int event)
{
    if (event == INIT_EVENT) {
        m_model.Reset(new MyModel10(m_so));
        m_model->boolValue.AddListener(
            DPL::FastDelegate<void (const DPL::PropertyEvent<bool>&)>(
                this,
                &ShmController3::OnValueChanged));
    } else if (event == DESTROY_EVENT) {
        m_model->boolValue.RemoveListener(
            DPL::FastDelegate<void (const DPL::PropertyEvent<bool>&)>(
                this,
                &ShmController3::OnValueChanged));
        m_model.Reset();
    }
}

void ShmController3::OnValueChanged(const DPL::PropertyEvent<bool>& event)
{
    if (event.value) {
        // change back
        m_model->boolValue.Set(false);
    } else {
        LogError("Expected value = true, got false");
    }

    m_waitable->Signal();
}

RUNNER_TEST(SharedMemory_050_SharedProperty)
{
    RemoveIpcs();

    bool result = true;
    DPL::WaitableEvent waitable;
    // listener controller
    ShmController3 controller(&waitable);
    Wait(waitable);

    TestSharedObject4Ptr sharedObject = TestSharedObject4::Create();

    for (size_t i = 0; i < SHARED_PROP_REPEATS; ++i) {
        sharedObject->SetProperty<TestSharedObject4::BOOLEAN>(true);
        Wait(waitable);
        result = sharedObject->GetProperty<TestSharedObject4::BOOLEAN,
                                           bool>();
        RUNNER_ASSERT(result == false);
    }
    controller.PostEvent(DESTROY_EVENT);
    Wait(waitable);
}

//////////////////////////////////////////////

class MyModel2 : public DPL::Model
{
  public:
    explicit MyModel2(const TestSharedObjectPtr& shared_object) :
        counter(this, shared_object) {}

    SharedProperty<int, 0, TestSharedObject> counter;
};

class SPController : public ListeningController<TestSharedObject>
{
  public:
    explicit SPController(DPL::WaitableEvent* event) :
        ListeningController<TestSharedObject>(event), m_repeats(1) {}

    virtual void OnEvent(const int event);

    void OnValueChanged1(const DPL::PropertyEvent<int>& event);
    void OnValueChanged2(const DPL::PropertyEvent<int>& event);

  private:
    std::unique_ptr<MyModel2> m_model1;
    std::unique_ptr<MyModel2> m_model2;

    int m_repeats;
    std::shared_ptr<TestSharedObject> m_so2;
};

void SPController::OnEvent(const int event)
{
    if (event == INIT_EVENT) {
        m_so2 = SharedObjectFactory<TestSharedObject>::Create(SHM_KEY,
                                                              SEM_NAME);

        // create and register 2 models sharing the same property
        m_model1.Reset(new MyModel2(m_so));
        m_model2.Reset(new MyModel2(m_so2));
        m_model1->counter.AddListener(
            DPL::FastDelegate<void (const DPL::PropertyEvent<int>&)>(
                this,
                &SPController::OnValueChanged1));
        m_model2->counter.AddListener(
            DPL::FastDelegate<void (const DPL::PropertyEvent<int>&)>(
                this,
                &SPController::OnValueChanged2));
        m_model1->counter.Set(1);
    } else if (event == DESTROY_EVENT) {
        m_model1->counter.RemoveListener(
            DPL::FastDelegate<void (const DPL::PropertyEvent<int>&)>(
                this,
                &SPController::OnValueChanged1));
        m_model2->counter.RemoveListener(
            DPL::FastDelegate<void (const DPL::PropertyEvent<int>&)>(
                this,
                &SPController::OnValueChanged2));

        m_model1.Reset();
        m_model2.Reset();
        m_so2.Reset();
    }
}

void SPController::OnValueChanged1(const DPL::PropertyEvent<int>& event)
{
    if (m_repeats >= SINGLE_PROCESS_REPEATS) {
        PostEvent(DESTROY_EVENT);
        return;
    }

    LogDebug("[1] Value changed to " << event.value);
    m_repeats++;
    m_model1->counter.Set(event.value + 1);
}

void SPController::OnValueChanged2(const DPL::PropertyEvent<int>& event)
{
    if (m_repeats >= SINGLE_PROCESS_REPEATS) {
        PostEvent(DESTROY_EVENT);
        return;
    }

    LogDebug("[2] Value changed to " << event.value);
    m_repeats++;
    m_model2->counter.Set(event.value + 1);
}

RUNNER_TEST(SharedMemory_060_SingleProcess)
{
    RemoveIpcs();

    DPL::WaitableEvent waitable;
    SPController controller(&waitable);
    TestSharedObjectPtr sho = SharedObjectFactory<TestSharedObject>::Create(
            SHM_KEY,
            SEM_NAME);

    // wait for creation
    Wait(waitable);

    // wait for destruction
    Wait(waitable);

    int value = sho->GetProperty<0, int>();

    LogDebug("final value: " << value);

    // check value
    RUNNER_ASSERT(value == SINGLE_PROCESS_REPEATS);
}

//////////////////////////////////////////////

class ListenerTestController : public ListeningController<TestSharedObject>,
    public ISharedObjectListener<0, int>,
    public ISharedObjectListener<1, int>,
    public ISharedObjectListener<2, char>,
    public ISharedObjectListener<3, int[64]>
{
  public:
    explicit ListenerTestController(DPL::WaitableEvent* event) :
        ListeningController<TestSharedObject>(event) {}

    ~ListenerTestController();

    virtual void OnEvent(const int event);

    virtual void ValueChanged(size_t propertyEnum,
                              const int& value,
                              const void* info = NULL);
    virtual void ValueChanged(size_t propertyEnum,
                              const char& value,
                              const void* info = NULL);
    virtual void ValueChanged(size_t propertyEnum,
                              const int(&value)[64],
                              const void* info = NULL);
};

ListenerTestController::~ListenerTestController()
{}

void ListenerTestController::OnEvent(const int event)
{
    if (event == INIT_EVENT) {
        // add self as a listener to shared object
        m_so->AddListener<0, int>(this);
        m_so->AddListener<1, int>(this);
        m_so->AddListener<2, char>(this);
        m_so->AddListener<3, int[64]>(this);
    } else if (event == DESTROY_EVENT) {
        // remove self from listener list
        m_so->RemoveListener<0, int>(this);
        m_so->RemoveListener<1, int>(this);
        m_so->RemoveListener<2, char>(this);
        m_so->RemoveListener<3, int[64]>(this);
    }
}

void ListenerTestController::ValueChanged(size_t propertyEnum,
                                          const int& value,
                                          const void* /*info*/)
{
    LogDebug("ValueChanged(int) " << propertyEnum << " " << value);
    if ((propertyEnum == 0 &&
         value == 1) || (propertyEnum == 1 && value == 2))
    {
        g_values[propertyEnum]++;
        if (g_values[propertyEnum] == 3) {
            m_waitable->Signal();
        }
    }
}

void ListenerTestController::ValueChanged(size_t propertyEnum,
                                          const char& value,
                                          const void* /*info*/)
{
    LogDebug("ValueChanged(char) " << propertyEnum << " " << value);
    if (propertyEnum == 2 && value == 'c') {
        g_values[propertyEnum]++;
        if (g_values[propertyEnum] == 3) {
            m_waitable->Signal();
        }
    }
}

void ListenerTestController::ValueChanged(size_t propertyEnum,
                                          const int(&value)[64],
                                          const void* /*info*/)
{
    LogDebug("ValueChanged(int[64]) " << propertyEnum << " " << value[5]);
    if (propertyEnum == 3 && value[5] == 5) {
        g_values[propertyEnum]++;
        if (g_values[propertyEnum] == 3) {
            m_waitable->Signal();
        }
    }
}

RUNNER_TEST(SharedMemory_070_SharedObjectListeners)
{
    RemoveIpcs();

    // setup global flags
    for (size_t i = 0; i < TestTypeList::Size; ++i) {
        g_values[i] = 0;
    }

    // create shared object
    TestSharedObjectPtr sho = SharedObjectFactory<TestSharedObject>::Create(
            SHM_KEY, SEM_NAME);

    // create 1st listener and wait for it
    DPL::WaitableEvent waitable;
    ListenerTestController c1(&waitable);
    Wait(waitable);

    // create 2nd listener and wait for it
    ListenerTestController c2(&waitable);
    Wait(waitable);

    // create 3rd listener and wait for it
    ListenerTestController c3(&waitable);
    Wait(waitable);

    // set properties and wait for result
    sho->SetProperty<0, int>(1);
    Wait(waitable);

    RUNNER_ASSERT(g_values[0] == 3);

    sho->SetProperty<1, int>(2);
    Wait(waitable);

    RUNNER_ASSERT(g_values[1] == 3);

    sho->SetProperty<2, char>('c');
    Wait(waitable);

    RUNNER_ASSERT(g_values[2] == 3);

    int array[64];
    memset(array, 0, 64 * sizeof(array[0]));
    array[5] = 5;
    sho->SetProperty<3, int[64]>(array);
    Wait(waitable);

    RUNNER_ASSERT(g_values[3] == 3);

    // finalize listeners
    c1.PostEvent(DESTROY_EVENT);
    Wait(waitable);

    c2.PostEvent(DESTROY_EVENT);
    Wait(waitable);

    c3.PostEvent(DESTROY_EVENT);
    Wait(waitable);
}

//////////////////////////////////////////////

/*
 * class simulating DB access
 */
class DAO : public DPL::Noncopyable
{
  public:
    DAO() : m_boolValue(false) {}

    void SetBoolValue(const bool& value)
    {
        m_boolValue = value;
    }

    bool GetBoolValue() const
    {
        return m_boolValue;
    }

  private:
    bool m_boolValue;
};

/*
 * Model with property having set delegate defined
 */
class MyModel3 : public DPL::Model
{
  public:
    typedef SharedPropertyEx<bool,
                             TestSharedObject4::BOOLEAN,
                             TestSharedObject4> PropertyType;

    MyModel3(const TestSharedObject4Ptr& shared_object, DAO* dao) :
        boolValue(this,
                  shared_object,
                  PropertyType::SetDelegate(dao, &DAO::SetBoolValue))
    {}

    PropertyType boolValue;
};

RUNNER_TEST(SharedMemory_090_SetPropertyDelegate)
{
    RemoveIpcs();

    // dao object
    DAO dao;

    // create shared object
    TestSharedObject4Ptr sho = TestSharedObject4::Create();

    // set property but call dao delegate within semaphore
    sho->SetProperty<TestSharedObject4::BOOLEAN>(
        true,
        MyModel3::PropertyType::SetDelegate(&dao, &DAO::SetBoolValue));

    // check dao value
    RUNNER_ASSERT(dao.GetBoolValue() == true);

    // check shared object value
    bool shoValue = sho->GetProperty<TestSharedObject4::BOOLEAN, bool>();
    RUNNER_ASSERT(shoValue == true);

    // try the same with shared property
    MyModel3 model(sho, &dao);

    // set property
    model.boolValue.Set(false);

    // check dao value
    RUNNER_ASSERT(dao.GetBoolValue() == false);

    // check sho value
    shoValue = sho->GetProperty<TestSharedObject4::BOOLEAN, bool>();
    RUNNER_ASSERT(shoValue == false);

    // check property value
    RUNNER_ASSERT(model.boolValue.Get() == false);
}

//////////////////////////////////////////////

/*
 * Lazy initialization test shared object
 */
class LazySharedObject : public SharedObject<TestTypeList>
{
  private:
    LazySharedObject() :
        m_read(false)
    {}

  public:
    explicit LazySharedObject(const std::string& semaphore) :
        SharedObject<TestTypeList>(semaphore)
        , m_read(false)
    {}

    void Init();

    bool IsRead() const
    {
        return m_read;
    }

  private:
    friend class SharedObjectFactory<LazySharedObject>;

    bool m_read;
};

void LazySharedObject::Init()
{
    SetPropertyInternal<0>(42);
    m_read = true;
}

RUNNER_TEST(SharedMemory_100_LazyInit)
{
    RemoveIpcs();

    typedef std::shared_ptr<LazySharedObject> LazySharedObjectPtr;

    // create shared object
    LazySharedObjectPtr sho = SharedObjectFactory<LazySharedObject>::Create(
            SHM_KEY, SEM_NAME);

    RUNNER_ASSERT(sho->IsRead() == false);

    // get property causing lazy init
    int value = sho->GetProperty<0, int>();

    RUNNER_ASSERT(sho->IsRead() == true);
    RUNNER_ASSERT(value == 42);

    // create another object
    LazySharedObjectPtr sho2 = SharedObjectFactory<LazySharedObject>::Create(
            SHM_KEY, SEM_NAME);

    RUNNER_ASSERT(sho2->IsRead() == false);

    // get property NOT causing lazy init
    value = sho2->GetProperty<0, int>();

    RUNNER_ASSERT(sho2->IsRead() == false);
    RUNNER_ASSERT(value == 42);

    // destroy both objects
    sho.Reset();
    sho2.Reset();

    // create shared object
    LazySharedObjectPtr sho3 = SharedObjectFactory<LazySharedObject>::Create(
            SHM_KEY, SEM_NAME);

    RUNNER_ASSERT(sho3->IsRead() == false);

    // set property causing lazy init
    sho3->SetProperty<0>(43);
    value = sho3->GetProperty<0, int>();

    RUNNER_ASSERT(sho3->IsRead() == true);
    RUNNER_ASSERT(value == 43);
}

//////////////////////////////////////////////

bool SetCondition(const int& readValue, int& setValue);
bool SetCondition(const int& readValue, int& setValue)
{
    LogDebug("Condition delegate called with read value = " << readValue <<
             " and set value = " << setValue);

    if (readValue > 3) {
        LogDebug("Condition is false");
        return false;
    }

    LogDebug("Condition is true");
    if (4 == setValue) {
        setValue = 10;
        LogDebug("Changing set value to " << setValue);
    }
    return true;
}

void SetDelegate(const int& readValue);
void SetDelegate(const int& readValue)
{
    LogDebug("Set delegate called " << readValue);
    g_delegateCalls++;
}

RUNNER_TEST(SharedMemory_120_ConditionalSet)
{
    RemoveIpcs();

    TestSharedObjectPtr sho = SharedObjectFactory<TestSharedObject>::Create(
            SHM_KEY,
            SEM_NAME);

    g_delegateCalls = 0;

    RUNNER_ASSERT(0 == (sho->GetProperty<0, int>()));

    DPL::FastDelegate<bool (const int&, int&)> condition(&SetCondition);
    DPL::FastDelegate<void (const int&)> delegate(&SetDelegate);

    bool succeeded = false;

    succeeded = sho->ConditionalSetProperty<0>(-2, condition);

    RUNNER_ASSERT(succeeded);
    RUNNER_ASSERT(-2 == (sho->GetProperty<0, int>()));

    succeeded = sho->ConditionalSetProperty<0>(4, condition, delegate);

    RUNNER_ASSERT(succeeded);
    RUNNER_ASSERT(10 == (sho->GetProperty<0, int>()));
    RUNNER_ASSERT(1 == g_delegateCalls);

    succeeded = sho->ConditionalSetProperty<0>(5, condition);

    RUNNER_ASSERT(!succeeded);
    RUNNER_ASSERT(10 == (sho->GetProperty<0, int>()));

    succeeded = sho->ConditionalSetProperty<0>(666, condition, delegate);

    RUNNER_ASSERT(!succeeded);
    RUNNER_ASSERT(10 == (sho->GetProperty<0, int>()));
    RUNNER_ASSERT(1 == g_delegateCalls);
}

//////////////////////////////////////////////

/*
 * Shared object used by multiple threads as a singleton.
 */
class MTSharedObject : public SharedObject<TestTypeList>
{
  public:
    explicit MTSharedObject(const std::string& semaphore) :
        SharedObject<TestTypeList>(semaphore)
    {}

    void Clear();

  private:
    friend class SharedObjectFactory<MTSharedObject>;
};

typedef std::shared_ptr<MTSharedObject> MTSharedObjectPtr;

void MTSharedObject::Clear()
{
    int array[64] = {};
    SetProperty<0>(0);
    SetProperty<1>(0);
    SetProperty<2>(static_cast<char>(0));
    SetProperty<3>(array);
}

/*
 * Shared object singleton
 */
class SharedObjectSingleton
{
  public:
    static MTSharedObjectPtr Instance();
    static void Destroy();

  private:
    static MTSharedObjectPtr m_sho;
    static DPL::Mutex m_mutex;
};

MTSharedObjectPtr SharedObjectSingleton::m_sho;
DPL::Mutex SharedObjectSingleton::m_mutex;

MTSharedObjectPtr SharedObjectSingleton::Instance()
{
    DPL::Mutex::ScopedLock lock(&m_mutex);
    if (!m_sho) {
        m_sho = SharedObjectFactory<MTSharedObject>::Create(SHM_KEY, SEM_NAME);
    }
    return m_sho;
}

void SharedObjectSingleton::Destroy()
{
    DPL::Mutex::ScopedLock lock(&m_mutex);
    m_sho.Reset();
}

/*
 * Listening controller
 */
class ShmController4 : public ListeningController<MTSharedObject>,
    public ISharedObjectListener<0, int>,
    public ISharedObjectListener<1, int>,
    public ISharedObjectListener<2, char>,
    public ISharedObjectListener<3, int[64]>
{
  public:
    enum {
        ADD_LISTENERS = 2,
        REMOVE_LISTENERS = 3,
        DESTROY_SINGLETON = 4
    };

    explicit ShmController4(DPL::WaitableEvent* event) :
        ListeningController<MTSharedObject>(event),
        m_counter(0)
    {}

    virtual void OnEventReceived(const int& event);

    virtual void ValueChanged(size_t propertyEnum,
                              const int& value,
                              const void* info = NULL);
    virtual void ValueChanged(size_t propertyEnum,
                              const char& value,
                              const void* info = NULL);
    virtual void ValueChanged(size_t propertyEnum,
                              const int(&value)[64],
                              const void* info = NULL);

    bool NotRegistered();

  private:
    void Sleep();

    size_t m_counter;
    static unsigned int seed = time(NULL);
};

void ShmController4::ValueChanged(size_t propertyEnum,
                                  const int& value,
                                  const void* /*info*/)
{
    LogDebug("ValueChanged(int) " << propertyEnum << " " << value);
    if ((propertyEnum == 0 && value == 1) ||
        (propertyEnum == 1 && value == 11))
    {
        m_waitable->Signal();
    }
}

void ShmController4::ValueChanged(size_t propertyEnum,
                                  const char& value,
                                  const void* /*info*/)
{
    LogDebug("ValueChanged(char) " << propertyEnum << " " << value);
    if (propertyEnum == 2 && value == 'a') {
        m_waitable->Signal();
    }
}

void ShmController4::ValueChanged(size_t propertyEnum,
                                  const int(&value)[64],
                                  const void* /*info*/)
{
    LogDebug("ValueChanged(int[64]) " << propertyEnum << " " << value[5]);
    if (propertyEnum == 3 && value[0] == 0 && value[1] == 1 && value[2] == 2) {
        m_waitable->Signal();
    }
}

void ShmController4::Sleep()
{
    DPL::Thread::GetCurrentThread()->MiliSleep(
        rand_r(&seed) % MAX_SINGLETON_LISTENER_DELAY);
}

void ShmController4::OnEventReceived(const int& event)
{
    switch (event) {
    case INIT_EVENT:
        m_so = SharedObjectSingleton::Instance();
        m_waitable->Signal();
        break;

    case DESTROY_EVENT:
        LogDebug("Destroying shared object");
        // deregister, destroy and notify main thread
        m_so.Reset();
        m_waitable->Signal();
        break;

    case ADD_LISTENERS:
        // add listener and notify
        m_so->AddListener<0, int>(this);
        Sleep();
        m_so->AddListener<1, int>(this);
        Sleep();
        m_so->AddListener<2, char>(this);
        Sleep();
        m_so->AddListener<3, int[64]>(this);
        Sleep();
        m_waitable->Signal();
        break;

    case REMOVE_LISTENERS:
        // remove listener and notify
        m_so->RemoveListener<0, int>(this);
        Sleep();
        m_so->RemoveListener<1, int>(this);
        Sleep();
        m_so->RemoveListener<2, char>(this);
        Sleep();
        m_so->RemoveListener<3, int[64]>(this);
        Sleep();
        m_waitable->Signal();
        break;

    case DESTROY_SINGLETON:
        SharedObjectSingleton::Destroy();
        m_waitable->Signal();
        break;

    default:
        LogError("Unsupported event received: " << event);
    }
}

void MultipleWait(DPL::WaitableEvent(&event)[MAX_THREADS]);
void MultipleWait(DPL::WaitableEvent(&event)[MAX_THREADS])
{
    for (size_t i = 0; i < MAX_THREADS; ++i) {
        Wait(event[i]);
    }
}

/*
 * Try to remove property listener. If there's no such listener an exception
 * should be thrown.
 */
#define LISTENER_ASSERT(property) \
    Try { \
        singleton->RemoveListener<(property)>(controller[i]); \
        LogError("Controller " << i << " is still listening for property " \
                               << #property); \
        RUNNER_ASSERT_MSG(false, "No listeners expected"); \
    } \
    Catch(MTSharedObject::Exception::ListenerNotFound) { \
        RUNNER_ASSERT(true); \
    } \

// test
RUNNER_TEST(SharedMemory_130_SharedObjectSingleton)
{
    RemoveIpcs();   // we need non existing shm

    // writer shared object
    TestSharedObjectPtr sho = SharedObjectFactory<TestSharedObject>::Create(
            SHM_KEY, SEM_NAME);

    ShmController4* controller[MAX_THREADS];
    DPL::WaitableEvent waitable[MAX_THREADS];

    const int array[64] = { 0, 1, 2 };

    // Create and wait for notification. Make sure that the thread/controller 0
    // is created first
    LogInfo("Creating controllers");
    for (size_t i = 0; i < MAX_THREADS; ++i) {
        controller[i] = new ShmController4(&waitable[i]);
        Wait(waitable[i]);
    }

    // singleton will be created by thread/controller 0 by now
    MTSharedObjectPtr singleton = SharedObjectSingleton::Instance();

    for (size_t repeats = 0; repeats < SINGLETON_TEST_REPEATS; ++repeats) {
        LogInfo("%%%%%%%%%%%%%%%%%%%%%");
        LogInfo("Iteration " << repeats + 1 << " of " << SINGLETON_TEST_REPEATS);
        singleton->Clear();

        // add listeners
        LogInfo("Adding listeners");
        for (size_t i = 0; i < MAX_THREADS; ++i) {
            controller[i]->PostEvent(ShmController4::ADD_LISTENERS);
        }
        // wait for listeners
        MultipleWait(waitable);

        RUNNER_ASSERT((singleton->GetProperty<0, int>()) == 0);
        RUNNER_ASSERT((singleton->GetProperty<1, int>()) == 0);
        RUNNER_ASSERT((singleton->GetProperty<2, char>()) == 0);

        int checkArray[64] = {};
        singleton->GetProperty<3>(checkArray);
        RUNNER_ASSERT(checkArray[0] == 0);
        RUNNER_ASSERT(checkArray[1] == 0);
        RUNNER_ASSERT(checkArray[2] == 0);

        // change
        LogInfo("Setting property 0");
        sho->SetProperty<0>(1);
        // wait for confirmations
        MultipleWait(waitable);

        // change
        LogInfo("Setting property 1");
        sho->SetProperty<1>(11);
        // wait for confirmations
        MultipleWait(waitable);

        // change
        LogInfo("Setting property 2");
        sho->SetProperty<2>('a');
        // wait for confirmations
        MultipleWait(waitable);

        // change
        LogInfo("Setting property 3");
        sho->SetProperty<3>(array);
        // wait for confirmations
        MultipleWait(waitable);

        // remove listeners
        LogInfo("Removing listeners");
        for (size_t i = 0; i < MAX_THREADS; ++i) {
            controller[i]->PostEvent(ShmController4::REMOVE_LISTENERS);
        }
        // wait for listeners
        MultipleWait(waitable);

        // check if listeners array is empty
        LogInfo("Checking listeners");
        for (size_t i = 0; i < MAX_THREADS; ++i) {
            LISTENER_ASSERT(0);
            LISTENER_ASSERT(1);
            LISTENER_ASSERT(2);
            LISTENER_ASSERT(3);
        }

        RUNNER_ASSERT((singleton->GetProperty<0, int>()) == 1);
        RUNNER_ASSERT((singleton->GetProperty<1, int>()) == 11);
        RUNNER_ASSERT((singleton->GetProperty<2, char>()) == 'a');
        singleton->GetProperty<3>(checkArray);
        RUNNER_ASSERT(checkArray[0] == 0);
        RUNNER_ASSERT(checkArray[1] == 1);
        RUNNER_ASSERT(checkArray[2] == 2);
    }

    singleton.Reset();

    // Destroy controllers and wait for confirmation. Make sure that
    // thread/controller 0 is destroyed in the end
    LogInfo("Destroying controllers");
    for (int i = MAX_THREADS - 1; i >= 0; --i) {
        controller[i]->PostEvent(DESTROY_EVENT);
        Wait(waitable[i]);
        if (i == 0) {
            /*
             * Destroy singleton before thread that created it finishes.
             * This is to properly close all waitable handles opened by
             * SharedObject in thread 0.
             */
            LogInfo("Destroying singleton");
            controller[i]->PostEvent(ShmController4::DESTROY_SINGLETON);
            Wait(waitable[i]);
        }
        delete controller[i];
    }
}

#undef LISTENER_ASSERT

/*
 *  test preconditions & postconditions:
 *   - no existing shared memory with given SHM_KEY
 *   - no existing semaphore of given SEM_NAME
 */
RUNNER_TEST(SharedMemory_001_Preconditions) {
    RemoveIpcs();
}

RUNNER_TEST(SharedMemory_999_Postconditions) {
    RemoveIpcs();
}
