/*
 * Process.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <unistd.h>
#include <sys/wait.h>

#include <kizbox/framework/core/Errno.h>
#include <kizbox/framework/core/Log.h>
#include "Process.h"

namespace Overkiz
{
  Process::Process()
  {
    id = getpid();
    state = 0;
  }

  Process::~Process()
  {
  }

  void Process::exec(const char *name, char * const argv[], char * const envp[])
  {
    if(execve(name, argv, envp) == -1)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  bool Process::fork(const Shared::Pointer<OnChildTerminated> & sub)
  {
    bool ret = 0;
    //Subscribe to this signal to wait terminated

    if(current.empty())
      current = Shared::Pointer<Process>::create();

    Signal::Manager::add(SIGCLD, & (*current));
    pid_t pid = ::fork();

    if(pid < 0)
    {
      throw Overkiz::Errno::Exception();
    }

    Shared::Pointer<Process> proc = Shared::Pointer<Process>::create();

    if(!pid)
    {
      //child
      current = proc;
      current->id = getpid();
      ret = true;
    }
    else
    {
      //parent
      proc->id = pid;
      lock.acquire();
      current->children[pid] = proc;
      proc->terminatedListeners = sub;
      lock.release();
    }

    return ret;
  }

  void Process::send(uint32_t signal)
  {
    if(kill(id, signal))
    {
      throw Overkiz::Errno::Exception();
    }
  }

  void Process::handle(const Signal& signal)
  {
    if(signal.info.ssi_signo == SIGCLD)
    {
      int pid, state;

      while((pid = waitpid(-1, &state, WNOHANG)) > 0)
      {
        const auto& f = current->children.find(pid);

        if(f != current->children.end())
        {
          if(WIFEXITED(state) || WIFSIGNALED(state))
          {
            lock.acquire();

            if(!f->second->terminatedListeners.empty())
              f->second->terminatedListeners->terminated(state);

            current->children.erase(pid);

            if(current->children.empty())
            {
              Signal::Manager::remove(SIGCLD, this);
            }

            lock.release();
          }
        }
      }
    }
  }

  Shared::Pointer<Process> Process::get()
  {
    Shared::Pointer<Process> proc;
    lock.acquire();
    proc = current;
    lock.release();
    return proc;
  }

  int Process::vsystem(const char* command)
  {
    //Vfork breaks POSIX compliant (POS33-C)
    pid_t pid = 0;
    int status = EXIT_FAILURE;
    OVK_DEBUG("System run %s.", command);
    pid = vfork(); /* Replace by posix_spawn() ? */

    if(pid == -1)
    {
      OVK_ERROR("System call failed. Failed to run \"%s\"! (%s)", command, strerror(errno));
    }
    else if(pid == 0) /* child */
    {
      if(execl("/bin/sh", "sh", "-c", command, (char *)nullptr) == -1)
      {
        OVK_ERROR("System call failed. Failed to run \"%s\"! (%s)", command, strerror(errno));
      }

      _exit(1);  /* in case execve() fails */
    }
    else
    {
      waitpid(pid, &status, 0);
    }

    return status;
  }

  Thread::Lock Process::lock;
  Shared::Pointer<Process> Process::current ;
}
