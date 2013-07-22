/*
 * Copyright 2013 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * @file        scope_guard.h
 * @author      Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of scope guard RAII
 */
#ifndef DPL_SCOPE_GUARD_H_
#define DPL_SCOPE_GUARD_H_

#include <cstddef>
#include <utility>
#include <new>
#include <type_traits>
#include <dpl/preprocessor.h>

namespace DPL {
template<typename FunctionType>
class ScopeGuard
{
  public:
    explicit ScopeGuard(const FunctionType& function)
        : m_function{function},
          m_released{false}
    {}

    explicit ScopeGuard(FunctionType&& function)
        : m_function{std::move(function)},
          m_released{false}
    {}

    ScopeGuard(ScopeGuard&& other)
        : m_function{std::move(other.m_function)},
          m_released{other.m_released}
    {
        other.Release();
    }

    ScopeGuard(const ScopeGuard&) = delete;

    ~ScopeGuard()
    {
        if (!m_released)
        {
            Execute();
        }
    }

    ScopeGuard& operator=(const ScopeGuard&) = delete;

    void Release()
    {
        m_released = true;
    }

    void* operator new(size_t) = delete;

  private:
    // FIXME change to noexcept when available
    void Execute() throw()
    {
        m_function();
    }

    FunctionType m_function;
    bool m_released;
};

template<typename FunctionType>
inline ScopeGuard<typename std::decay<FunctionType>::type>
MakeScopeGuard(FunctionType&& function)
{
  return ScopeGuard<typename std::decay<FunctionType>::type>(
          std::forward<FunctionType>(function));
}

namespace detail {
enum class ScopeGuardOnExit {};

template <typename FunctionType>
inline ScopeGuard<typename std::decay<FunctionType>::type>
operator+(detail::ScopeGuardOnExit, FunctionType&& function)
{
  return ScopeGuard<typename std::decay<FunctionType>::type>(
          std::forward<FunctionType>(function));
}
}
}

// FIXME provide support for compilers not supporting variadic macros;
//       instead of using variadic macros (for compatibility) we could
//       capture all by '&' (only referenced variables would be captured)
#define DPL_SCOPE_EXIT(...) \
    auto DPL_ANONYMOUS_VARIABLE(DPL_SCOPE_EXIT_STATE) \
    = ::DPL::detail::ScopeGuardOnExit() + [__VA_ARGS__]()

#endif // DPL_SCOPE_GUARD_H_
