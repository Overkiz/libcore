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
    setStackSize(4 * getpagesize());
    ::sigemptyset(&mask);

    if(sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
    {
      throw;
    }

    fd = signalfd(-1, &mask, SFD_NONBLOCK | SFD_CLOEXEC);
  }

  Signal::Manager::~Manager()
  {
  }

  void Signal::Manager::process(uint32_t evts)
  {
    Signal signal;

    if(evts & EPOLLIN)
    {
      while(fd > 0)
      {
        ssize_t size = read(fd, &signal.info, sizeof(signal.info));

        if(size < 0)
        {
          if(errno != EAGAIN)
            throw Overkiz::Errno::Exception();

          break;
        }

        std::set<Handler *>& hdls = handlers[signal.info.ssi_signo];

        for(std::set<Handler *>::iterator i = hdls.begin();
            i != hdls.end(); ++i)
        {
          Handler *handler = *i;
          handler->handle(signal);
        }
      }
    }
  }

  void Signal::Manager::add(uint32_t signal, Handler *handler)
  {
    Shared::Pointer<Manager> manager = *key;

    if(manager.empty())
    {
      manager = Shared::Pointer<Manager>::create();
      key = manager;
    }

    if(sigisemptyset(&manager->mask) != 0)
    {
      manager->modify(EPOLLIN);
      manager->start();
    }

    std::set<Handler *>& handlers = manager->handlers[signal];
    handlers.insert(handler);
    ::sigaddset(&manager->mask, signal);

    if(sigprocmask(SIG_BLOCK, &manager->mask, NULL) < 0)
    {
      throw;
    }

    manager->fd = signalfd(manager->fd, &manager->mask,
                           SFD_NONBLOCK | SFD_CLOEXEC);
  }

  void Signal::Manager::remove(uint32_t signal, Handler *handler)
  {
    Shared::Pointer<Manager> manager = *key;

    if(manager.empty())
    {
      return;
    }

    std::set<Handler *>& handlers = manager->handlers[signal];
    handlers.erase(handler);
    ::sigdelset(&manager->mask, signal);

    if(sigprocmask(SIG_SETMASK, &manager->mask, NULL) < 0)
    {
      throw;
    }

    manager->fd = signalfd(manager->fd, &manager->mask,
                           SFD_NONBLOCK | SFD_CLOEXEC);

    if(manager->handlers->size() == 0)
    {
      key = Shared::Pointer<Manager>();
    }

    if(sigisemptyset(&manager->mask) == 0)
    {
      manager->stop();
    }
  }

  Thread::Key<Signal::Manager> Signal::Manager::key;

}

