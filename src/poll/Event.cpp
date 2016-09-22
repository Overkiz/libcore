/*
 * Event.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <unistd.h>
#include <sys/eventfd.h>
#include <stdio.h>
#include <algorithm>

#include "Poller.h"
#include "Event.h"
#include "Errno.h"

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
    disable();
  }

  void Event::send()
  {
    count++;
    Manager::get()->send(this);
  }

  void Event::entry()
  {
    uint64_t last = count;
    count = 0;
    receive(last);
  }

  void Event::save()
  {
    if(count)
      Manager::get()->remove(this);
  }

  void Event::restore()
  {
    if(count)
      Manager::get()->send(this);
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
    modify(EPOLLIN);
  }

  Event::Manager::~Manager()
  {
  }

  void Event::Manager::send(Event * evt)
  {
    start();
    auto it = std::find(wevents.begin(), wevents.end(), evt);

    if(it == wevents.end())
    {
      wevents.push_back(evt);
    }

    if(wevents.size())
    {
      writeEvent();
    }
  }

  void Event::Manager::writeEvent()
  {
    if(!(events & EPOLLOUT))
    {
      uint32_t evts = events | EPOLLOUT;
      modify(evts);
    }
  }

  void Event::Manager::remove(Event *evt)
  {
    auto it = std::find(wevents.begin(), wevents.end(), evt);

    if(it != wevents.end())
      wevents.erase(it);
  }

  void Event::Manager::process(uint32_t evts)
  {
    if(evts & EPOLLOUT)
    {
      uint64_t events = 1;

      if(write(fd, &events, sizeof(events)) < 0)
        throw Overkiz::Errno::Exception();

      uint32_t newEvents = events & ~EPOLLOUT;
      modify(newEvents);
    }

    if(evts & EPOLLIN)
    {
      uint64_t received = 0;

      if(read(fd, &received, sizeof(received)) < 0)
        throw Overkiz::Errno::Exception();

      if(received != 0)
      {
        std::vector<Event*> tmpEvent = wevents;
        wevents.clear();

        for(auto it = tmpEvent.begin(); it != tmpEvent.end(); it++)
        {
          Event* e = *it;

          //Resume only IDLE tasks
          if(e->status() != RUNNING && e->status() != PAUSED)
          {
            Poller::get()->resume(e);
          }
          else
          {
            auto it2 = std::find(wevents.begin(), wevents.end(), e);

            if(it2 == wevents.end())
            {
              wevents.push_back(e);
            }
          }
        }

        if(!wevents.empty())
        {
          writeEvent();
        }
        else
        {
          stop();
        }
      }
    }
  }

  Thread::Key<Event::Manager> Event::Manager::manager;

}
