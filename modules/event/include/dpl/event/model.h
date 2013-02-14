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
 * @file    model.h
 * @author  Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version 1.0
 * @brief   Header file for model
 */
#ifndef DPL_MODEL_H
#define DPL_MODEL_H

#include <dpl/read_write_mutex.h>
#include <dpl/noncopyable.h>

namespace DPL {
namespace Event {
class Model :
    public Noncopyable
{
  protected:
    mutable DPL::ReadWriteMutex m_mutex;

    template<typename Type, typename StorageMethod>
    friend class PropertyBase;

    template<typename Type, typename AccessType, typename StorageMethod>
    friend class Property;

  public:
    virtual ~Model() = 0;
};
}
} // namespace DPL

#endif // DPL_MODEL_H
