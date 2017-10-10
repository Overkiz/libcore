/*
 * Timer.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <cstdint>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <cerrno>
#include <climits>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>

#include <kizbox/framework/core/Errno.h>
#include "Timer.h"
#include "Poller.h"

#ifndef TFD_TIMER_CANCEL_ON_SET
  #define TFD_TIMER_CANCEL_ON_SET (1 << 1)
#endif

namespace Overkiz
{

  namespace Timer
  {

    Real::Manager::Manager()
    {
      struct itimerspec max = { { LONG_MAX, 999999999 }, { 0, 0 } };
      fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC);

      if(fd < 0)
      {
        throw Overkiz::Errno::Exception();
      }

      if(timerfd_settime(fd, TFD_TIMER_ABSTIME | TFD_TIMER_CANCEL_ON_SET, &max, nullptr) != 0)
      {
        throw Errno::Exception();
      }

      check();
      Watcher::modify(EPOLLIN);
    }

    Real::Manager::~Manager()
    {
    }

    void Real::Manager::check()
    {
      reliable = Overkiz::Time::Real::reliable();
    }

    void Real::Manager::start(Real *timer)
    {
      if(timer == nullptr)
      {
        throw;
      }

      bool added = false;

      if(timer->relative == true)
      {
        timer->offset = Time::Real::now();
      }
      else
      {
        Time::Elapsed time = { 0, 0 };
        timer->offset = time;
      }

      for(std::list<Real *>::iterator i = timers.begin(); i != timers.end();
          ++i)
      {
        Real *current = *i;

        if(current->time + current->offset > timer->time + timer->offset)
        {
          timers.insert(i, timer);
          added = true;
          break;
        }
      }

      if(!added)
      {
        timers.push_back(timer);
      }

      Watcher::start();
      reschedule();
    }

    void Real::Manager::stop(Real *timer)
    {
      for(std::list<Real *>::iterator i = timers.begin(); i != timers.end();)
      {
        Real *current = *i;

        if(current == timer)
        {
          i = timers.erase(i);
        }
        else
        {
          ++i;
        }
      }

      if(!timers.empty())
      {
        reschedule();
      }
      else
      {
        Watcher::disable();
      }
    }

    void Real::Manager::reschedule()
    {
      struct itimerspec next = { { 0, 0 }, { 0, 0 } };
      auto i = timers.begin();

      if(i == timers.end())
      {
        next.it_value.tv_sec = 0;
        next.it_value.tv_nsec = 0;
      }
      else
      {
        Real *timer = *i;
        Time::Real n = timer->time + timer->offset;
        Time::Elapsed t = n;
        next.it_value.tv_sec = t.seconds; //timer->time.getSeconds() + timer->startTime.getSeconds();
        next.it_value.tv_nsec = t.nanoseconds; //timer->time.getNanoSeconds() + timer->startTime.getNanoSeconds();
      }

      if(timerfd_settime(fd, TFD_TIMER_ABSTIME | TFD_TIMER_CANCEL_ON_SET, &next, nullptr) != 0)
      {
        if(errno == ECANCELED)
        {
          std::list<Real *> tmp = timers;
          timers.clear();

          while((i = tmp.begin()) != tmp.end())
          {
            Timer::Real *timer = *i;
            timer->manager = Shared::Pointer<Manager>();
            timer->cancelled = 1;
            Poller::get()->resume(timer);
            i = tmp.erase(i);
          }

          check();
        }
        else
        {
          throw Errno::Exception();
        }
      }
    }

    void Real::Manager::process(uint32_t evts)
    {
      if(evts & EPOLLIN)
      {
        uint64_t nb;
        std::list<Real *>::iterator i;
        ssize_t ret = read(fd, &nb, sizeof(nb));

        if(ret <= 0)
        {
          if(errno == ECANCELED)
          {
            std::list<Real *> tmp = timers;
            timers.clear();

            while((i = tmp.begin()) != tmp.end())
            {
              Timer::Real *timer = *i;
              timer->manager = Shared::Pointer<Manager>();
              timer->cancelled = 1;
              Poller::get()->resume(timer);
              i = tmp.erase(i);
            }

            check();
          }
          else if(errno == EAGAIN)
          {
            //no timer expirations have occurred at the time of the read
            return;
          }
          else
          {
            throw Overkiz::Errno::Exception();
          }
        }

        Time::Real now = Time::Real::now(); //(clock);

        while((i = timers.begin()) != timers.end())
        {
          Timer::Real *timer = *i;
          Time::Real diff = now - timer->offset;

          if(diff < timer->time)
          {
            break;
          }
          else
          {
            timer->manager = Shared::Pointer<Manager>();
            i = timers.erase(i);
            Poller::get()->resume(timer);
          }
        }

        if(!timers.empty())
        {
          reschedule();
        }
        else
        {
          Watcher::disable();
        }
      }
    }

    Thread::Key<Real::Manager> Real::Manager::manager;

    Real::Real(bool rel)
    {
      relative = rel;
      cancelled = 0;
    }

    Real::Real(const Time::Real& newTime, bool rel) :
      time(newTime)
    {
      relative = rel;
      cancelled = 0;
    }

    Real::~Real()
    {
      stop();
    }

    void Real::setTime(const Time::Real& newTime, bool rel)
    {
      bool empty = manager.empty();

      if(!empty)
      {
        stop();
      }

      time = newTime;
      relative = rel;

      if(!empty)
      {
        start();
      }
    }

    const Time::Real& Real::getTime(bool& rel) const
    {
      rel = relative;
      return time;
    }

    const Time::Real& Real::getTime() const
    {
      return time;
    }

    void Real::entry()
    {
      disable();
      Time::Real now = Time::Real::now();

      if(cancelled)
      {
        cancelled = 0;
        reschedule(now);
      }
      else
      {
        expired(now);
      }
    }

    void Real::save()
    {
      if(!manager.empty())
      {
        manager->stop(this);
        manager = Shared::Pointer<Real::Manager>();
      }
    }

    void Real::restore()
    {
      if(isEnabled())
      {
        manager = Real::Manager::get();
        manager->start(this);
      }
    }

    Shared::Pointer<Real::Manager>& Real::Manager::get()
    {
      if(manager->empty())
      {
        manager = Shared::Pointer<Manager>::create();
      }

      return *manager;
    }

    void Real::enable()
    {
      if(Task::status() != Task::Status::PAUSED)
      {
        if(manager.empty())
        {
          manager = Manager::get();
        }
        else
        {
          Shared::Pointer<Manager> newManager = Manager::get();

          if(newManager != manager)
          {
            manager->stop(this);
            manager = newManager;
          }
          else
          {
            return;
          }
        }

        manager->start(this);
      }

      Task::enable();
    }

    void Real::disable()
    {
      Task::disable();

      if(!manager.empty())
      {
        manager->stop(this);
        manager = Shared::Pointer<Manager>();
      }
    }

    void Real::start()
    {
      if(relative == true)
      {
        offset = Time::Real::now();
      }
      else
      {
        Time::Elapsed time = { 0, 0 };
        offset = time;
      }

      enable();
    }

    void Real::reschedule(const Time::Real& time)
    {
      expired(time);
    }

    void Real::stop()
    {
      disable();
    }

    void Real::pause()
    {
      disable();

      if(!relative)
      {
        offset = Time::Real::now() - offset;
      }
    }

    void Real::resume()
    {
      if(!relative)
      {
        offset = Time::Real::now() - offset;
      }
      else
      {
        Time::Elapsed time = { 0, 0 };
        offset = time;
      }

      enable();
    }

    bool Real::isRunning()
    {
      return isEnabled();
    }

    Monotonic::Manager::Manager()
    {
      fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

      if(fd < 0)
      {
        throw Overkiz::Errno::Exception();
      }

      Watcher::modify(EPOLLIN);
    }

    Monotonic::Manager::~Manager()
    {
    }

    void Monotonic::Manager::start(Monotonic *timer)
    {
      if(timer == nullptr)
      {
        throw;
      }

      bool added = false;

      for(auto i = timers.begin(); i != timers.end(); ++i)
      {
        Monotonic *current = *i;

        if(current->time + current->offset > timer->time + timer->offset)
        {
          timers.insert(i, timer);
          added = true;
          break;
        }
      }

      if(!added)
      {
        timers.push_back(timer);
      }

      Watcher::start();
      reschedule();
    }

    void Monotonic::Manager::stop(Monotonic *timer)
    {
      for(auto i = timers.begin(); i != timers.end();)
      {
        Monotonic *current = *i;

        if(current == timer)
        {
          i = timers.erase(i);
        }
        else
        {
          ++i;
        }
      }

      if(!timers.empty())
      {
        reschedule();
      }
      else
      {
        Watcher::disable();
      }
    }

    void Monotonic::Manager::reschedule()
    {
      struct itimerspec next = { { 0, 0 }, { 0, 0 } };
      auto i = timers.begin();

      if(i == timers.end())
      {
        next.it_value.tv_sec = 0;
        next.it_value.tv_nsec = 0;
      }
      else
      {
        Monotonic *timer = *i;
        Time::Monotonic n = timer->time + timer->offset;
        Time::Elapsed t = n;
        next.it_value.tv_sec = t.seconds; //timer->time.getSeconds() + timer->startTime.getSeconds();
        next.it_value.tv_nsec = t.nanoseconds; //timer->time.getNanoSeconds() + timer->startTime.getNanoSeconds();
      }

      if(timerfd_settime(fd, TFD_TIMER_ABSTIME, &next, nullptr) != 0)
      {
        throw Errno::Exception();
      }
    }

    void Monotonic::Manager::process(uint32_t evts)
    {
      if(evts & EPOLLIN)
      {
        uint64_t nb;
        int ret = read(fd, &nb, sizeof(nb));

        if(ret <= 0)
        {
          if(errno == EAGAIN)
          {
            //no timer expirations have occurred at the time of the read
            return;
          }
          else
          {
            throw Overkiz::Errno::Exception();
          }
        }

        Time::Monotonic now = Time::Monotonic::now(); //(clock);
        std::list<Monotonic *>::iterator i;

        while((i = timers.begin()) != timers.end())
        {
          Timer::Monotonic *timer = *i;
          Time::Monotonic diff = now - timer->offset;

          if(diff < timer->time)
          {
            break;
          }
          else
          {
            timer->manager = Shared::Pointer<Manager>();
            i = timers.erase(i);
            Poller::get()->resume(timer);
          }
        }

        if(!timers.empty())
        {
          reschedule();
        }
        else
        {
          Watcher::disable();
        }
      }
    }

    Shared::Pointer<Monotonic::Manager>& Monotonic::Manager::get()
    {
      if(manager->empty())
      {
        manager = Shared::Pointer<Manager>::create();
      }

      return *manager;
    }

    Thread::Key<Monotonic::Manager> Monotonic::Manager::manager;

    Monotonic::Monotonic(bool rel)
    {
      relative = rel;
    }

    Monotonic::Monotonic(const Time::Monotonic& newTime, bool rel) :
      time(newTime)
    {
      relative = rel;
    }

    Monotonic::~Monotonic()
    {
      stop();
    }

    void Monotonic::setTime(const Time::Monotonic& newTime, bool rel)
    {
      bool empty = manager.empty();

      if(!empty)
      {
        stop();
      }

      time = newTime;
      relative = rel;

      if(!empty)
      {
        start();
      }
    }

    const Time::Monotonic& Monotonic::getTime(bool& rel) const
    {
      rel = relative;
      return time;
    }

    const Time::Monotonic& Monotonic::getTime() const
    {
      return time;
    }

    void Monotonic::start()
    {
      disable();

      if(relative == true)
      {
        offset = Time::Monotonic::now();
      }
      else
      {
        Time::Elapsed time = { 0, 0 };
        offset = time;
      }

      enable();
    }

    void Monotonic::stop()
    {
      disable();
    }

    void Monotonic::pause()
    {
      disable();

      if(!relative)
      {
        offset = Time::Monotonic::now() - offset;
      }
    }

    void Monotonic::resume()
    {
      if(!relative)
      {
        offset = Time::Monotonic::now() - offset;
      }
      else
      {
        Time::Elapsed time = { 0, 0 };
        offset = time;
      }

      enable();
    }

    void Monotonic::enable()
    {
      if(Task::status() != Task::Status::PAUSED)
      {
        if(manager.empty())
        {
          manager = Manager::get();
        }
        else
        {
          Shared::Pointer<Manager> newManager = Manager::get();

          if(newManager != manager)
          {
            manager->stop(this);
            manager = newManager;
          }
          else
          {
            return;
          }
        }

        manager->start(this);
      }

      Task::enable();
    }

    void Monotonic::disable()
    {
      Task::disable();

      if(!manager.empty())
      {
        manager->stop(this);
        manager = Shared::Pointer<Manager>();
      }
    }

    bool Monotonic::isRunning()
    {
      return isEnabled();
    }

    void Monotonic::entry()
    {
      disable();
      expired(Time::Monotonic::now());
    }

    void Monotonic::save()
    {
      if(!manager.empty())
      {
        manager->stop(this);
        manager = Shared::Pointer<Manager>();
      }
    }

    void Monotonic::restore()
    {
      if(isEnabled())
      {
        manager = Monotonic::Manager::get();
        manager->start(this);
      }
    }

    Concrete::Real::Real(bool rel) :
      Timer::Real(rel), delegate(nullptr)
    {
    }

    Concrete::Real::Real(const Time::Real& time, bool relative) :
      Timer::Real(time, relative), delegate(nullptr)
    {
    }

    Concrete::Real::~Real()
    {
    }

    Concrete::Real::Delegate::~Delegate()
    {
    }

    void Concrete::Real::Delegate::reschedule(Timer::Real *timer,
        const Time::Real& time)
    {
      expired(timer, time);
    }

    void Concrete::Real::setDelegate(Concrete::Real::Delegate * delegate)
    {
      if(delegate)
        this->delegate = delegate;
    }

    void Concrete::Real::clearDelegate()
    {
      this->delegate = nullptr;
    }

    void Concrete::Real::expired(const Time::Real& time)
    {
      if(delegate)
        delegate->expired(this, time);
    }

    void Concrete::Real::reschedule(const Time::Real& time)
    {
      if(delegate)
        delegate->reschedule(this, time);
    }

    Concrete::Monotonic::Monotonic(bool rel) :
      Timer::Monotonic(rel), delegate(nullptr)
    {
    }

    Concrete::Monotonic::Monotonic(const Time::Monotonic& newTime, bool rel) :
      Timer::Monotonic(newTime, rel), delegate(nullptr)
    {
    }

    Concrete::Monotonic::~Monotonic()
    {
    }

    Concrete::Monotonic::Delegate::~Delegate()
    {
    }

    void Concrete::Monotonic::setDelegate(
      Concrete::Monotonic::Delegate * delegate)
    {
      if(delegate)
        this->delegate = delegate;
    }

    void Concrete::Monotonic::clearDelegate()
    {
      this->delegate = nullptr;
    }

    void Concrete::Monotonic::expired(const Time::Monotonic& time)
    {
      if(delegate)
        delegate->expired(this, time);
    }

  }

}
