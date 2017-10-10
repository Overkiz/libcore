/*
 * Inotify.cpp
 *
 *      Copyright (C) 2016 Overkiz SA.
 */

#include <unistd.h>
#include <algorithm>
#include <linux/limits.h>
#include <sys/stat.h>

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
    setStackSize(4*4096);
    fd = inotify_init1(IN_NONBLOCK| IN_CLOEXEC);

    if(fd < 1)
      throw Overkiz::Errno::Exception();

    modify(EPOLLIN | EPOLLERR);
  }

  InotifyManager::~InotifyManager()
  {
  }

  void InotifyManager::FileCreated::modified(const uint32_t mask)
  {
    //Check if file exists
    struct stat st;

    if(stat(inst->path.c_str(), &st) == 0)
    {
      InotifyManager::get()->add(inst);
      path.clear();
      stop();
      inst->modified(IN_CREATE);
    }
  }

  void InotifyManager::watchDirectory(InotifyInstance * inst)
  {
    //File doesn't exists yet, wait for creation
    auto path = inst->path.substr(0, inst->path.find_last_of('/'));

    if(!path.empty())
    {
      auto i = Shared::Pointer<FileCreated>::create(path, IN_CREATE, inst);
      i->start();
      directories.push_back(i);
    }
    else
    {
      throw Overkiz::Errno::Exception();
    }
  }

  void InotifyManager::add(InotifyInstance * inst)
  {
    int wd = inotify_add_watch(fd, inst->path.c_str(), inst->mask);

    if(wd < 1)
    {
      if(errno == ENOENT)
      {
        watchDirectory(inst);
      }
      else
      {
        throw Overkiz::Errno::Exception();
      }
    }
    else
    {
      if(instances.empty())
        start();

      instances[wd] = inst;
    }
  }

  void InotifyManager::remove(InotifyInstance * inst)
  {
    for(auto it = instances.begin(); it != instances.end(); ++it)
    {
      if(it->second == inst)
      {
        int wd = it->first;

        if(inotify_rm_watch(fd, wd) != 0)
          throw Overkiz::Errno::Exception();

        instances.erase(it);
        break;
      }
    }

    for(const auto & directory : directories)
    {
      if(!directory.empty() && directory->inst == inst)
      {
        directory->path.clear();
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
      static const size_t buf_max_size = 4096;
      char tmp_buf[buf_max_size];

      for(;;)
      {
        ssize_t ret = read(fd, tmp_buf, buf_max_size);

        if(ret < 1)
          break;

        const struct inotify_event * event = nullptr;

        /* Loop over all events in the buffer */

        for(char *ptr = tmp_buf; ptr < tmp_buf + ret;
            ptr += sizeof(struct inotify_event) + event->len)
        {
          event = (const struct inotify_event *) ptr;
          auto it = instances.find(event->wd);

          if(it == instances.end())
          {
            OVK_WARNING("Inotify read event, but any instance to notify.");
            continue;
          }

          //Dispatch event
          it->second->modified(event->mask);

          if((event->mask & IN_CLOSE) || (event->mask & IN_IGNORED))
          {
            struct stat fileInfo;

            if(stat(it->second->path.c_str(), &fileInfo) == 0)
            {
              //File moved; reload inotify watcher
              add(it->second);
            }
            else
            {
              //File deleted
              watchDirectory(it->second);
            }

            instances.erase(it);
          }
        }
      }

      for(auto dir = directories.begin() ; dir != directories.end();)
      {
        if(dir->empty() || (*dir)->path.empty())
        {
          dir = directories.erase(dir);
        }
        else
        {
          ++dir;
        }
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
