/*
 * Terminal.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <unistd.h>
#include <sys/types.h>
#include <cerrno>
#include <cstring>
#include <cstdio>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/serial.h>

#include <kizbox/framework/core/Exception.h>
#include <kizbox/framework/core/Errno.h>
#include <kizbox/framework/core/Log.h>
#include "Terminal.h"

namespace Overkiz
{

  namespace Terminal
  {

#define IS_NOT_OPEN (-1)

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

      const char * Locked::getId() const
      {
        return "com.overkiz.Kizbox.Framework.Core.Terminal.Locked";
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


      const char * Latency::getId() const
      {
        return "com.overkiz.Kizbox.Framework.Core.Terminal.Latency";
      }

    }

    Base::Base(bool lock) :
      _flags(0),_delegate(nullptr),_currentStatus(0),_waitStatus(0),_lock(lock)
    {
      this->fd = IS_NOT_OPEN;
      memset(&_config, 0, sizeof(struct termios));
    }

    Base::~Base()
    {
      this->close();
    }

    int Base::wait(int status)
    {
      if(status & Stream::OPENED)
      {
        _waitStatus |= Stream::OPENED;
      }
      else
      {
        _waitStatus &= ~Stream::OPENED;
      }

      return _currentStatus;
    }

    void Base::open() throw(Exception::Open, Exception::Configuration, Exception::Locked)
    {
      if(_delegate == nullptr)
      {
        throw Exception::Open(0);
      }

      if((this->fd = ::open(_path.c_str(), _flags)) < 0)
      {
        this->fd = IS_NOT_OPEN;
        throw Exception::Open(errno);
      }

      if(_lock)
      {
        if(lockf(this->fd, F_TLOCK,0) != 0)
        {
          throw Exception::Locked();
        }
      }

      _currentStatus = Stream::OPENED;
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
        _currentStatus = 0;
      }
      else
      {
        throw Exception::Close();
      }
    }

    int Base::getStatus() const
    {
      return _currentStatus;
    }

    int Base::getWaitStatus() const
    {
      return _waitStatus;
    }

    void Base::setDelegate(Stream::Delegate * newDelegate)
    {
      _delegate = newDelegate;
    }

    void Base::setNoCTtyFlag()
    {
      _flags |= O_NOCTTY;
    }

    void Base::resetCTtyFlag()
    {
      _flags &= ~O_NOCTTY;
    }

    void Base::setControlMode(tcflag_t c_cflag) throw(Exception::Configuration)
    {
      _config.c_cflag = c_cflag;

      if(!(fd < 0))
      {
        applyConfiguration();
      }
    }

    void Base::setLocalMode(tcflag_t c_lflag) throw(Exception::Configuration)
    {
      _config.c_lflag = c_lflag;

      if(!(fd < 0))
      {
        applyConfiguration();
      }
    }

    void Base::setLine(cc_t c_line) throw(Exception::Configuration)
    {
      _config.c_line = c_line;

      if(!(fd < 0))
      {
        applyConfiguration();
      }
    }

    void Base::setLowLatency(bool isLowLatency) throw(Exception::Configuration, Overkiz::Errno::Exception)
    {
      if(fd < 0)
        throw Exception::Configuration();

      struct serial_struct new_serinfo;

      if(ioctl(fd, TIOCGSERIAL, &new_serinfo) < 0)
        throw Overkiz::Errno::Exception();

      if(isLowLatency)
        new_serinfo.flags |= ASYNC_LOW_LATENCY;
      else
        new_serinfo.flags &= ~ASYNC_LOW_LATENCY;

      if(ioctl(fd, TIOCSSERIAL, &new_serinfo) < 0)
        throw Overkiz::Errno::Exception();
    }

    void Base::setLatency(uint32_t latencyTimeInMs) throw(Exception::Configuration, Overkiz::Errno::Exception,  Exception::Latency)
    {
      if(fd < 0)
      {
        throw Exception::Configuration();
      }

      // only for usb-serial => ttyUSB
      if(_path.find("ttyUSB") == std::string::npos)
      {
        // nothing to do
        return;
      }

      size_t pos =  _path.find_last_of('/');

      if(pos == std::string::npos || (_path.length()-pos) < sizeof("ttyUSB0"))
      {
        throw Exception::Configuration();
      }

      pos++; // drop '/'
      std::string latencyPath = "/sys/bus/usb-serial/devices/"+  _path.substr(pos) + "/latency_timer";
      struct stat lt;

      if(::stat(latencyPath.c_str(), &lt) < 0)
      {
        OVK_DEBUG("%s > file not found : %s ",__FUNCTION__ ,latencyPath.c_str());
        return;
      }

      // unlock
      if(latencyTimeInMs == 1)
      {
        setLowLatency(true); // set to 1ms
        return;
      }
      else
      {
        setLowLatency(false); // set to 16 ms

        if(latencyTimeInMs == 16)
        {
          return;
        }
      }

      int sysFd = ::open(latencyPath.c_str(), O_RDWR|O_TRUNC, S_IRWXU);

      if(sysFd < 0)
      {
        throw Overkiz::Errno::Exception();
      }

      std::string value= std::to_string(latencyTimeInMs);

      if(::write(sysFd, value.c_str(), value.length()) != (ssize_t)value.length())
      {
        ::close(sysFd);
        throw Overkiz::Errno::Exception();
      }

      char data[15];
      memset(data,0x00,sizeof(data));

      if(lseek(sysFd,0,SEEK_SET) < 0)
      {
        ::close(sysFd);
        throw Overkiz::Errno::Exception();
      }

      if(::read(sysFd, data, sizeof(data) -1) < 0)
      {
        ::close(sysFd);
        throw Overkiz::Errno::Exception();
      }

      uint32_t latency = atoi(data);
      ::close(sysFd);

      if(latency != latencyTimeInMs)
      {
        OVK_ERROR("Failed to set latency %u != %u",latency , latencyTimeInMs);
        throw Exception::Latency();
      }
    }

    void Base::setControlCharacters(int index, char value)
    throw(Exception::Configuration)
    {
      _config.c_cc[index] = value;

      if(!(fd < 0))
      {
        applyConfiguration();
      }
    }

    void Base::flush(FlushType type)
    {
      if(tcflush(fd, type) < 0)
      {
        throw Overkiz::Errno::Exception();
      }
    }

    void Base::applyConfiguration(int when) throw(Exception::Configuration)
    {
      if(tcsetattr(this->fd, when, &_config) < 0)
      {
        throw Exception::Configuration();
      }
    }

    Input::Input(const std::string & path)
    {
      _path = path;
      _flags = O_RDONLY | O_NOCTTY | O_NONBLOCK;
    }

    Input::~Input()
    {
    }

    int Input::wait(int status)
    {
      Base::wait(status);

      if(status & Stream::INPUT_READY)
      {
        if(!(_currentStatus & Stream::INPUT_READY)
           && !(_waitStatus & Stream::INPUT_READY))
        {
          modify(EPOLLIN | events);
          _waitStatus |= Stream::INPUT_READY;
        }
      }
      else
      {
        _waitStatus &= ~Stream::INPUT_READY;
        modify(~EPOLLIN & events);
      }

      return _currentStatus;
    }

    void Input::process(uint32_t evts)
    {
      if((evts & EPOLLERR) || (evts & EPOLLHUP))
      {
        this->close();
        _delegate->ready(_currentStatus);
      }
      else if(evts & EPOLLIN)
      {
        _currentStatus |= Stream::INPUT_READY;
        _delegate->ready(_currentStatus);
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
        _currentStatus &= ~Stream::INPUT_READY;
      }

      return dataRead;
    }

    void Input::setInputMode(tcflag_t c_iflag) throw(Exception::Configuration)
    {
      _config.c_iflag = c_iflag;

      if(!(fd < 0))
      {
        applyConfiguration(TCSAFLUSH);
      }
    }

    void Input::setInputSpeed(speed_t c_ispeed) throw(Exception::Configuration)
    {
      if(cfsetispeed(&_config, c_ispeed) < 0)
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
      _path = path;
      _flags = O_WRONLY | O_NOCTTY | O_NONBLOCK;
    }

    Output::~Output()
    {
    }

    int Output::wait(int status)
    {
      Base::wait(status);

      if(status & Stream::OUTPUT_READY)
      {
        if(!(_currentStatus & Stream::OUTPUT_READY)
           && !(_waitStatus & Stream::OUTPUT_READY))
        {
          modify(EPOLLOUT | events);
          _waitStatus |= Stream::OUTPUT_READY;
        }
      }
      else
      {
        _waitStatus &= ~Stream::OUTPUT_READY;
        modify(~EPOLLOUT & events);
      }

      return _currentStatus;
    }

    void Output::process(uint32_t evts)
    {
      if((evts & EPOLLERR) || (evts & EPOLLHUP))
      {
        this->close();
        _delegate->ready(_currentStatus);
      }
      else if(evts & EPOLLOUT)
      {
        _currentStatus |= Stream::OUTPUT_READY;
        _delegate->ready(_currentStatus);
        _currentStatus &= ~Stream::OUTPUT_READY;
      }
    }

    size_t Output::write(const void *data, size_t size, bool more)
    throw(Exception::Write)
    {
      ssize_t dataWritten = 0;

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

      if((size_t)dataWritten < size)
      {
        _currentStatus &= ~Stream::OUTPUT_READY;
      }

      return dataWritten;
    }

    void Output::setOutputMode(tcflag_t c_oflag) throw(Exception::Configuration)
    {
      _config.c_oflag = c_oflag;

      if(!(fd < 0))
      {
        applyConfiguration(TCSADRAIN);
      }
    }

    void Output::setOutputSpeed(speed_t c_ospeed) throw(Exception::Configuration)
    {
      if(cfsetospeed(&_config, c_ospeed) < 0)
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
      _path = path;
      _flags = O_RDWR | O_NOCTTY | O_NONBLOCK;
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
          if(!(_currentStatus & Stream::INPUT_READY))
          {
            newEvent |= EPOLLIN;
            _waitStatus |= Stream::INPUT_READY;
          }
        }
        else
        {
          _waitStatus &= ~Stream::INPUT_READY;
          newEvent &= ~EPOLLIN;
        }

        if(status & Stream::OUTPUT_READY)
        {
          if(!(_currentStatus & Stream::OUTPUT_READY))
          {
            newEvent |= EPOLLOUT;
            _waitStatus |= Stream::OUTPUT_READY;
          }
        }
        else
        {
          _waitStatus &= ~Stream::OUTPUT_READY;
          newEvent &= ~EPOLLOUT;
        }

        modify(newEvent);
      }

      return _currentStatus;
    }

    void InputOutput::process(uint32_t evts)
    {
      if((evts & EPOLLERR) || (evts & EPOLLHUP))
      {
        this->close();
        _delegate->ready(_currentStatus);
      }
      else if((evts & EPOLLIN) || (evts & EPOLLOUT))
      {
        if(evts & EPOLLIN)
        {
          _currentStatus |= Stream::INPUT_READY;
        }

        if(evts & EPOLLOUT)
        {
          _currentStatus |= Stream::OUTPUT_READY;
        }

        _delegate->ready(_currentStatus);
        _currentStatus &= ~Stream::OUTPUT_READY;
      }
    }

  }

}
