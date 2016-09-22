/*
 * Task.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <unistd.h>

#include "Task.h"

namespace Overkiz
{

  Task::Task()
  {
    manager = nullptr;
    stackSize = getpagesize();
    enabled = 0;
    state = Status::IDLE;
  }

  Task::~Task()
  {
    if(manager && state == Task::Status::RUNNING)
      manager->remove(this);
  }

  void Task::setStackSize(size_t stkSize)
  {
    stackSize = stkSize;
  }

  size_t Task::getStackSize() const
  {
    return stackSize;
  }

  Task::Status Task::status()
  {
    return state;
  }

  void Task::enable()
  {
    enabled = true;
  }

  void Task::disable()
  {
    enabled = false;
  }

  bool Task::isEnabled()
  {
    return !!enabled;
  }

  Task::SimpleManager::SimpleManager() :
    isRemoved(false), current(nullptr)
  {
  }

  Task::SimpleManager::~SimpleManager()
  {
  }

  void Task::SimpleManager::resume(Task *task)
  {
    if(task && task->enabled)
    {
      task->manager = this;
      task->state = Task::Status::RUNNING;
      isRemoved = false;
      Task * prev = current;
      current = task;
      task->entry();
      current = prev;

      if(!isRemoved)
      {
        task->state = Task::Status::IDLE;
        isRemoved = false;
      }
    }
  }

  void Task::SimpleManager::reset(Task *task)
  {
    //Task may have throw an exception
    if(!isRemoved)
      task->state = Task::Status::IDLE;
  }

  void Task::SimpleManager::remove(Task *task)
  {
    //Check if this task is the current task
    if(current == task)
      isRemoved = true;
  }

  Task::InterruptibleManager::InterruptibleManager() :
    isRemoved(false), currentTask(nullptr)
  {
  }

  Task::InterruptibleManager::~InterruptibleManager()
  {
  }

  void Task::InterruptibleManager::resume(Task *task)
  {
    if(task && task->enabled)
    {
      Shared::Pointer<Coroutine> * current = &coroutines[task];

      if(current->empty() || (*current)->stackSize() < task->stackSize)
      {
        (*current) = Shared::Pointer<Coroutine>::create(task->stackSize);
      }

      (*current)->task = task;
      task->manager = this;
      isRemoved = false;
      Task * prev = currentTask;
      currentTask = task;
      Coroutine::resume((*current));
      currentTask = prev;
      task = (*current)->task;

      //Check if task was deleted
      if(isRemoved)
      {
        coroutines.erase(task);
        isRemoved = false;
        return;
      }

      if((*current)->status() != Coroutine::Status::STOPPED)
      {
        if(task)
        {
          task->state = Status::PAUSED;
        }
      }
      else
      {
        if(task)
        {
          task->state = Status::IDLE;
          (*current)->task = nullptr;
          coroutines.erase(task);
        }
      }
    }
  }

  void Task::InterruptibleManager::reset(Task *task)
  {
    //Task may have throw an exception
    if(isRemoved)
    {
      coroutines.erase(task);
      isRemoved = false;
    }
    else
    {
      // Reset coroutine
      task->state = Status::IDLE;
      coroutines[task] = Shared::Pointer<Coroutine>::create(task->stackSize);
    }
  }

  void Task::InterruptibleManager::remove(Task *task)
  {
    if(currentTask == task)
    {
      //It's not possible to remove this coroutine here, because we are using his stack
      isRemoved = true;
    }
    else
    {
      coroutines.erase(task);
    }
  }

  void Task::InterruptibleManager::reset()
  {
    coroutines.clear();
  }

  Task::InterruptibleManager::Coroutine::Coroutine(size_t stackSize) :
    Overkiz::Coroutine(stackSize)
  {
    task = nullptr;
  }

  Task::InterruptibleManager::Coroutine::~Coroutine()
  {
  }

  void Task::InterruptibleManager::Coroutine::entry()
  {
    if(task)
    {
      task->state = Task::Status::RUNNING;
      task->entry();
    }

    if(task)
    {
      task->state = Task::Status::IDLE;
    }
  }

  void Task::InterruptibleManager::Coroutine::save()
  {
    if(task)
    {
      task->state = Task::Status::PAUSED;
      task->save();
    }
  }

  void Task::InterruptibleManager::Coroutine::restore()
  {
    if(task)
    {
      task->state = Task::Status::RUNNING;
      task->restore();
    }
  }

}
