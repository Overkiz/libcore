/*
 * Watcher.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

#include <kizbox/framework/core/Poller.h>
#include "Watcher.h"

namespace Overkiz
{

  Watcher::Watcher()
  {
    fd = -1;
    events = 0;
    current = 0;
  }

  Watcher::Watcher(const Watcher& src)
  {
    events = src.events;
    current = 0;

    if(src.fd >= 0)
    {
      fd = dup(src.fd);
    }
  }

  Watcher::Watcher(int newFd, uint32_t newEvents)
  {
    fd = newFd;
    events = newEvents;
    current = 0;
  }

  Watcher::~Watcher()
  {
    if(fd >= 0)
    {
      stop();
      close(fd);
    }

    cleanup();
    fd = -1;
    events = 0;
  }

  void Watcher::modify(uint32_t evts)
  {
    if(evts != events)
    {
      if(!manager.empty() && Task::isEnabled()
         && Task::status() != Task::Status::PAUSED)
      {
        manager->modify(this, evts);
      }

      events = evts;
    }
  }

  void Watcher::start()
  {
    enable();
  }

  void Watcher::stop()
  {
    disable();
  }

  void Watcher::entry()
  {
    process(current);
  }

  void Watcher::save()
  {
    if(!manager.empty())
    {
      manager->remove(this);
      manager = Shared::Pointer<Poller>();
    }
  }

  void Watcher::restore()
  {
    if(isEnabled())
    {
      manager = Poller::get();
      manager->add(this);
    }
  }

  void Watcher::cleanup()
  {
    manager = Shared::Pointer<Poller>();
  }

  void Watcher::enable()
  {
    if(Task::status() != Task::Status::PAUSED)
    {
      if(manager.empty())
      {
        manager = Poller::get();
      }
      else
      {
        Shared::Pointer<Poller> newManager = Poller::get();

        if(newManager != manager)
        {
          manager->remove(this);
          manager = newManager;
        }
        else
        {
          return;
        }
      }

      manager->add(this);
    }

    Task::enable();
  }

  void Watcher::disable()
  {
    Task::disable();

    if(!manager.empty())
    {
      manager->remove(this);
      manager = Shared::Pointer<Poller>();
    }
  }

}
