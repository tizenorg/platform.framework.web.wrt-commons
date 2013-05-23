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
 * @file    task_list.h
 * @author  Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @author  Radoslaw Wicik (r.wicik@samsung.com)
 * @version 1.0
 * @brief   Header file for task list
 */
#ifndef DPL_TASK_LIST_H
#define DPL_TASK_LIST_H

#include <dpl/task.h>
#include <list>

namespace DPL {
class TaskList :
    public Task
{
  private:
    typedef std::list<Task *> Tasks;

    Tasks m_tasks;
    Tasks::iterator m_currentTask;
    bool m_switched;

    bool m_running;

  protected:
    void AddTask(Task *task);
    void SwitchToTask(Task *task);

  public:
    TaskList();
    virtual ~TaskList();

    bool NextStep();
    bool Abort();
    size_t GetTaskCount() const;
    size_t GetStepCount() const;
};
} // namespace DPL

#endif // DPL_TASK_LIST_H