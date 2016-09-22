/*
 * Poller.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <errno.h>

#include <config.h>
#include <kizbox/framework/core/Watcher.h>
#include <kizbox/framework/core/Log.h>
#include <kizbox/framework/core/Time.h>
#include <kizbox/framework/core/Errno.h>
#include "Poller.h"

#define MAX_EVENTS 10

#define PIDDIR "PIDDIR"
#define BUFFERSIZE 255

namespace Overkiz
{

  Poller::Poller(bool interruptibleTasks, bool usePidFile) :
    taskManager(nullptr), inter(interruptibleTasks)
  {
    count = 0;
    state = STOPPED;
    fd = epoll_create1(EPOLL_CLOEXEC);

    if(usePidFile)
      checkPidFile();

    if(fd == -1)
    {
      Overkiz::Poller::CreationException e;
      throw e;
    }

    if(interruptibleTasks)
    {
      taskManager = new Task::InterruptibleManager();
    }
    else
    {
      taskManager = new Task::SimpleManager();
    }
  }

  bool Poller::isInterruptible() const
  {
    return inter;
  }

  Poller::~Poller()
  {
    if(fd != -1)
    {
      close(fd);
    }

    delete taskManager;
  }

  void Poller::add(Watcher *watcher)
  {
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = watcher->events;
    event.data.ptr = watcher;

    if(epoll_ctl(fd, EPOLL_CTL_ADD, watcher->fd, &event) != 0)
    {
      if(errno == EEXIST)
      {
        modify(watcher, event.events);
      }
      else
      {
        Overkiz::Poller::AddWatcherException e;
        throw e;
      }
    }
    else
    {
      count++;
    }
  }

  void Poller::modify(Watcher *watcher, uint32_t events)
  {
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = events;
    event.data.ptr = watcher;

    if(epoll_ctl(fd, EPOLL_CTL_MOD, watcher->fd, &event) == -1)
    {
      if(errno == ENOENT)
      {
        watcher->events = events;
        add(watcher);
      }
      else
      {
        Overkiz::Poller::ModifyWatcherException e;
        throw e;
      }
    }
  }

  void Poller::remove(Watcher *watcher)
  {
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = 0;
    event.data.ptr = watcher;

    if(epoll_ctl(fd, EPOLL_CTL_DEL, watcher->fd, &event) != 0)
    {
      if(errno != ENOENT)
      {
        Overkiz::Poller::RemoveWatcherException e;
        throw e;
      }
    }
    else
    {
      count--;
    }
  }

  void Poller::resume(Task *task)
  {
    if(taskManager)
      taskManager->resume(task);
  }

  void Poller::reset(Task *task)
  {
    if(taskManager)
      taskManager->reset(task);
  }

  Poller::Status Poller::status()
  {
    return state;
  }

  void Poller::loop(bool daemonize)
  {
    if(state != STOPPED)
    {
      Overkiz::Poller::RunningException e;
      throw e;
    }

    struct epoll_event events[MAX_EVENTS];

    state = WAITING;

    run(daemonize);

    while(count)
    {
      int ret = 0;
      ret = epoll_wait(fd, events, MAX_EVENTS, -1);

      if(ret == 0)  //Epoll timeout
      {
        //nothing to do
        continue;
      }
      else if(ret < 0)    //Error occurs
      {
        if(errno==EINTR)
        {
          OVK_ERROR("Poller interrupted (errno=%d)",errno);
        }
        else
        {
          OVK_ERROR("Poller exiting (errno=%d)",errno);
          throw Overkiz::Errno::Exception();
        }
      }

      for(int i = 0; i < ret; i++)
      {
        state = BUSY;
        Watcher *watcher = static_cast<Watcher *>(events[i].data.ptr);
        watcher->current = events[i].events;

        try
        {
          #ifndef HAVE_RELEASE
          Time::Monotonic t1 = Time::Monotonic::now();
          #endif
          resume(watcher);
          #ifndef HAVE_RELEASE
          Time::Elapsed delta = (Time::Elapsed)(Time::Monotonic::now() - t1);

          if(delta.seconds > 2)
          {
            OVK_WARNING("--------task <%p> has spent %li seconds and %li nanoseconds !!",watcher, delta.seconds, delta.nanoseconds);
          }

          #endif
        }
        catch(const Overkiz::Exception & e)
        {
          OVK_ERROR("Task throw Overkiz exception: %s", e.getId());
          reset(watcher);

          //Check for Unrecoverable exceptions
          if(std::string(Coroutine::Exception().getId()).compare(e.getId()) == 0)
          {
            OVK_CRITICAL("Unrecoverable exception.");
            throw;
          }
        }
        catch(const std::exception & e)
        {
          OVK_ERROR("Task throw Generic exception: %s", e.what());
          reset(watcher);
        }
        catch(...)
        {
          OVK_ERROR("Task throw unknown exception");
          reset(watcher);
          #ifndef HAVE_RELEASE
          throw;
          #endif
        }

        state = WAITING;
      }

      if(!count)
        OVK_NOTICE("Any fd to watch. Exit poll loop.");
    }

    state = STOPPED;
  }

  Shared::Pointer<Poller>& Poller::get(bool interruptibleTasks, bool usePidFile)
  {
    if(poller->empty())
    {
      OVK_DEBUG("Create new poller with %s tasks%s.", interruptibleTasks ? "interruptible" : "simple", interruptibleTasks ? " !! Check your stackSize !!" : "");
      poller = Shared::Pointer<Poller>::create(interruptibleTasks, usePidFile);
    }

    return *poller;
  }

  Thread::Key<Poller> Poller::poller;

}
