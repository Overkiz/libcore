/*
 * Signal.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_SIGNAL_H_
#define OVERKIZ_SIGNAL_H_

#include <sys/signalfd.h>
#include <signal.h>
#include <set>

#include <kizbox/framework/core/Watcher.h>
#include <kizbox/framework/core/Poller.h>

namespace Overkiz
{
  class Poller;
  class Signal
  {
  public:

    class Manager;

    class Handler
    {
    protected:

      Handler()
      {
      }

      virtual ~Handler()
      {
      }

      virtual void handle(const Overkiz::Signal& signal) = 0;

    private:

      friend class Manager;
    };

    Signal();

    virtual ~Signal();

    struct signalfd_siginfo info;

    class Manager: public Watcher, private Daemon::Listener
    {
    public:

      static void add(uint32_t signal, Handler *handler);

      static void remove(uint32_t signal, Handler *handler);

    private:

      Manager();

      virtual ~Manager();

      void process(uint32_t events);

      static Manager & get();

      void willFork();

      void forked();

      sigset_t mask;
      std::map<uint32_t, std::set<Handler *>> handlers;

      static Thread::Key<Manager> key;

      template<typename T> friend class Shared::Pointer;

      friend class Daemon;
    };

  private:

  };

}

#endif /* OVERKIZ_SIGNAL_H_ */
