/*
 * Signal.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <unistd.h>

#include <kizbox/framework/core/Errno.h>
#include "Signal.h"

namespace Overkiz
{

  Signal::Signal()
  {
  }

  Signal::~Signal()
  {
  }

  Signal::Manager::Manager()
  {
    setStackSize((size_t)(4 * getpagesize()));
    ::sigemptyset(&mask);

    if(sigprocmask(SIG_BLOCK, &mask, nullptr) < 0)
    {
      throw;
    }

    fd = signalfd(-1, &mask, SFD_NONBLOCK | SFD_CLOEXEC);
    Poller::get()->addListener(this);
  }

  Signal::Manager::~Manager()
  {
    Poller::get()->removeListener(this);
  }

  void Signal::Manager::willFork()
  {
    stop();
    close(fd);
    fd = -1;
  }

  void Signal::Manager::forked()
  {
    if(sigprocmask(SIG_BLOCK, &mask, nullptr) < 0)
    {
      throw;
    }

    fd = signalfd(fd, &mask, SFD_NONBLOCK | SFD_CLOEXEC);
    modify(EPOLLIN);
    start();
  }

  Signal::Manager & Signal::Manager::get()
  {
    Shared::Pointer<Manager> manager = *key;

    if(manager.empty())
    {
      manager = Shared::Pointer<Manager>::create();
      key = manager;
    }

    return *manager;
  }

  void Signal::Manager::process(uint32_t evts)
  {
    Signal signal;

    if(evts & EPOLLIN)
    {
      while(fd > -1)
      {
        ssize_t size = read(fd, &signal.info, sizeof(signal.info));

        if(size < 0)
        {
          if(errno != EAGAIN)
          {
            throw Overkiz::Errno::Exception();
          }

          break;
        }

        if(handlers.find(signal.info.ssi_signo) != handlers.end())
        {
          const auto handlerset = handlers[signal.info.ssi_signo];

          for(Handler* handler : handlerset)
          {
            handler->handle(signal);
          }
        }
      }
    }
  }

  void Signal::Manager::add(uint32_t signal, Handler *handler)
  {
    Manager & manager = get();

    if(sigisemptyset(&manager.mask) != 0)
    {
      manager.modify(EPOLLIN);
      manager.start();
    }

    if(manager.handlers.find(signal) == manager.handlers.end())
    {
      ::sigaddset(&manager.mask, signal);

      if(sigprocmask(SIG_BLOCK, &manager.mask, nullptr) < 0)
      {
        throw;
      }

      manager.fd = signalfd(manager.fd, &manager.mask, SFD_NONBLOCK | SFD_CLOEXEC);
    }

    manager.handlers[signal].insert(handler);
  }

  void Signal::Manager::remove(uint32_t signal, Handler *handler)
  {
    Manager & manager = get();
    auto & handlers = manager.handlers[signal];
    handlers.erase(handler);

    if(handlers.empty())
    {
      manager.handlers.erase(signal);
      ::sigdelset(&manager.mask, signal);

      if(sigprocmask(SIG_SETMASK, &manager.mask, nullptr) < 0)
      {
        throw;
      }

      manager.fd = signalfd(manager.fd, &manager.mask, SFD_NONBLOCK | SFD_CLOEXEC);
    }

    if(manager.handlers.empty())
    {
      manager.stop();
    }
  }

  Thread::Key<Signal::Manager> Signal::Manager::key;

}

