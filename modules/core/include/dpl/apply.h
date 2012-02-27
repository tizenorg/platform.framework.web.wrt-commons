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
/**
 * @file    apply.h
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief   Implementation file for Apply functionality.
 */
#ifndef DPL_APPLY_H_
#define DPL_APPLY_H_

#ifndef __GXX_EXPERIMENTAL_CXX0X__ // C++11 compatibility check
# include <bits/c++0x_warning.h>
#else

#include <tuple>
#include <utility>

namespace DPL
{
enum class ExtraArgsInsertPolicy
{
    /**
     * Extra arguments will be add at the end of the argument list
     * passed to operation call.
     */
    Append,

    /**
     * Extra arguments will be add at the begining of the argument list
     * passed to operation call.
     */
    Prepend
};

template<ExtraArgsInsertPolicy>
struct _ApplyDispatchByPolicy;

template<typename Result = void,
         ExtraArgsInsertPolicy insertPolicy = ExtraArgsInsertPolicy::Append,
         typename Operation,
         typename ... ArgsT,
         typename ... ArgsE>
Result Apply(Operation op,
             const std::tuple<ArgsT ...>& t,
             ArgsE && ... extra)
{
    return _ApplyDispatchByPolicy<insertPolicy>::
            template Apply<Result>(op, t, std::forward<ArgsE>(extra) ...);
}

template<size_t N, size_t M>
struct _ApplyTuple
{
    template<typename Result,
             typename Operation,
             typename ... ArgsT1,
             typename ... ArgsT2,
             typename ... Args>
    static Result Apply(Operation op,
                        const std::tuple<ArgsT1 ...>& t1,
                        const std::tuple<ArgsT2 ...>& t2,
                        Args && ... args)
    {
        return _ApplyTuple<N - 1, M>::
                template Apply<Result>(op,
                                       t1,
                                       t2,
                                       std::get<N - 1>(t1),
                                       std::forward<Args>(args) ...);
    }
};

template<size_t M>
struct _ApplyTuple<0, M>
{
    template<typename Result,
             typename Operation,
             typename ... ArgsT1,
             typename ... ArgsT2,
             typename ... Args>
    static Result Apply(Operation op,
                        const std::tuple<ArgsT1 ...>& t1,
                        const std::tuple<ArgsT2 ...>& t2,
                        Args && ... args)
    {
        return _ApplyTuple<0, M - 1>::
                template Apply<Result>(op,
                                       t1,
                                       t2,
                                       std::get<M - 1>(t2),
                                       std::forward<Args>(args) ...);
    }
};

template<>
struct _ApplyTuple<0, 0>
{
    template<typename Result,
             typename Operation,
             typename ... ArgsT1,
             typename ... ArgsT2,
             typename ... Args>
    static Result Apply(Operation op,
                        const std::tuple<ArgsT1 ...>&,
                        const std::tuple<ArgsT2 ...>&,
                        Args && ... args)
    {
        return op(std::forward<Args>(args) ...);
    }
};

template<size_t N>
struct _ApplyArgs
{
    template<typename Result,
             typename Operation,
             typename ... ArgsT,
             typename ... Args>
    static Result Apply(Operation op,
            const std::tuple<ArgsT ...>& t,
            Args && ... args)
    {
        return _ApplyArgs<N - 1>::
                       template Apply<Result>(op,
                                              t,
                                              std::get<N - 1>(t),
                                              std::forward<Args>(args) ...);
    }
};

template<>
struct _ApplyArgs<0>
{
    template<typename Result,
             typename Operation,
             typename ... ArgsT,
             typename ... Args>
    static Result Apply(Operation op,
                        const std::tuple<ArgsT ...>&,
                        Args && ... args)
    {
        return op(std::forward<Args>(args) ...);
    }
};

template<>
struct _ApplyDispatchByPolicy<ExtraArgsInsertPolicy::Append>
{
    template<typename Result,
             typename Operation,
             typename ... ArgsT,
             typename ... ArgsE>
    static Result Apply(Operation op,
                        const std::tuple<ArgsT ...>& t,
                        ArgsE && ... extra)
    {
        return _ApplyArgs<sizeof ... (ArgsT)>::
                       template Apply<Result>(op,
                                              t,
                                              std::forward<ArgsE>(extra) ...);
    }
};

template<>
struct _ApplyDispatchByPolicy<ExtraArgsInsertPolicy::Prepend>
{
    template<typename Result,
             typename Operation,
             typename ... ArgsT,
             typename ... ArgsE>
    static Result Apply(Operation op,
                        const std::tuple<ArgsT ...>& t,
                        ArgsE && ... extra)
    {
        return _ApplyTuple<sizeof ... (ArgsT), sizeof ... (ArgsE)>::
                       template Apply<Result>(op,
                               t,
                               std::make_tuple(std::forward<ArgsE>(extra) ...));
    }
};
} // namespace DPL

#endif // __GXX_EXPERIMENTAL_CXX0X__

#endif // DPL_APPLY_H_
