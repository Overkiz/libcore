/*
 * Process.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_PROCESS_H_
#define OVERKIZ_PROCESS_H_

#include <map>
#include <sched.h>

#include <kizbox/framework/core/Shared.h>
#include <kizbox/framework/core/Thread.h>
#include <kizbox/framework/core/Signal.h>

namespace Overkiz
{
  class Process: Signal::Handler
  {
  public:

    void send(uint32_t signal);

    static void exec(const char *, char * const argv[], char * const envp[]);

    static Shared::Pointer<Process> fork();

    static Shared::Pointer<Process> get();

    static int vsystem(const char* command);

  private:

    Process();

    virtual ~Process();

    void handle(const Signal& signal);

    pid_t id;
    int state;

    static std::map<pid_t, Shared::Pointer<Process>> children;

    static Thread::Lock lock;
    static Shared::Pointer<Process> current;

    template<typename T> friend class Shared::Pointer;

  };

}

#endif /* OVERKIZ_PROCESS_H_ */
