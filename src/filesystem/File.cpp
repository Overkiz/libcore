/*
 * Directory.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>

#include <kizbox/framework/core/Errno.h>
#include <kizbox/framework/core/Stream.h>
#include "File.h"

#define PREFIX_EXCEPTION_ID   "com.overkiz.Kizbox.Framework.Core.File"

namespace Overkiz
{

  namespace File
  {

    Base::Base() :
      status(0), delegate(NULL), fd(-1), flags(0)
    {
    }

    Base::~Base()
    {
      if(fd >= 0)
      {
        close();
      }
    }

    void Base::open()
    {
      if(fd >= 0)
        return;

      fd = ::open(path.c_str(), flags);

      if(fd == -1)
      {
        throw Overkiz::Errno::Exception();
      }

      status |= Stream::OPENED;
    }

    void Base::close()
    {
      if(fd < 0)
        return;

      if(::close(fd) != 0)
      {
        throw Overkiz::Errno::Exception();
      }

      fd = -1;
      status = 0;
    }

    int Base::getStatus() const
    {
      return status;
    }

    int Base::getWaitStatus() const
    {
      return status;
    }

    void Base::setDelegate(Overkiz::Stream::Delegate * delegate)
    {
      this->delegate = delegate;
    }

    int Base::wait(int status)
    {
      return status;
    }

    const std::string & Base::getPath() const
    {
      return path;
    }

    void Base::setPath(const std::string & path)
    {
      this->path = path;
    }

    void Base::setFlag(bool enable, int flags)
    {
      if(enable)
      {
        this->flags |= flags;
      }
      else
      {
        this->flags &= ~flags;
      }
    }

    off_t Base::seek(long offset)
    {
      if(fd < 0)
      {
        return -1;
      }

      return lseek(fd, offset, SEEK_SET);
    }

    Input::Input()
    {
      flags = O_WRONLY;
    }

    Input::Input(const std::string & path, int fd)
    {
      this->path = path;
      this->fd = fd;
      flags = O_RDONLY;
    }

    Input::~Input()
    {
    }

    void Input::open()
    {
      Base::open();
      status |= Stream::INPUT_READY;
    }

    size_t Input::read(void * buffer, size_t count)
    {
      if(fd < 0)
        return -1;

      ssize_t r = ::read(fd, buffer, count);

      if(r == -1)
      {
        throw Overkiz::Errno::Exception();
      }

      if((size_t) r < count)
      {
        status &= ~Stream::INPUT_READY;
      }

      return r;
    }

    Output::Output()
    {
      flags = O_WRONLY;
    }

    Output::Output(const std::string & path, int fd)
    {
      this->path = path;
      this->fd = fd;
      flags = O_WRONLY;
    }

    Output::~Output()
    {
    }

    void Output::open()
    {
      Base::open();
      status |= Stream::OUTPUT_READY;
    }

    size_t Output::write(const void * buffer, size_t count, bool more)
    {
      if(fd < 0)
        return -1;

      ssize_t w = ::write(fd, buffer, count);

      if(w == -1)
      {
        throw Overkiz::Errno::Exception();
      }

      return w;
    }

  }

}
