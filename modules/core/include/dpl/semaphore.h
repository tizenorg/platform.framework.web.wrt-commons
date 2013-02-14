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
 * @file        semaphore.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of semaphore
 */
#ifndef DPL_SEMAPHORE_H
#define DPL_SEMAPHORE_H

#include <dpl/noncopyable.h>
#include <dpl/exception.h>
#include <semaphore.h>
#include <string>

namespace DPL {
class Semaphore :
    private Noncopyable
{
  public:
    class ScopedLock :
        private Noncopyable
    {
      private:
        Semaphore *m_semaphore;

      public:
        explicit ScopedLock(Semaphore *semaphore);
        ~ScopedLock();
    };

    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, CreateFailed)
        DECLARE_EXCEPTION_TYPE(Base, LockFailed)
        DECLARE_EXCEPTION_TYPE(Base, UnlockFailed)
        DECLARE_EXCEPTION_TYPE(Base, RemoveFailed)
    };

  private:
    enum Type
    {
        Type_Unnamed,
        Type_Named
    };

    Type m_type;

    mutable union
    {
        struct
        {
            sem_t handle;
        } unnamed;

        struct
        {
            sem_t *handle;
            char *name;
            bool unlinkOnDestroy;
        } named;
    } m_semaphore;

    sem_t *InternalGet() const;
    void InternalDestroy();

  public:
    /**
     * decrement the semaphore counter
     */
    void Lock() const;

    /**
     * increment the semaphore counter
     */
    void Unlock() const;

    /**
     * Remove a named semaphore
     *
     * @param fileName Name of the semaphore
     */
    static void Remove(const std::string &fileName);

    /**
     * Open an unnamed semaphore
     *
     * @param maxLockCount Maximum number of threads allowed to enter semaphore
     */
    explicit Semaphore(size_t maxLockCount);

    /**
     * Open a named semaphore
     *
     * @param fileName Semaphore filename
     * @param allowCreate Should non-existing semaphore be created
     * @param maxLockCount Maximum number of threads allowed to enter semaphore
     * @param permissions Semaphore file permissions
     * @param unlinkOnDestroy Should semaphore file be deleted on destruction
     */
    explicit Semaphore(const std::string &fileName,
                       bool allowCreate = true,
                       bool exclusiveCreate = false,
                       size_t maxLockCount = 1,
                       int permissions = 0600,
                       bool unlinkOnDestroy = false);

    /**
     * Destroy a semaphore
     */
    ~Semaphore();
};
} // namespace DPL

#endif // DPL_SEMAPHORE_H
