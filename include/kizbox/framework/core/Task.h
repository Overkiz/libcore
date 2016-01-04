/*
 * Task.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_TASK_H_
#define OVERKIZ_TASK_H_

#include <map>

#include <kizbox/framework/core/Coroutine.h>
#include <kizbox/framework/core/Shared.h>

namespace Overkiz
{

  /**
   * A task is like a coroutine except it does not own his stack.
   * Instead it shares a stack with other tasks.
   */
  class Task
  {
  public:

    /**
     * Task Manager interface, it manage tasks resume
     */
    class IManager
    {
    public:

      virtual ~IManager()
      {
      }

      /**
       * Resume or launch a given task.
       *
       * @param task : the task to resume.
       */
      virtual void resume(Task *task) = 0;

      virtual void remove(Task *task)
      {
      }

    };

    /**
     * A simple task manager is the object which control simple task
     * Task is not interruptible
     */
    class SimpleManager: public IManager
    {
    public:

      SimpleManager();

      virtual ~SimpleManager();

      void resume(Task *task);

      void remove(Task *task);

    private:
      bool isRemoved;
      Task *current;
    };

    /**
     * A task interruptible manager is the object which owns the stack and execute interruptible tasks.
     * (tasks can be yield)
     */
    class InterruptibleManager: public IManager
    {
    public:

      InterruptibleManager();

      virtual ~InterruptibleManager();

      void resume(Task *task);

      void remove(Task *task);

    protected:

      void reset();

    private:

      class Coroutine: public Overkiz::Coroutine
      {
      public:

        Coroutine(size_t stkSize);

        virtual ~Coroutine();

        void entry();

        void save();

        void restore();

      private:

        Task *task;

        friend class Task;
      };

      std::map<Task *, Shared::Pointer<Coroutine>> coroutines;

      bool isRemoved;
      Task *currentTask;

      friend class Task;

    };

    /**
     * Task status.
     */
    typedef enum
    {
      IDLE,   //!< IDLE
      RUNNING,   //!< RUNNING
      PAUSED, //!< PAUSED
    } Status;

    /**
     * Constructor.
     *
     * @return a new task.
     */
    Task();

    /**
     * Destructor.
     *
     * @return
     */
    virtual ~Task();

    /**
     * Set watcher stack size.
     *
     * @param stkSize
     */
    void setStackSize(size_t stkSize);

    /**
     * Get watcher stack size
     *
     * @return the current stack size.
     */
    size_t getStackSize() const;

    /**
     * Get task status.
     *
     * @return the task status.
     */
    Status status();

    /**
     * Enable this task.
     * After calling this method the task can be launched by a task manager.
     */
    virtual void enable();

    /**
     * Disable this task.
     * After calling this method the task can't be launched by a task manager.
     */
    virtual void disable();

    /**
     * Test if the task is enabled.
     *
     * @return true if the task is enabled.
     */
    bool isEnabled();

  protected:

    /**
     * Entry point of the task.
     */
    virtual void entry()
    {
    }

    /**
     * Save point of the task.
     */
    virtual void save()
    {
    }

    /**
     * Restore point of the task.
     */
    virtual void restore()
    {
    }

  private:
    size_t stackSize;
    Status state;

    IManager * manager;

    bool enabled;

    friend class Manager;
  };

}

#endif /* OVERKIZ_TASK_H_ */
