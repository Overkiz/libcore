/*
 * Poller.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_POLLER_H_
#define OVERKIZ_POLLER_H_

#include <errno.h>
#include <string.h>
#include <stdint.h>

#include <kizbox/framework/core/Task.h>
#include <kizbox/framework/core/Thread.h>
#include <kizbox/framework/core/Daemon.h>

namespace Overkiz
{

  class Watcher;

  /**
   * Poller class is an implementation of Watcher::Manager Interface based
   * on linux epoll system.
   *
   */
  class Poller : private Overkiz::Daemon
  {
  public:

    class Exception: public Overkiz::Exception
    {
    public:

      Exception() :
        _error(errno)
      {
      }

      Exception(int error) :
        _error(error)
      {
      }

      virtual ~Exception()
      {
      }

      virtual const char * getMessage() const
      {
        return strerror(_error);
      }

      virtual const char * getId() const
      {
        return "com.overkiz.Framework.Core.Poller.Exception";
      }

    protected:

      int _error;

    };

    class CreationException: public Overkiz::Poller::Exception
    {
    public:

      CreationException() :
        Overkiz::Poller::Exception(errno)
      {
      }

      CreationException(int error) :
        Overkiz::Poller::Exception(error)
      {
      }

      virtual ~CreationException()
      {
      }

      virtual const char * getId() const
      {
        return "com.overkiz.Framework.Core.Poller.CreationException";
      }

    };

    class RunningException: public Overkiz::Poller::Exception
    {
    public:

      RunningException() :
        Overkiz::Poller::Exception(0)
      {
      }

      virtual ~RunningException()
      {
      }

      virtual const char * getMessage() const
      {
        return "Already running";
      }

      virtual const char * getId() const
      {
        return "com.overkiz.Framework.Core.Poller.RunningException";
      }

    };

    class AddWatcherException: public Overkiz::Poller::Exception
    {
    public:

      AddWatcherException() :
        Overkiz::Poller::Exception(errno)
      {
      }

      AddWatcherException(int error) :
        Overkiz::Poller::Exception(error)
      {
      }

      virtual ~AddWatcherException()
      {
      }

      virtual const char * getId() const
      {
        return "com.overkiz.Framework.Core.Poller.AddWatcherException";
      }

    };

    class ModifyWatcherException: public Overkiz::Poller::Exception
    {
    public:

      ModifyWatcherException() :
        Overkiz::Poller::Exception(errno)
      {
      }

      ModifyWatcherException(int error) :
        Overkiz::Poller::Exception(error)
      {
      }

      virtual ~ModifyWatcherException()
      {
      }

      virtual const char * getId() const
      {
        return "com.overkiz.Framework.Core.Poller.ModifyWatcherException";
      }

    };

    class RemoveWatcherException: public Overkiz::Poller::Exception
    {
    public:

      RemoveWatcherException() :
        Overkiz::Poller::Exception(errno)
      {
      }

      RemoveWatcherException(int error) :
        Overkiz::Poller::Exception(error)
      {
      }

      virtual ~RemoveWatcherException()
      {
      }

      virtual const char * getId() const
      {
        return "com.overkiz.Framework.Core.Poller.RemoveWatcherException";
      }

    };

    typedef enum
    {
      STOPPED, WAITING, BUSY,
    } Status;

    /**
     *
     * @return
     */
    Status status();

    /**
     * The Loop which waits for events on the registered Watchers
     * (sockets, timer, signals, ...).
     * For each Watcher receiving an event, the schedule function
     * (implemented in Coroutine::Scheduler class) is called.
     */
    void loop(bool daemonize = false);

    /**
     * The poller handle the task manager, then you can resume a task with this method
     */
    void resume(Task *task);

    /**
     * The poller handle the task manager, then you can reset a task with this method
     */
    void reset(Task *task);

    /**
     *
     *
     * @return
     */
    static Shared::Pointer<Poller>& get(bool interruptibleTasks = true, bool usePidFile = true, bool forcedNew = false);

    /**
     * Get interruptible feature from task manager.
     */
    bool isInterruptible() const;

    void stop();

    void addListener(Daemon::Listener * list);

    void removeListener(Daemon::Listener * list);

  protected:

    /**
     * Add a new Watcher to the poller.
     *
     * @param watcher : the watcher to add.
     */
    void add(Watcher *watcher);

    /**
     * Modify events listened by the Watcher.
     *
     * @param watcher : the watcher to modify
     * @param events : the new events to listen
     */
    void modify(Watcher *watcher, uint32_t events);

    /**
     * Remove a watcher from the poller.
     *
     * @param watcher : the watcher to remove.
     */
    void remove(Watcher *watcher);

  private:

    /**
     * Constructor.
     *
     * @return
     */
    Poller(bool interruptibleTasks, bool usePidFile);

    /**
     * Destructor.
     *
     * @return
     */
    virtual ~Poller();

    Status state;
    int fd;
    int count;

    Task::IManager * taskManager;
    bool inter;
    bool abort;

    static Thread::Key<Poller> poller;

    template<typename T> friend class Shared::Pointer;
    friend class Watcher;

  };

}

#endif /* POLLER_H_ */
