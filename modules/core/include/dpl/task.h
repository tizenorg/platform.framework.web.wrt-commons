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

#include <dpl/scoped_array.h>
#include <dpl/noncopyable.h>
#include <dpl/union_cast.h>
#include <dpl/semaphore.h>
#include <dpl/foreach.h>
#include <dpl/mutex.h>
#include <dpl/assert.h>
#include <dpl/log/log.h>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <list>
#include <stack>
#include <cctype>

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

template<typename ImplementationType>
class MultiTaskDecl
    : public Task
{
protected:
    typedef void (ImplementationType::*Step)();
    typedef std::list<Step> StepList;
    typedef std::stack<Step> StepStack;

private:
    static std::string StepToString(Step step)
    {
        std::ostringstream pseudoAddressStream;
        pseudoAddressStream << std::hex << union_cast<size_t>(step);

        std::string pseudoAddress = pseudoAddressStream.str();

        std::transform(pseudoAddress.begin(), pseudoAddress.end(),
                       pseudoAddress.begin(), ::toupper);

        return std::string("0x") + pseudoAddress;
    }

    struct ConditionalStep
    {
        Step step;

        // Depencency lists
        StepList unsatisfiedDependencies;
        StepList satisfiedDependencies;

        ConditionalStep()
            : step(NULL)
        {
        }

        ConditionalStep(Step stepArg,
                        StepList dependenciesArg)
            : step(stepArg),
              unsatisfiedDependencies(dependenciesArg)
        {
        }
    };

    typedef std::list<ConditionalStep> ConditionalStepList;

    // Synchronization
    Semaphore m_activeStepsSemaphore;
    mutable Mutex m_dependencyListMutex;

    // Those steps that have their dependency lists satified and are ready
    // to be executed
    // Current step is defined to be the front of satisfied list
    ConditionalStepList m_satisfiedSteps;

    // Those steps that are going to be executed but their dependency
    // lists have not been satified
    ConditionalStepList m_unsatisfiedSteps;

    // Those steps that have their dependency lists satified and are currently
    // being executed
    ConditionalStepList m_executingSteps;

    // Those steps that have been executed with their dependency lists
    // satisfied
    ConditionalStepList m_historicSteps;

    ///< Growing list of submitted abort steps
    StepStack m_abortSteps;

    // Deriving implementation
    ImplementationType *m_implementation;

    // Max parallel steps
    size_t m_maxParallelCount;

    ///< Valid in dependency list mutex only
    void SatisfyDependencies(const ConditionalStep &conditionalStep)
    {
        LogPedantic("Satisfying steps with dependecy to step: "
                    << StepToString(conditionalStep.step));

        // Can satisfy conditional steps if and only if there are no more
        // satisfied, unsatisfied or running same steps
        // There is at least one historic step - this one
        if (IsContainingStep(conditionalStep.step, m_unsatisfiedSteps) ||
            IsContainingStep(conditionalStep.step, m_satisfiedSteps) ||
            IsContainingStep(conditionalStep.step, m_executingSteps))
        {
            LogPedantic("Step " << StepToString(conditionalStep.step)
                        << " cannot satify other steps yet");

            return;
        }

        LogPedantic("Step " << StepToString(conditionalStep.step)
                    << " can satify other steps");

        // Do satisfy
        typename ConditionalStepList::iterator unsatisfiedStepIterator =
            m_unsatisfiedSteps.begin();;

        while (unsatisfiedStepIterator != m_unsatisfiedSteps.end())
        {
            typename StepList::iterator iterator =
                std::find(
                    unsatisfiedStepIterator->unsatisfiedDependencies.begin(),
                    unsatisfiedStepIterator->unsatisfiedDependencies.end(),
                    conditionalStep.step);

            // Does this conditional step need to be satisfied ?
            if (iterator ==
                unsatisfiedStepIterator->unsatisfiedDependencies.end())
            {
                continue;
            }

            LogPedantic("Satisfying step "
                        << StepToString(unsatisfiedStepIterator->step)
                        << " dependency to step "
                        << StepToString(conditionalStep.step));

            // Satisfy dependency
            unsatisfiedStepIterator->unsatisfiedDependencies.erase(
                iterator);

            unsatisfiedStepIterator->satisfiedDependencies.push_back(
                conditionalStep.step);

            // If step is fully satisfied, transfer it to the satisfied
            // steps list
            if (unsatisfiedStepIterator->unsatisfiedDependencies.empty())
            {
                LogPedantic("Step "
                            << StepToString(unsatisfiedStepIterator->step)
                            << " is fully satisfied");

                // Move step
                m_satisfiedSteps.push_back(*unsatisfiedStepIterator);

                unsatisfiedStepIterator =
                    m_unsatisfiedSteps.erase(unsatisfiedStepIterator);

                continue;
            }

            // Check next unsatisfied step
            ++unsatisfiedStepIterator;
        }
    }

    ///< Valid in dependency list mutex only
    bool IsContainingStep(Step step, const ConditionalStepList &dependencies) const
    {
        FOREACH (iterator, dependencies)
            if (iterator->step == step)
                return true;

        return false;
    }

    ///< Valid in dependency list mutex only
    bool IsStepDependecyListSatisfied(
        const StepList &dependencies) const
    {
        // All dependant step must be historic
        FOREACH (iterator, dependencies)
            if (!IsContainingStep(*iterator, m_historicSteps))
                return false;

        // Also, none dependant step can exist in
        // unsatisfied/satisfied/inProgress lists
        FOREACH (iterator, dependencies)
        {
            if (IsContainingStep(*iterator, m_unsatisfiedSteps) ||
                IsContainingStep(*iterator, m_satisfiedSteps) ||
                IsContainingStep(*iterator, m_executingSteps))
            {
                return false;
            }
        }

        return true;
    }

    bool AbortInternal()
    {
        // Clear all steps and construct
        // a single-dependency list of abort steps
        m_unsatisfiedSteps.clear();
        m_satisfiedSteps.clear();
        m_executingSteps.clear();
        m_historicSteps.clear();

        if (m_abortSteps.empty())
            return false;

        // Register last abort step as satisfied
        m_satisfiedSteps.push_back(
            ConditionalStep(
                m_abortSteps.top(),
                StepList()));

        Step lastStep = m_abortSteps.top();
        m_abortSteps.pop();

        // Create abort step list
        while (!m_abortSteps.empty())
        {
            // Add next unsatisfied step
            StepList dependencies;
            dependencies.push_back(lastStep);

            m_unsatisfiedSteps.push_back(
                ConditionalStep(
                    m_abortSteps.top(),
                    dependencies));

            // Remove top abort step
            lastStep = m_abortSteps.top();
            m_abortSteps.pop();
        }

        return true;
    }

protected:
    void AddStep(Step step,
                 const StepList &dependencies)
    {
        Mutex::ScopedLock lock(&m_dependencyListMutex);

        LogPedantic("Adding step: " << StepToString(step));

        FOREACH (iterator, dependencies)
            LogPedantic("  Dependency: " << StepToString(*iterator));

        // Add step to proper list
        if (IsStepDependecyListSatisfied(dependencies))
        {
            m_satisfiedSteps.push_back(ConditionalStep(step, dependencies));

            LogPedantic("Step " << StepToString(step) << " is satisfied");
        }
        else
        {
            m_unsatisfiedSteps.push_back(ConditionalStep(step, dependencies));

            LogPedantic("Step " << StepToString(step) << " is unsatisfied");
        }

        LogPedantic("Satisfied step count: " << m_satisfiedSteps.size());
        LogPedantic("Unsatisfied step count: " << m_unsatisfiedSteps.size());
    }

    void AddAbortStep(Step step)
    {
        Mutex::ScopedLock lock(&m_dependencyListMutex);

        m_abortSteps.push_front(step);

        LogPedantic("Abort step count: " << m_abortSteps.size());
    }

public:
    MultiTaskDecl(ImplementationType *implementation,
                  size_t maxParallelCount)
        : m_activeStepsSemaphore(maxParallelCount),
          m_implementation(implementation),
          m_maxParallelCount(maxParallelCount)
    {
    }

    bool NextStep()
    {
        ConditionalStep stepToExecute;
        typename ConditionalStepList::iterator executingStepIterator;

        // Take the main semaphore lock
        Semaphore::ScopedLock semaphoreLock(&m_activeStepsSemaphore);

        // Take the dependency list lock
        {
            Mutex::ScopedLock listLock(&m_dependencyListMutex);

            // Get next step to execute
            if (m_satisfiedSteps.empty())
            {
                LogPedantic("No more satisfied steps to execute");
                return false;
            }

            // Get next satisfied step to execute
            stepToExecute = m_satisfiedSteps.front();
            m_satisfiedSteps.pop_front();

            // Register it in executing step list
            m_executingSteps.push_back(stepToExecute);
            executingStepIterator = --m_executingSteps.end();

            // Leave the dependency list lock
        }

        // Execute step
        (*m_implementation.*stepToExecute.step)();

        // Take a lock again
        {
            Mutex::ScopedLock listLock(&m_dependencyListMutex);

            // Unregister executing step
            m_executingSteps.erase(executingStepIterator);

            // Add historic step
            m_historicSteps.push_back(stepToExecute);

            // Satisfy dependencies
            SatisfyDependencies(stepToExecute);

            // Leave lock
        }

        // Done
        return true;
    }

    bool Abort()
    {
        // Wait until all active steps are done
        // This is achieved by taking all semaphore slots
        ScopedArray<Semaphore::ScopedLock *> semaphoreLocks(
            new Semaphore::ScopedLock *[m_maxParallelCount]);

        for (size_t i = 0; i < m_maxParallelCount; ++i)
            semaphoreLocks[i] = new Semaphore::ScopedLock(
                &m_activeStepsSemaphore);

        // Result
        bool result;

        // Take step lists lock
        {
            Mutex::ScopedLock mutexLock(&m_dependencyListMutex);

            // Do internal abort
            result = AbortInternal();

            // Leave steps list lock
        }

        // Leave semaphore locks
        for (size_t i = 0; i < m_maxParallelCount; ++i)
            delete semaphoreLocks[i];

        // Return result
        return result;
    }

    size_t GetStepCount() const
    {
        // Return sum of sizes of all lists
        Mutex::ScopedLock lock(&m_dependencyListMutex);

        return static_cast<size_t>(
            m_unsatisfiedSteps.size() +
            m_satisfiedSteps.size() +
            m_executingSteps.size() +
            m_historicSteps.size());
    }
};
} // namespace DPL

#endif // DPL_TASK_H
