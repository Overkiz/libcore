/*
 * Pipe.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

#include <kizbox/framework/core/Errno.h>
#include <kizbox/framework/core/Log.h>
#include "Pipe.h"

namespace Overkiz
{

  namespace Pipe
  {

    namespace Named
    {

      Base::Base()
      {
      }

      Base::~Base()
      {
      }

      void Base::open(const std::string & path)
      {
        open(path, O_RDONLY);
      }

      void Base::close()
      {
        stop();
        ::close(fd);
        fd = -1;
      }

      void Base::setStackSize(size_t stkSize)
      {
        Watcher::setStackSize(stkSize);
      }

      void Base::open(const std::string & path, int openFcntl)
      {
        std::string checkPath = path;

        //Check if path contain .fifo
        if(path.find(".fifo") == std::string::npos)
        {
          checkPath = path + ".fifo";
        }

        if(!fileExists(checkPath))
        {
          if(mkfifo(checkPath.c_str(), S_IRWXU | S_IRGRP | S_IWGRP) == -1)
          {
            OVK_ERROR("Pipe creating error");
            throw PipeException();
          }
        }

        if((fd = ::open(checkPath.c_str(), openFcntl | O_NONBLOCK)) == -1)
        {
          OVK_ERROR("Pipe opening error.");
          throw Overkiz::Errno::Exception();
        }

        modify(EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR);
        start();
      }

      bool Base::fileExists(const std::string & path)
      {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
      }

      void Base::process(uint32_t evts)
      {
        uint32_t notifyValue = 0;
        OVK_TRACE("Events : %i\n", evts);

        if(evts & EPOLLIN)
        {
          //Remember that we have data to read in this socket
          notifyValue |= INPUT_READY;
          modify(events & ~EPOLLIN);
        }

        if(evts & EPOLLOUT)
        {
          notifyValue |= OUTPUT_READY;
          modify(events & ~EPOLLOUT);
        }

        if(evts & (EPOLLHUP))
        {
          notifyValue |= DISCONNECTED;
          modify(events & ~EPOLLHUP);
          stop();
        }

        if(evts & EPOLLERR)
        {
          notifyValue |= ERROR;
          modify(events & ~EPOLLERR);
          stop();
        }

        notify(notifyValue);
      }

      Server::Server()
      {
      }

      Server::~Server()
      {
      }

      int Server::receive(char * buff, size_t maxLen)
      {
        int ret;
        ret = read(fd, buff, maxLen);

        if(ret < 0)
        {
          OVK_ERROR("Error receive to pipe.");
          throw Overkiz::Errno::Exception();
        }

        modify(events | EPOLLIN);
        return ret;
      }

      Client::Client()
      {
      }

      Client::~Client()
      {
      }

      void Client::open(const std::string & path)
      {
        Base::open(path, O_WRONLY);
      }

      int Client::send(const char * buff, size_t len)
      {
        if(len > PIPE_BUF)
        {
          len = PIPE_BUF;
        }

        int ret;
        ret = ::write(fd, buff, len);

        if(ret < 0)
        {
          OVK_ERROR("Error send to pipe.");
          throw Overkiz::Errno::Exception();
        }

        return ret;
      }

    }

  }

}
