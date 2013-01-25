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
 * @file    property.h
 * @author  Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version 1.0
 * @brief   Header file for property
 */
#ifndef DPL_PROPERTY_H
#define DPL_PROPERTY_H

#include <dpl/event/model.h>
#include <dpl/event/event_support.h>
#include <dpl/assert.h>
#include <dpl/noncopyable.h>
#include <dpl/read_write_mutex.h>
#include <dpl/fast_delegate.h>
#include <dpl/once.h>

namespace DPL
{
namespace Event
{

/**
 * Property is a class that encapsulates model's property fields.
 * Its main purpose is to automate things related to model's properties
 * such as: data storage, synchronization and event emitting.
 *
 * Property is a template of the following schema:
 *
 * template class Property<type, property access rights, property storage mode>
 *
 * Type is an internal type that property is encapsulating. It is required
 * that the type is default-constructible and copyable.
 *
 * Property access rights control which operations are allowed to be
 * executed on property.
 *
 * Property storage mode is a mode describing where and how internal data should
 * be stored.
 *
 * Property modifiers:
 *
 * PropertyStorageCached: The data is stored internally as one copy. It can
 *                        never be changed from external.
 *
 * PropertyStorageDynamic: The data is stored remotely and is accessed via
 *                         provided delegates. It can change at any time
 *                         if external mechanism changes its value.
 *
 * PropertyStorageDynamicCached: The data is stored internally, but only after
 *                               it has been retrieved by delegates. The
 *                               changed data is stored internally and also set
 *                               remotely with delegate. After the value has
 *                               been received from external source, it will
 *                               never be updated externally.
 *
 * Property access modes:
 *
 * PropertyReadOnly: Property is a read-only property.
 *                   It doesn't have Set() method.
 *
 * PropertyReadWrite: Property is a read-write property. It have both Get()
 *                    and Set() methods.
 *
 * Examples:
 *
 * Note: All properties, if not specified otherwise in template arguments,
 * have read-write access rights and cached data storage.
 *
 * Simple property with int:
 * @code DPL::Property<int> Number;
 *
 * A property with string:
 * @code DPL::Property<std::string> Name;
 *
 * A read-only float property:
 * @code DPL::Property<float, DPL::PropertyReadOnly> Gravity;
 *
 * A read-write string property:
 * @code DPL::Property<std::string, DPL::PropertyReadWrite> Caption;
 *
 * A read-write string property which is stored internally:
 * @code DPL::Property<std::string,
 *                     DPL::PropertyReadWrite,
 *                     DPL::PropertyStorageCached> Name;
 *
 * A read-write string property which is stored externally:
 * @code DPL::Property<std::string,
 *                     DPL::PropertyReadWrite,
 *                     DPL::PropertyStorageDynamic> RemoteName;
 *
 * A read-write string property which is stored externally, but also cached:
 * @code DPL::Property<std::string,
 *                     DPL::PropertyReadOnly,
 *                     DPL::PropertyStorageDynamicCached> CachedName;
 *
 * A model is an agregation of many properties. Whenever some of them change,
 * an event with this change is emmited along with model handle which
 * contains this property. These changes can be listened to. To achieve this,
 * one have to add a delegate which contains changed property value and
 * a pointer to the model.
 *
 * Example of a model with a property:
 *
 * @code
 * class MyModel: public DPL::Model
 * {
 * public:
 *     DPL::Property<int> Number1;
 *     DPL::Property<int> Number2;
 *     DPL::Property<int, DPL::PropertyReadOnly> Number3;
 *
 *     // Read write property delegate method can be static
 *     static int ReadCustomValue(Model *model);
 *
 *     // Read write property delegate method can also be method
 *     void WriteCustomValue(const int &value, Model *model);
 *
 *     MyModel()
 *      : // Initialize property with default value and this model
 *        Number1(this),
 *
 *        // Initialize property with 123 and this model
 *        Number2(this, 123),
 *
 *        // Initialize property with 0 and custom read delegate and this model
 *        Number3(this, 0, &ReadCustomValue),
 *
 *        // Initialize property with 7 and custom read-write delegates
 *        // and this model
 *        Number4(this, 7, &ReadCustomValue,
 *                DPL::MakeDelegate(this, &WriteCustomValue)
 *     {
 *     }
 * };
 *
 * DPL's delegate mechanism is a general solution, which is capable of
 * binding various types of functions and method as a delegate and
 * using them in unified way.
 *
 * Example of registering and unregistering for model's property changes:
 *
 * @code
 * class SomeModel : public DPL::Model
 * {
 * public:
 *     DPL::Property<int> Value;
 *
 *     SomeModel()
 *         : Value(this)
 *     {
 *     }
 * };
 *
 * void ValueChanged(const int &value, Model *model)
 * {
 *     std::cout << "Value changed to: " << value << std::endl;
 * }
 *
 * int main()
 * {
 *     SomeModel model;
 *
 *     // Register a global function as property changed listener
 *     model.AddListener(&ValueChanged);
 *     [...]
 *     model.RemoveListener(&ValueChanged);
 *
 *     // Register a class method as property changed listener
 *     class Receiver
 *     {
 *     public:
 *         void OnValueChanged(const int &value, Model *model)
 *         {
 *             [...]
 *         }
 *     } receiver;
 *
 *     model.AddListener(
 *         DPL::MakeDelegate(&receiver, &Receiver::OnValueChanged));
 *     [...]
 *     model.RemoveListener(
 *         DPL::MakeDelegate(&receiver, &Receiver::OnValueChanged));
 * }
 */
struct PropertyStorageCached {};        ///< Always use cached
struct PropertyStorageDynamic {};       ///< Always use dynamic
struct PropertyStorageDynamicCached {}; ///< Use dynamic then cache

struct PropertyReadOnly {};  ///< Read only, not setter available
struct PropertyReadWrite {}; ///< Read and write


template<typename Type>
struct PropertyEvent
{
    PropertyEvent(const Type &v, Model *s)
        : value(v),
          sender(s)
    {
    }

    Type value;
    Model *sender;
};

template<typename ReadDelegateType, typename WriteDelegateType>
class PropertyStorageMethodDynamicBase
{
protected:
    ReadDelegateType m_readValue;
    WriteDelegateType m_writeValue;

    PropertyStorageMethodDynamicBase(ReadDelegateType readValue,
                                     WriteDelegateType writeValue)
        : m_readValue(readValue),
          m_writeValue(writeValue)
    {
    }
};

template<typename Type>
class PropertyStorageMethodCachedBase
{
protected:
    mutable Type m_value;

    PropertyStorageMethodCachedBase()
    {
    }
};

class PropertyStorageMethodBase
{
protected:
    explicit PropertyStorageMethodBase(Model *model)
        : m_model(model)
    {
    }

    Model *m_model;
};

template<typename Type,
         typename StorageMethod,
         typename ReadDelegateType,
         typename WriteDelegateType>
class PropertyStorageMethod;

template<typename Type,
         typename ReadDelegateType,
         typename WriteDelegateType>
class PropertyStorageMethod<Type,
                            PropertyStorageCached,
                            ReadDelegateType,
                            WriteDelegateType>
    : protected PropertyStorageMethodBase,
      protected PropertyStorageMethodCachedBase<Type>
{
public:
    PropertyStorageMethod(Model *model,
                          ReadDelegateType /*readValue*/,
                          WriteDelegateType /*writeValue*/)
        : PropertyStorageMethodBase(model)
    {
    }

    Type Get() const
    {
        return this->m_value;
    }

    void Set(const Type &value)
    {
        this->m_value = value;
    }
};

template<typename Type, typename ReadDelegateType, typename WriteDelegateType>
class PropertyStorageMethod<Type,
                            PropertyStorageDynamic,
                            ReadDelegateType,
                            WriteDelegateType>
    : protected PropertyStorageMethodBase,
      protected PropertyStorageMethodDynamicBase<ReadDelegateType,
                                                 WriteDelegateType>
{
public:
    PropertyStorageMethod(Model *model,
                          ReadDelegateType readValue,
                          WriteDelegateType writeValue)
        : PropertyStorageMethodBase(model),
          PropertyStorageMethodDynamicBase<ReadDelegateType, WriteDelegateType>(
              readValue,
              writeValue)
    {
    }

    Type Get() const
    {
        Assert(!this->m_readValue.empty());
        return this->m_readValue(m_model);
    }

    void Set(const Type &value)
    {
        Assert(!this->m_writeValue.empty());
        this->m_writeValue(value, m_model);
    }
};

template<typename Type, typename ReadDelegateType, typename WriteDelegateType>
class PropertyStorageMethod<Type,
                            PropertyStorageDynamicCached,
                            ReadDelegateType,
                            WriteDelegateType>
    : protected PropertyStorageMethodBase,
      protected PropertyStorageMethodDynamicBase<ReadDelegateType,
                                                 WriteDelegateType>,
      protected PropertyStorageMethodCachedBase<Type>
{
private:
    typedef PropertyStorageMethod<Type,
                                  PropertyStorageDynamicCached,
                                  ReadDelegateType,
                                  WriteDelegateType> ThisType;

    // These two are mutable
    void OnceEnsure() const
    {
        this->m_value = this->m_readValue(m_model);
    }

    void OnceDisable() const
    {
    }

protected:
    mutable Once m_once;

public:
    PropertyStorageMethod(Model *model,
                          ReadDelegateType readValue,
                          WriteDelegateType writeValue)
        : PropertyStorageMethodBase(model),
          PropertyStorageMethodDynamicBase<ReadDelegateType, WriteDelegateType>(
              readValue, writeValue)
    {
    }

    Type Get() const
    {
        Assert(!this->m_readValue.empty());
        m_once.Call(Once::Delegate(this, &ThisType::OnceEnsure));
        return this->m_value;
    }

    void Set(const Type &value)
    {
        Assert(!this->m_writeValue.empty());

        this->m_writeValue(value, m_model);
        this->m_value = value;
        m_once.Call(Once::Delegate(this, &ThisType::OnceDisable));
    }
};

template<typename Type, typename StorageMethod>
class PropertyBase
    : protected EventSupport<PropertyEvent<Type> >
{
public:
    typedef typename EventSupport<PropertyEvent<Type> >::EventListenerType
        EventListenerType;

    typedef typename EventSupport<PropertyEvent<Type> >::DelegateType
        DelegateType;

    typedef FastDelegate<Type (Model *)>
        ReadDelegateType;

    typedef FastDelegate<void (const Type &, Model *)>
        WriteDelegateType;

protected:
    PropertyStorageMethod<Type,
                          StorageMethod,
                          ReadDelegateType,
                          WriteDelegateType> m_storage;
    Model *m_model;

    PropertyBase(Model *model,
                 ReadDelegateType readValue,
                 WriteDelegateType writeValue)
        : m_storage(model, readValue, writeValue),
          m_model(model)
    {
    }

public:
    virtual Type Get() const
    {
        ReadWriteMutex::ScopedReadLock lock(&m_model->m_mutex);
        return m_storage.Get();
    }

    void AddListener(DelegateType delegate)
    {
        EventSupport<PropertyEvent<Type> >::AddListener(delegate);
    }

    void RemoveListener(DelegateType delegate)
    {
        EventSupport<PropertyEvent<Type> >::RemoveListener(delegate);
    }
};

template<typename Type,
         typename AccessType = PropertyReadWrite,
         typename StorageMethod = PropertyStorageCached>
class Property;

template<typename Type, typename StorageMethod>
class Property<Type, PropertyReadOnly, StorageMethod>
    : public PropertyBase<Type, StorageMethod>
{
public:
    typedef typename PropertyBase<Type, StorageMethod>::EventListenerType
        EventListenerType;

    typedef typename PropertyBase<Type, StorageMethod>::DelegateType
        DelegateType;

    typedef typename PropertyBase<Type, StorageMethod>::ReadDelegateType
        ReadDelegateType;

    typedef typename PropertyBase<Type, StorageMethod>::WriteDelegateType
        WriteDelegateType;

public:
    explicit Property(Model *model,
             ReadDelegateType readValue = NULL)
        : PropertyBase<Type, StorageMethod>(model, readValue, NULL)
    {
    }

    Property(Model *model,
             const Type &value,
             ReadDelegateType readValue = NULL)
        : PropertyBase<Type, StorageMethod>(model, readValue, NULL)
    {
        this->m_storage.Set(value);
    }
};

template<typename Type, typename StorageMethod>
class Property<Type, PropertyReadWrite, StorageMethod>
    : public PropertyBase<Type, StorageMethod>
{
public:
    typedef typename PropertyBase<Type, StorageMethod>::EventListenerType
        EventListenerType;

    typedef typename PropertyBase<Type, StorageMethod>::DelegateType
        DelegateType;

    typedef typename PropertyBase<Type, StorageMethod>::ReadDelegateType
        ReadDelegateType;

    typedef typename PropertyBase<Type, StorageMethod>::WriteDelegateType
        WriteDelegateType;

public:
    explicit Property(Model *model,
             ReadDelegateType readValue = NULL,
             WriteDelegateType writeValue = NULL)
        : PropertyBase<Type, StorageMethod>(model, readValue, writeValue)
    {
    }

    Property(Model *model,
             const Type &value,
             ReadDelegateType readValue = NULL,
             WriteDelegateType writeValue = NULL)
        : PropertyBase<Type, StorageMethod>(model, readValue, writeValue)
    {
        this->m_storage.Set(value);
    }

    virtual void Set(const Type &value)
    {
        ReadWriteMutex::ScopedWriteLock lock(&this->m_model->m_mutex);

        if (this->m_storage.Get() == value)
            return;

        this->m_storage.Set(value);

        this->EmitEvent(PropertyEvent<Type>(value, this->m_model),
                  EmitMode::Auto);
    }

    void SetWithoutLock(const Type &value)
    {
        if (this->m_storage.Get() == value)
            return;

        this->m_storage.Set(value);
    }
};

}
} // namespace DPL

#endif // DPL_PROPERTY_H
