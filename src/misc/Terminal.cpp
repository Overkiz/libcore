/*
 * Terminal.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <kizbox/framework/core/Exception.h>
#include "Terminal.h"

namespace Overkiz
{

  namespace Terminal
  {

#define IS_NOT_OPEN -1

    namespace Exception
    {

      Open::Open(int errorId)
      {
        this->errorId = errorId;
      }

      const char * Open::getId() const
      {
        switch(errorId)
        {
          case 0:
            return "com.overkiz.Kizbox.Framework.Core.Terminal.Open.Error.NoDelegate";

          case EEXIST:
          case EISDIR:
          case ENAMETOOLONG:
          case ENOENT:
          case ENOTDIR:
          case ENODEV:
          case EFAULT:
          case ELOOP:
            return "com.overkiz.Kizbox.Framework.Core.Terminal.Open.Error.BadPathname";

          case EACCES:
          case ENXIO:
          case EROFS:
          case ETXTBSY:
            return "com.overkiz.Kizbox.Framework.Core.Terminal.Open.Error.AccessProblem";

          case ENOSPC:
          case ENOMEM:
          case EMFILE:
          case ENFILE:
            return "com.overkiz.Kizbox.Framework.Core.Terminal.Open.Error.MemoryProblem";

          default:
            return "com.overkiz.Kizbox.Framework.Core.Terminal.Open.Error.Unknown";
        }
      }

      const char * Close::getId() const
      {
        return "com.overkiz.Kizbox.Framework.Core.Terminal.Close.Error.BadFd";
      }

      const char * Configuration::getId() const
      {
        return "com.overkiz.Kizbox.Framework.Core.Terminal.Configuration.Error";
      }

      Read::Read(int errorId)
      {
        this->errorId = errorId;
      }

      const char * Read::getId() const
      {
        switch(errorId)
        {
          case EISDIR:
          case EBADF:
          case EINVAL:
            return "com.overkiz.Kizbox.Framework.Core.Terminal.Read.Error.BadFd";

          case EFAULT:
            return "com.overkiz.Kizbox.Framework.Core.Terminal.Read.Error.BadBufferAddress";

          default:
            return "com.overkiz.Kizbox.Framework.Core.Terminal.Read.Error.Unknown";
        }
      }

      Write::Write(int errorId)
      {
        this->errorId = errorId;
      }

      const char * Write::getId() const
      {
        switch(errorId)
        {
          case ENOSPC:
            return "com.overkiz.Kizbox.Framework.Core.Terminal.Write.Error.NoSpace";

          case EBADF:
          case EINVAL:
            return "com.overkiz.Kizbox.Framework.Core.Terminal.Write.Error.BadFd";

          case EFAULT:
            return "com.overkiz.Kizbox.Framework.Core.Terminal.Write.Error.BadBufferAddress";

          default:
            return "com.overkiz.Kizbox.Framework.Core.Terminal.Write.Error.Unknown";
        }
      }

    }

    Base::Base()
    {
      flags = 0;
      delegate = NULL;
      this->fd = IS_NOT_OPEN;
      currentStatus = 0;
      waitStatus = 0;
      memset(&config, 0, sizeof(struct termios));
    }

    Base::~Base()
    {
      this->close();
    }

    int Base::wait(int status)
    {
      if(status & Stream::OPENED)
      {
        waitStatus |= Stream::OPENED;
      }
      else
      {
        waitStatus &= ~Stream::OPENED;
      }

      return currentStatus;
    }

    void Base::open() throw(Exception::Open)
    {
      if(this->delegate == NULL)
      {
        throw Exception::Open(0);
      }

      if((this->fd = ::open(this->path.c_str(), this->flags)) < 0)
      {
        this->fd = IS_NOT_OPEN;
        throw Exception::Open(errno);
      }

      currentStatus = Stream::OPENED;
      applyConfiguration();
      start();
    }

    void Base::close() throw(Exception::Close)
    {
      stop();

      if(this->fd == IS_NOT_OPEN)
        return;

      if(::close(this->fd) == 0)
      {
        this->fd = IS_NOT_OPEN;
        currentStatus = 0;
      }
      else
      {
        throw Exception::Close();
      }
    }

    int Base::getStatus() const
    {
      return currentStatus;
    }

    int Base::getWaitStatus() const
    {
      return waitStatus;
    }

    void Base::setDelegate(Stream::Delegate * newDelegate)
    {
      this->delegate = newDelegate;
    }

    void Base::setNoCTtyFlag()
    {
      this->flags |= O_NOCTTY;
    }

    void Base::setControlMode(tcflag_t c_cflag) throw(Exception::Configuration)
    {
      config.c_cflag = c_cflag;

      if(!(fd < 0))
      {
        applyConfiguration();
      }
    }

    void Base::setLocalMode(tcflag_t c_lflag) throw(Exception::Configuration)
    {
      config.c_lflag = c_lflag;

      if(!(fd < 0))
      {
        applyConfiguration();
      }
    }

    void Base::setLine(cc_t c_line) throw(Exception::Configuration)
    {
      config.c_line = c_line;

      if(!(fd < 0))
      {
        applyConfiguration();
      }
    }

    void Base::setControlCharacters(int index, char value)
    throw(Exception::Configuration)
    {
      config.c_cc[index] = value;

      if(!(fd < 0))
      {
        applyConfiguration();
      }
    }

    void Base::applyConfiguration(int when) throw(Exception::Configuration)
    {
      if(tcsetattr(this->fd, when, &this->config) < 0)
      {
        throw Exception::Configuration();
      }
    }

    Input::Input(const std::string & path)
    {
      this->path = path;
      this->flags = O_RDONLY | O_NONBLOCK;
    }

    Input::~Input()
    {
    }

    int Input::wait(int status)
    {
      Base::wait(status);

      if(status & Stream::INPUT_READY)
      {
        if(!(currentStatus & Stream::INPUT_READY)
           && !(waitStatus & Stream::INPUT_READY))
        {
          modify(EPOLLIN | events);
          waitStatus |= Stream::INPUT_READY;
        }
      }
      else
      {
        waitStatus &= ~Stream::INPUT_READY;
        modify(~EPOLLIN & events);
      }

      return currentStatus;
    }

    void Input::process(uint32_t evts)
    {
      if((evts & EPOLLERR) || (evts & EPOLLHUP))
      {
        this->close();
        this->delegate->ready(currentStatus);
      }
      else if(evts & EPOLLIN)
      {
        currentStatus |= Stream::INPUT_READY;
        delegate->ready(currentStatus);
      }
    }

    size_t Input::read(void *data, size_t size) throw(Exception::Read)
    {
      ssize_t dataRead = 0;

      if((dataRead = ::read(this->fd, data, size)) < 0)
      {
        if(errno == EAGAIN)
        {
          dataRead = 0;
        }
        else
        {
          throw Exception::Read(errno);
        }
      }

      if(dataRead < (ssize_t)size)
      {
        currentStatus &= ~Stream::INPUT_READY;
      }

      return dataRead;
    }

    void Input::setInputMode(tcflag_t c_iflag) throw(Exception::Configuration)
    {
      this->config.c_iflag = c_iflag;

      if(!(fd < 0))
      {
        applyConfiguration(TCSAFLUSH);
      }
    }

    void Input::setInputSpeed(speed_t c_ispeed) throw(Exception::Configuration)
    {
      if(cfsetispeed(&this->config, c_ispeed) < 0)
      {
        throw Exception::Configuration();
      }

      if(!(fd < 0))
      {
        applyConfiguration(TCSAFLUSH);
      }
    }

    Output::Output(const std::string & path)
    {
      this->path = path;
      this->flags = O_WRONLY | O_NONBLOCK;
    }

    Output::~Output()
    {
    }

    int Output::wait(int status)
    {
      Base::wait(status);

      if(status & Stream::OUTPUT_READY)
      {
        if(!(currentStatus & Stream::OUTPUT_READY)
           && !(waitStatus & Stream::OUTPUT_READY))
        {
          modify(EPOLLOUT | events);
          waitStatus |= Stream::OUTPUT_READY;
        }
      }
      else
      {
        waitStatus &= ~Stream::OUTPUT_READY;
        modify(~EPOLLOUT & events);
      }

      return currentStatus;
    }

    void Output::process(uint32_t evts)
    {
      if((evts & EPOLLERR) || (evts & EPOLLHUP))
      {
        this->close();
        this->delegate->ready(currentStatus);
      }
      else if(evts & EPOLLOUT)
      {
        currentStatus |= Stream::OUTPUT_READY;
        delegate->ready(currentStatus);
        currentStatus &= ~Stream::OUTPUT_READY;
      }
    }

    size_t Output::write(const void *data, size_t size, bool more)
    throw(Exception::Write)
    {
      size_t dataWritten = 0;

      if((dataWritten = ::write(this->fd, data, size)) < 0)
      {
        if(errno == EAGAIN)
        {
          dataWritten = 0;
        }
        else
        {
          throw Exception::Write(errno);
        }
      }

      if(dataWritten < size)
      {
        currentStatus &= ~Stream::OUTPUT_READY;
      }

      return dataWritten;
    }

    void Output::setOutputMode(tcflag_t c_oflag) throw(Exception::Configuration)
    {
      this->config.c_oflag = c_oflag;

      if(!(fd < 0))
      {
        applyConfiguration(TCSADRAIN);
      }
    }

    void Output::setOutputSpeed(speed_t c_ospeed) throw(Exception::Configuration)
    {
      if(cfsetospeed(&this->config, c_ospeed) < 0)
      {
        throw Exception::Configuration();
      }

      if(!(fd < 0))
      {
        applyConfiguration(TCSADRAIN);
      }
    }

    InputOutput::InputOutput(const std::string & path) :
      Input(path), Output(path)
    {
      this->path = path;
      this->flags = O_RDWR | O_NONBLOCK;
    }

    InputOutput::~InputOutput()
    {
    }

    int InputOutput::wait(int status)
    {
      Base::wait(status);

      if((status & Stream::INPUT_READY) || (status & Stream::OUTPUT_READY))
      {
        uint32_t newEvent = events;

        if(status & Stream::INPUT_READY)
        {
          if(!(currentStatus & Stream::INPUT_READY))
          {
            newEvent |= EPOLLIN;
            waitStatus |= Stream::INPUT_READY;
          }
        }
        else
        {
          waitStatus &= ~Stream::INPUT_READY;
          newEvent &= ~EPOLLIN;
        }

        if(status & Stream::OUTPUT_READY)
        {
          if(!(currentStatus & Stream::OUTPUT_READY))
          {
            newEvent |= EPOLLOUT;
            waitStatus |= Stream::OUTPUT_READY;
          }
        }
        else
        {
          waitStatus &= ~Stream::OUTPUT_READY;
          newEvent &= ~EPOLLOUT;
        }

        modify(newEvent);
      }

      return currentStatus;
    }

    void InputOutput::process(uint32_t evts)
    {
      if((evts & EPOLLERR) || (evts & EPOLLHUP))
      {
        this->close();
        this->delegate->ready(currentStatus);
      }
      else if((evts & EPOLLIN) || (evts & EPOLLOUT))
      {
        if(evts & EPOLLIN)
        {
          currentStatus |= Stream::INPUT_READY;
        }

        if(evts & EPOLLOUT)
        {
          currentStatus |= Stream::OUTPUT_READY;
        }

        delegate->ready(currentStatus);
        currentStatus &= ~Stream::OUTPUT_READY;
      }
    }

  }

}
