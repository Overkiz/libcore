/*
 * Process.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <unistd.h>
#include <sys/wait.h>

#include <kizbox/framework/core/Errno.h>
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
    execve(name, argv, envp);
  }

  Shared::Pointer<Process> Process::fork()
  {
    pid_t pid = ::fork();

    if(pid < 0)
    {
      throw Overkiz::Errno::Exception();
    }

    Shared::Pointer<Process> proc = Shared::Pointer<Process>::create();

    if(!pid)
    {
      proc->id = getpid();
      Signal::Manager::add(SIGCLD, & (*proc));
      current = proc;
    }
    else
    {
      int status;
      proc->id = pid;
      lock.acquire();
      current->children[pid] = proc;
      lock.release();

      if(waitpid(pid, &proc->state, WNOHANG) > 0)
      {
        if(WIFEXITED(proc->state) || WIFSIGNALED(proc->state))
        {
          lock.acquire();
          current->children.erase(pid);
          lock.release();
        }
      }
    }

    return proc;
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
      waitpid(signal.info.ssi_pid, &state, WNOHANG);

      if(WIFEXITED(state) || WIFSIGNALED(state))
      {
        lock.acquire();
        current->children.erase(signal.info.ssi_pid);
        lock.release();
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

  Thread::Lock Process::lock;
  Shared::Pointer<Process> Process::current = Shared::Pointer<Process>::create();
  std::map<pid_t, Shared::Pointer<Process>> Process::children;
}
