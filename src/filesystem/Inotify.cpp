/*
 * Inotify.cpp
 *
 *      Copyright (C) 2016 Overkiz SA.
 */

#include <unistd.h>
#include <algorithm>
#include <linux/limits.h>

#include <kizbox/framework/core/Errno.h>
#include <kizbox/framework/core/Log.h>
#include "Inotify.h"

namespace Overkiz
{
  InotifyInstance::InotifyInstance(const std::string & path, uint32_t event) : path(path), mask(event)
  {
  }

  InotifyInstance::~InotifyInstance()
  {
    stop();
  }

  void InotifyInstance::start()
  {
    InotifyManager::get()->add(this);
  }

  void InotifyInstance::stop()
  {
    InotifyManager::get()->remove(this);
  }

  InotifyManager::InotifyManager()
  {
    fd = inotify_init1(IN_NONBLOCK| IN_CLOEXEC);

    if(fd < 1)
      throw Overkiz::Errno::Exception();

    modify(EPOLLIN | EPOLLERR);
  }

  InotifyManager::~InotifyManager()
  {
  }

  void InotifyManager::add(InotifyInstance * inst)
  {
    int wd = inotify_add_watch(fd, inst->path.c_str(), inst->mask);

    if(wd < 1)
      throw Overkiz::Errno::Exception();

    if(instances.empty())
      start();

    instances[wd] = inst;
  }

  void InotifyManager::remove(InotifyInstance * inst)
  {
    for(auto it = instances.begin(); it != instances.end(); ++it)
    {
      if(it->second == inst)
      {
        int wd = it->first;
        int ret = inotify_rm_watch(fd, wd);
        instances.erase(it);
        break;
      }
    }

    if(instances.empty())
      stop();
  }

  void InotifyManager::process(uint32_t events)
  {
    if(events & EPOLLERR)
    {
      stop();
      OVK_ERROR("Inotify process error.");
    }

    if(events & EPOLLIN)
    {
      for(;;)
      {
        inotify_event event;
        int ret = read(fd, &event, sizeof(struct inotify_event) + NAME_MAX + 1);

        if(ret < 1)
          break;

        //Dispatch
        auto it = instances.find(event.wd);

        if(it != instances.end())
          it->second->modified(event.mask);
      }
    }
  }

  Shared::Pointer<InotifyManager>& InotifyManager::get()
  {
    if(manager->empty())
    {
      manager = Shared::Pointer<InotifyManager>::create();
    }

    return *manager;
  }

  Thread::Key<InotifyManager> InotifyManager::manager;
}
