/*
 * Event.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <unistd.h>
#include <sys/eventfd.h>
#include <stdio.h>

#include "Poller.h"
#include "Event.h"

namespace Overkiz
{

  Event::Event() :
    count(0)
  {
    enable();
  }

  Event::~Event()
  {
    Manager::get()->remove(this);
  }

  void Event::send()
  {
    Manager::get()->send(this);
  }

  void Event::entry()
  {
    receive(count);
  }

  void Event::save()
  {
    disable();
  }

  void Event::restore()
  {
    enable();
    Manager::get()->writeEvent(this);
  }

  Shared::Pointer<Event::Manager>& Event::Manager::get()
  {
    if(manager->empty())
    {
      manager = Shared::Pointer<Manager>::create();
    }

    return *manager;
  }

  Event::Manager::Manager()
  {
    setStackSize(2 * 4096);
    fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    events = EPOLLIN;
    start();
  }

  Event::Manager::~Manager()
  {
  }

  void Event::Manager::send(Event * evt)
  {
    auto it = wevents.find(evt);

    if(it != wevents.end())
    {
      wevents[evt]++;
    }
    else
    {
      wevents[evt] = 1;
    }

    if(evt->isEnabled() && wevents.size())
    {
      writeEvent();
    }
  }

  void Event::Manager::writeEvent(Event *evt)
  {
    if(evt)
    {
      auto it = wevents.find(evt);
      bool result = (it == wevents.end());

      if(result)
        return;
    }

    if(!(events & EPOLLOUT))
    {
      uint32_t evts = events | EPOLLOUT;
      modify(evts);
    }
  }

  void Event::Manager::remove(Event *evt)
  {
    wevents.erase(evt);
  }

  void Event::Manager::process(uint32_t evts)
  {
    if(evts & EPOLLOUT)
    {
      ssize_t ret;
      uint64_t events = 1;
      ret = write(fd, &events, sizeof(events));
      uint32_t newEvents = events & ~EPOLLOUT;
      modify(newEvents);
    }

    if(evts & EPOLLIN)
    {
      uint64_t received = 0;
      ssize_t ret;
      ret = read(fd, &received, sizeof(received));

      if(received != 0)
      {
        std::map<Event*, uint64_t> tmpEvent = wevents;
        wevents.clear();

        for(auto it = tmpEvent.begin(); it != tmpEvent.end(); it++)
        {
          if(it->first->isEnabled() && it->first->status() != RUNNING)
          {
            it->first->count = it->second;
            Event * evt = it->first;
            Poller::get()->resume(evt);
          }
          else
          {
            wevents.insert(*it);
          }
        }

        if(!wevents.empty())
          writeEvent();
      }
    }
  }

  Thread::Key<Event::Manager> Event::Manager::manager;

}
