/*
 * Watcher.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_WATCHER_H_
#define OVERKIZ_WATCHER_H_

#include <stdint.h>
#include <sys/epoll.h>

#include <kizbox/framework/core/Thread.h>
#include <kizbox/framework/core/Task.h>

namespace Overkiz
{

  class Poller;

  class Watcher: public Task
  {
  public:

    class LockedException: public Overkiz::Exception
    {
    public:

      LockedException()
      {
      }

      virtual ~LockedException()
      {
      }

      virtual const char * getId() const
      {
        return "com.overkiz.Framework.Core.Watcher.LockedException";
      }

    };

    class RunningException: public Overkiz::Exception
    {
    public:

      RunningException()
      {
      }

      virtual ~RunningException()
      {
      }

      virtual const char * getId() const
      {
        return "com.overkiz.Framework.Core.Watcher.RunningException";
      }

    };

    virtual void enable();

    virtual void disable();

    void start();

    void stop();

  protected:

    Watcher();

    Watcher(const Watcher& src);

    Watcher(int fd, uint32_t events);

    virtual ~Watcher();

    void modify(uint32_t events);

    virtual void process(uint32_t events) = 0;

    virtual void save();

    virtual void restore();

    virtual void cleanup();

    uint32_t events;
    int fd;

  private:

    void entry();

    Shared::Pointer<Poller> manager;
    uint32_t current;

    friend class Poller;
    template<typename T> friend class Shared::Pointer;
  };

}

#endif /* OVERKIZ_WATCHER_H_ */
