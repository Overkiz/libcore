/*
 * Inotify.h
 *
 *      Copyright (C) 2016 Overkiz SA.
 */

#ifndef OVERKIZ_INOTIFY_H_
#define OVERKIZ_INOTIFY_H_

#include <string>
#include <sys/inotify.h>
#include <map>

#include <kizbox/framework/core/Shared.h>
#include <kizbox/framework/core/Watcher.h>

namespace Overkiz
{
  class InotifyManager;
  class InotifyInstance
  {
  public:

    /* See inotify.h for list of events */
    InotifyInstance(const std::string & path, uint32_t events = 0);

    virtual ~InotifyInstance();

    void start();

    void stop();

    virtual void modified(const uint32_t mask) = 0;

  private:

    uint32_t mask;
    std::string path;

    friend class InotifyManager;

  };

  class InotifyManager: Overkiz::Watcher
  {
  public:

    InotifyManager();

    virtual ~InotifyManager();

    static Shared::Pointer<InotifyManager>& get();

  private:

    void add(InotifyInstance * inst);

    void remove(InotifyInstance * inst);

    void process(uint32_t events);

    std::map<uint32_t, InotifyInstance *> instances;

    static Thread::Key<InotifyManager> manager;

    friend class InotifyInstance;

  };


}

#endif /* OVERKIZ_INOTIFY_H_ */
