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
 * @file        test_fast_delegate.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of fast delegate tests.
 */
#include <dpl/test/test_runner.h>
#include <dpl/fast_delegate.h>
#include <dpl/log/log.h>

RUNNER_TEST_GROUP_INIT(DPL)

// Sample copied and adopted from
// http://www.codeproject.com/KB/cpp/FastDelegate.aspx
//
// Demonstrate the syntax for FastDelegates.
//                -Don Clugston, May 2004.
// It's a really boring example, but it shows the most important cases.
// Declare some functions of varying complexity...
void SimpleStaticFunction(int num, const char *str);
void SimpleStaticFunction(int num, const char *str)
{
    LogDebug("In SimpleStaticFunction. Num=" << num << ", str =" << str);
}

void SimpleVoidFunction();
void SimpleVoidFunction()
{
    LogDebug("In SimpleVoidFunction with no parameters.");
}

class CBaseClass
{
  protected:
    const char *m_name;

  public:
    CBaseClass(const char *name) :
        m_name(name)
    {}

    virtual ~CBaseClass()
    {}

    void SimpleMemberFunction(int num, const char *str)
    {
        LogDebug("In SimpleMemberFunction in " << m_name << ". Num="
                                               << num << ", str = " << str);
    }

    int SimpleMemberFunctionReturnsInt(int num, const char *str)
    {
        LogDebug(
            "In SimpleMemberFunctionReturnsInt in " << m_name << ". Num="
                                                    << num <<
            ", str = " << str);
        return -1;
    }

    void ConstMemberFunction(int num, const char *str) const
    {
        LogDebug("In ConstMemberFunction in " << m_name << ". Num="
                                              << num << ", str = " << str);
    }

    virtual void SimpleVirtualFunction(int num, const char *str)
    {
        LogDebug("In SimpleVirtualFunction in " << m_name << ". Num="
                                                << num << ", str = " << str);
    }

    static void StaticMemberFunction(int num, const char *str)
    {
        LogDebug("In StaticMemberFunction Num="
                 << num << ", str = " << str);
    }
};

class COtherClass
{
    double rubbish; // to ensure this class has non-zero size.

  public:
    virtual ~COtherClass()
    {}

    virtual void UnusedVirtualFunction(void)
    {}
    virtual void TrickyVirtualFunction(int num, const char *str) = 0;
};

class VeryBigClass
{
    int letsMakeThingsComplicated[400];
};

// This declaration ensures that we get a convoluted class heirarchy.
class CDerivedClass :
    public VeryBigClass,
    virtual public COtherClass,
    virtual public CBaseClass
{
    double m_somemember[8];

  public:
    CDerivedClass() :
        CBaseClass("Base of Derived")
    {
        m_somemember[0] = 1.2345;
    }

    void SimpleDerivedFunction(int num, const char *str)
    {
        LogDebug("In SimpleDerivedFunction Num="
                 << num << ", str = " << str);
    }

    virtual void AnotherUnusedVirtualFunction(int num, const char *str)
    {
        LogDebug(
            "In AnotherUnusedVirtualFunction in " << m_name << ". Num="
                                                  << num << ", str = " <<
            str);
    }

    virtual void TrickyVirtualFunction(int num, const char *str)
    {
        LogDebug("In TrickyVirtualFunction in " << m_name << ". Num="
                                                << num << ", str = " << str);
    }
};

RUNNER_TEST(FastDelegate_Test)
{
    // Delegates with up to 8 parameters are supported.
    // Here's the case for a void function.
    // We declare a delegate and attach it to SimpleVoidFunction()
    DPL::FastDelegate0<> noparameterdelegate(&SimpleVoidFunction);

    // invoke the delegate - this calls SimpleVoidFunction()
    noparameterdelegate();

    LogDebug("-- Examples using two-parameter delegates (int, char *) --");

    // By default, the return value is void.
    typedef DPL::FastDelegate2<int, const char *> MyDelegate;

    // If you want to have a non-void return value, put it at the end.
    typedef DPL::FastDelegate2<int, const char *, int> IntMyDelegate;

    MyDelegate funclist[12]; // delegates are initialized to empty
    CBaseClass a("Base A");
    CBaseClass b("Base B");
    CDerivedClass d;
    CDerivedClass c;

    IntMyDelegate newdeleg;
    newdeleg = DPL::MakeDelegate(&a,
                                 &CBaseClass::SimpleMemberFunctionReturnsInt);

    // Binding a simple member function
    funclist[0].bind(&a, &CBaseClass::SimpleMemberFunction);

    // You can also bind static (free) functions
    funclist[1].bind(&SimpleStaticFunction);

    // and static member functions
    funclist[2].bind(&CBaseClass::StaticMemberFunction);

    // and const member functions (these only need a const class pointer).
    funclist[3].bind((const CBaseClass *) &a,
                     &CBaseClass::ConstMemberFunction);

    funclist[4].bind(&a, &CBaseClass::ConstMemberFunction);

    // and virtual member functions
    funclist[5].bind(&b, &CBaseClass::SimpleVirtualFunction);

    // You can also use the = operator. For static functions, a fastdelegate
    // looks identical to a simple function pointer.
    funclist[6] = &CBaseClass::StaticMemberFunction;

    // The weird rule about the class of derived member function pointers
    // is avoided. For MSVC, you can use &CDerivedClass::SimpleVirtualFunction
    // here, but DMC will complain. Note that as well as .bind(), you can also
    // use the MakeDelegate() global function.
    funclist[7] = DPL::MakeDelegate(&d, &CBaseClass::SimpleVirtualFunction);

    // The worst case is an abstract virtual function of a virtually-derived
    // class with at least one non-virtual base class. This is a VERY obscure
    // situation, which you're unlikely to encounter in the real world.
    // FastDelegate versions prior to 1.3 had problems with this case on VC6.
    // Now, it works without problems on all compilers.
    funclist[8].bind(&c, &CDerivedClass::TrickyVirtualFunction);

    // BUT... in such cases you should be using the base class as an
    // interface, anyway.
    funclist[9].bind(&c, &COtherClass::TrickyVirtualFunction);

    // Calling a function that was first declared in the derived class is
    // straightforward
    funclist[10] = DPL::MakeDelegate(&c, &CDerivedClass::SimpleDerivedFunction);

    // You can also bind directly using the constructor
    MyDelegate dg(&b, &CBaseClass::SimpleVirtualFunction);

    const char *msg = "Looking for equal delegate";

    for (int i = 0; i < 12; i++) {
        LogDebug(i << ":");

        // The == and != operators are provided
        // Note that they work even for inline functions.
        if (funclist[i] == dg) {
            msg = "Found equal delegate";
        }

        // operator ! can be used to test for an empty delegate
        // You can also use the .empty() member function.
        if (!funclist[i]) {
            LogDebug("Delegate is empty");
        } else {
            // Invocation generates optimal assembly code.
            funclist[i](i, msg);
        }
    }
}
