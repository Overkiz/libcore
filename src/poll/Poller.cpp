/*
 * Poller.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <config.h>
#include <kizbox/framework/core/Watcher.h>
#include <kizbox/framework/core/Log.h>
#include <kizbox/framework/core/Errno.h>
#include "Poller.h"

#define MAX_EVENTS 10

namespace Overkiz
{

  Poller::Poller(bool interruptibleTasks) :
    taskManager(nullptr), inter(interruptibleTasks)
  {
    count = 0;
    state = STOPPED;
    fd = epoll_create(1);

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

  Poller::Status Poller::status()
  {
    return state;
  }

  void Poller::loop()
  {
    if(state != STOPPED)
    {
      Overkiz::Poller::RunningException e;
      throw e;
    }

    struct epoll_event events[MAX_EVENTS];

    int ret;

    state = WAITING;

    while(count)
    {
      ret = epoll_wait(fd, events, MAX_EVENTS, -1);

      if(ret == 0)  //Epoll timeout
      {
        //nothing to do
        continue;
      }
      else if(ret < 0)    //Error occurs
      {
        OVK_ERROR("Poller exiting");
        throw Overkiz::Errno::Exception();
      }

      for(int i = 0; i < ret; i++)
      {
        state = BUSY;
        Watcher *watcher = static_cast<Watcher *>(events[i].data.ptr);
        watcher->current = events[i].events;

        try
        {
          resume(watcher);
        }
        catch(const Overkiz::Exception & e)
        {
          OVK_ERROR("Task throw Overkiz exception: %s", e.getId());
        }
        catch(const std::exception & e)
        {
          OVK_ERROR("Task throw Generic exception: %s", e.what());
        }
        catch(...)
        {
          OVK_ERROR("Task throw unknown exception");
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

  Shared::Pointer<Poller>& Poller::get(bool interruptibleTasks)
  {
    if(poller->empty())
    {
      OVK_DEBUG("Create new poller with %s tasks%s.", interruptibleTasks ? "interruptible" : "simple", interruptibleTasks ? " !! Check your stackSize !!" : "");
      poller = Shared::Pointer<Poller>::create(interruptibleTasks);
    }

    return *poller;
  }

  Thread::Key<Poller> Poller::poller;

}
