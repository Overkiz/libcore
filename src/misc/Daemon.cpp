/*
 * Daemon.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/file.h>

#include <config.h>
#include <kizbox/framework/core/Log.h>
#include <kizbox/framework/core/Errno.h>
#include <kizbox/framework/core/Signal.h>
#include "Daemon.h"

#define PIDDIR "PIDDIR"
#define BUFFERSIZE 255

namespace Overkiz
{

  Daemon::Daemon() : lock(0)
  {
    updatePidFile();
  }

  Daemon::~Daemon()
  {
    removePidFile();
  }

  void Daemon::updatePidFile()
  {
    std::string ret;
    const char * pidd = getenv(PIDDIR);

    if(!pidd)
    {
      return;
    }

    //get process name
    std::string name = std::string("/proc/") + std::to_string(getpid()) + "/comm";
    int fd = ::open(name.c_str(), O_RDONLY);

    if(fd < 0)
    {
      return;
    }

    char pname[BUFFERSIZE];
    int r = read(fd, pname, BUFFERSIZE);
    close(fd);

    if(r > 0)
    {
      pname[r-1] = '\0';
    }
    else
    {
      return;
    }

    pidfile = std::string(pidd)+ "/" + std::string(pname) + ".pid";
  }

  void Daemon::makePidFile(int pid)
  {
    if(pidfile.empty() || lock <= 0)
    {
      return;
    }

    int fd = ::open(pidfile.c_str(), O_WRONLY | O_TRUNC);

    if(fd < 0)
    {
      OVK_ERROR("Failed to create pid file. (%s)", pidfile.c_str());
      exit(1);
    }

    const std::string pids = std::to_string(pid);

    if(::write(fd, pids.c_str(), pids.size()) < 0)
    {
      OVK_ERROR("Failed to write pid file. (%s)", pidfile.c_str());
      exit(1);
    }

    close(fd);
    close(lock);
  }

  void Daemon::removePidFile()
  {
    if(!pidfile.empty())
      unlink(pidfile.c_str());
  }


  static void redirectToNull()
  {
    int n;

    if((n = open("/dev/null",O_RDWR)) == -1)
    {
      OVK_WARNING("failed to reopen stdin while daemonising (errno=%d)",errno);
    }
    else
    {
      for(auto i = STDIN_FILENO ; i < STDERR_FILENO +1 ; ++i)
      {
        if(dup2(n, i) < 0)
        {
          OVK_WARNING("failed to dup2  (fd=%d) while daemonising (%s)",i,strerror(errno));
        }
      }

      close(n);
    }
  }

  void Daemon::run(bool forkk)
  {
    if(forkk)
    {
      // Because Signalfd need to block signals, we need to release signals before fork
      Signal::Manager::clean();
      // Fork, allowing the parent process to terminate.
      pid_t pid = fork();

      if(pid == -1)
      {
        OVK_ERROR("failed to fork 1 while daemonising (errno=%d)",errno);
        exit(1);
      }
      else if(pid != 0)
      {
        int status;
        ::waitpid(pid, &status, 0);
        exit(status);
      }

      // Start a new session for the daemon.
      if(setsid()==-1)
      {
        OVK_ERROR("failed to become a session leader while daemonising (errno=%d)",errno);
        exit(1);
      }

      // Fork again, allowing the parent process to terminate.
      signal(SIGHUP,SIG_IGN);
      pid=fork();

      if(pid == -1)
      {
        OVK_ERROR("failed to fork 2 while daemonising (errno=%d)",errno);
        exit(1);
      }
      else if(pid != 0)
      {
        makePidFile(pid);
        exit(0);
      }

      close(lock);
      OVK_NOTICE("Daemonize process %s. (%i)", ::program_invocation_short_name, getpid());
      // Close and reopen standard file descriptors.
      redirectToNull();
      Signal::Manager::reload();
    }
    else
    {
      makePidFile(getpid());
    }
  }

  void Daemon::checkPidFile()
  {
    if(pidfile.empty())
    {
      OVK_WARNING("Pid directory is not set.");
      return;
    }

    lock = ::open(pidfile.c_str(), O_CREAT | O_RDONLY, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    if(lock < 0)
    {
      OVK_ERROR("Failed to open/create pid file. (%s)", pidfile.c_str());
      return;
    }

    flock(lock, LOCK_EX);
    char buff[BUFFERSIZE];
    int r = read(lock, buff, BUFFERSIZE);

    if(r > 0)
    {
      buff[r] = '\0';
      struct stat st;
      std::string proc = "/proc/" + std::string(buff);

      // If the pid is already taken by an app
      if(stat(proc.c_str(), &st) == 0)
      {
        char bf[BUFFERSIZE];
        char bf2[BUFFERSIZE];
        ssize_t rl = readlink(std::string(proc+"/exe").c_str(), bf, BUFFERSIZE);

        if(rl < 0)
        {
          OVK_ERROR("Error on readlink %s/exe.", proc.c_str());
          exit(1);
        }

        bf[rl] = '\0';
        ssize_t rl2 = readlink(std::string("/proc/" + std::to_string(getpid())+"/exe").c_str(), bf2, BUFFERSIZE);

        if(rl2 < 0)
        {
          OVK_ERROR("Error on reading /proc/pid/exe. (%i)", getpid());
          exit(1);
        }

        bf2[rl2] = '\0';

        // And the app have the same name, but not the same pid
        if(strcmp(bf, bf2) == 0)
        {
          if(std::string(buff).compare(std::to_string(getpid())) == 0)
          {
            OVK_NOTICE("Restarting Apps.");
          }
          else
          {
            OVK_ERROR("Application already running. (%s)", buff);
            exit(1);
          }
        }
        else
        {
          OVK_WARNING("An other apps is running.");
        }
      }
    }
    else if(r < 0)
    {
      OVK_ERROR("Failed to read pidfile. (%s)", pidfile.c_str());
    }
  }


}
