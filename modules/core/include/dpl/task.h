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
 * @file    task.h
 * @author  Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @author  Radoslaw Wicik (r.wicik@samsung.com)
 * @version 1.0
 * @brief   Header file for abstaract task definition
 */
#ifndef DPL_TASK_H
#define DPL_TASK_H

#include <dpl/noncopyable.h>
#include <dpl/foreach.h>
#include <dpl/assert.h>
#include <algorithm>
#include <list>

namespace DPL
{
class TaskList;

class Task
    : private Noncopyable
{
public:
    virtual ~Task() {}

    virtual bool NextStep() = 0;
    virtual bool Abort() = 0;
    virtual size_t GetStepCount() const = 0;
};

template<typename Impl>
class TaskDecl
    : public Task
{
protected:
    typedef void (Impl::*Step)();

private:
    typedef std::list<Step> StepList;

    StepList m_steps;
    StepList m_abortSteps;
    typename StepList::iterator m_currentStep;
    typename StepList::iterator m_nextStep;
    bool m_switched;

    Impl *m_impl;
    bool m_running;

protected:
    void AddStep(Step step)
    {
        Assert(!m_running && "AddStep is not allowed after calling NextStep!");
        Assert(m_steps.end() == std::find(m_steps.begin(), m_steps.end(), step) && "The same step started twice is not supported");
        m_steps.push_back(step);
        m_nextStep = m_steps.begin();
    }

    void AddAbortStep(Step step)
    {
        Assert(!m_running && "AddAbortStep is not allowed after calling NextStep!");
        Assert(m_abortSteps.end() == std::find(m_abortSteps.begin(), m_abortSteps.end(), step) && "The same step started twice is not supported");
        m_abortSteps.push_front(step);
    }

    void SwitchToStep(Step step)
    {
        /// @TODO There can be problem here if user sets the same method two times in task list.
        typename StepList::iterator i = std::find(m_steps.begin(), m_steps.end(), step);
        Assert(i != m_steps.end());
        m_nextStep = i;
        m_switched = true;
    }

    Step GetCurrentStep() const
    {
        if(m_currentStep == m_steps.end())
            return NULL;

        return *m_currentStep;
    }

public:
    TaskDecl(Impl *impl)
        : m_switched(false),
          m_impl(impl),
          m_running(false)
    {
        Assert(this == m_impl);
        m_currentStep = m_steps.end();
        m_nextStep = m_steps.end();
    }

    bool NextStep()
    {
        m_running = true;

        Assert(m_nextStep != m_steps.end() && "Step list is empty or all steps done");

        m_switched = false;

        Step call = *m_nextStep;
        (*m_impl.*call)();

        m_currentStep = m_nextStep;

        if (m_switched)
            return true;
        else
            return ++m_nextStep != m_steps.end();
    }

    bool Abort()
    {
        m_running = true;

        m_steps.clear();

        if (m_abortSteps.empty())
            return false;

        FOREACH (it, m_abortSteps)
            m_steps.push_back(*it);

        m_nextStep = m_steps.begin();

        m_abortSteps.clear();

        return true;
    }

    size_t GetStepCount() const
    {
        return static_cast<size_t>(m_steps.size());
    }
};
} // namespace DPL

#endif // DPL_TASK_H
