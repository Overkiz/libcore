/*
 * Coroutine.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <config.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cstring>
#include <cstdio>

#include <kizbox/framework/core/Errno.h>

extern "C" {
  #ifdef VALGRIND
#include <valgrind/valgrind.h>
  #endif
}

#include "Log.h"
#include "Coroutine.h"

#ifndef MPROTECT_SIZE
  #define MPROTECT_SIZE 1
#endif

namespace Overkiz
{
  #ifndef ASM_COROUTINE
  void Coroutine::launchCoro(Coroutine * coroutine)
  {
    coroutine->launch();

    if(setcontext(& coroutine->caller->ctx) == -1)
      throw Overkiz::Errno::Exception();
  }
  #endif

  Coroutine::Coroutine(size_t initsize)
  {
    state = Status::STOPPED;
    int pgsize = getpagesize();

    if(initsize % pgsize)
    {
      size = ((initsize / pgsize) + 1) * pgsize;
    }
    else
    {
      size = initsize;
    }

    size += (2 * MPROTECT_SIZE * pgsize);
    stack.base = mmap(nullptr, size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(stack.base == MAP_FAILED)
    {
      throw;
    }

    mprotect(stack.base, MPROTECT_SIZE * pgsize, PROT_NONE);
    mprotect((unsigned char *) stack.base + size - (MPROTECT_SIZE * pgsize),
             MPROTECT_SIZE * pgsize, PROT_NONE);
    #ifdef VALGRIND
    valgrind = VALGRIND_STACK_REGISTER(
                 (unsigned char *)stack.base + (MPROTECT_SIZE * pgsize),
                 (unsigned char *)stack.base + size - (MPROTECT_SIZE * pgsize) - 1);
    #endif
    #ifdef ASM_COROUTINE
    stack.top = Architecture::Context::init(
                  (unsigned char *) stack.base + size - (MPROTECT_SIZE * pgsize) - 1,
                  this);
    #else

    if(getcontext(&ctx) == -1)
      throw Overkiz::Errno::Exception();

    ctx.uc_stack.ss_size = size - (MPROTECT_SIZE * pgsize);
    ctx.uc_stack.ss_sp = stack.base;
    ctx.uc_link = nullptr;
    #endif
  }

  Coroutine::Coroutine()
  {
    stack.base = nullptr;
    stack.top = nullptr;
    size = 0;
    state = Status::RUNNING;
    #ifdef VALGRIND
    valgrind = 0;
    #endif
    #ifndef ASM_COROUTINE

    if(getcontext(&ctx) == -1)
      throw Overkiz::Errno::Exception();

    #endif
  }

  Coroutine::~Coroutine()
  {
    #ifdef VALGRIND
    VALGRIND_STACK_DEREGISTER(valgrind);
    #endif

    if(stack.base)
    {
      munmap(stack.base, size);

      if(state != Status::STOPPED)
      {
        OVK_ERROR("Coroutine destroyed while running.");
      }
    }
  }

  void Coroutine::resume(Shared::Pointer<Coroutine>& coro)
  {
    if(!coro->stack.base)
    {
      OVK_ERROR("Coroutine not initialized. Couldn't resume.");
      throw Coroutine::Exception();
    }

    if(coro->state != Status::STOPPED && coro->state != Status::PAUSED)
    {
      OVK_ERROR("Coroutine is already running. Couldn't resume.");
      throw Coroutine::Exception();
    }

    coro->caller = self();
    const Shared::Pointer<Coroutine>& coroutine = coro;
    coroutine->caller->state = Status::WAITING;
    *current = coroutine;

    if(coroutine->state == Status::PAUSED)
    {
      coroutine->restore();
    }

    #ifdef ASM_COROUTINE
    Architecture::Context::resume(coroutine->stack.top);
    #else

    if(coroutine->state == Status::STOPPED)
    {
      makecontext(&coroutine->ctx, (void (*)(void))launchCoro, 1, &(*coroutine));
    }

    if(swapcontext(&coroutine->caller->ctx, &coroutine->ctx) == -1)
      throw Overkiz::Errno::Exception();

    #endif
    *current = coroutine->caller;
    coroutine->caller->state = Status::RUNNING;
    coroutine->caller = Shared::Pointer<Coroutine>();
  }

  void Coroutine::yield()
  {
    Shared::Pointer<Coroutine> coro = *current;

    if(!coro.empty())
    {
      Shared::Pointer<Coroutine> parent = coro->caller;
      coro->state = Status::PAUSED;
      coro->save();
      #ifdef ASM_COROUTINE
      Architecture::Context::yield(coro->stack.top);
      #else

      //Save current context
      if(swapcontext(&coro->ctx, &parent->ctx) == -1)
        throw Overkiz::Errno::Exception();

      #endif
      coro->state = Status::RUNNING;
    }
    else
    {
      OVK_ERROR("Coroutine not supported.");
      throw Coroutine::Exception();
    }
  }

  void Coroutine::launch()
  {
    state = Status::RUNNING;
    entry();
    state = Status::STOPPED;
  }

  Coroutine::Status Coroutine::status() const
  {
    return state;
  }

  Shared::Pointer<Coroutine>& Coroutine::self()
  {
    if(current->empty())
    {
      *current = Shared::Pointer<Coroutine>::create();
    }

    return *current;
  }

  size_t Coroutine::stackSize()
  {
    return size - (2 * getpagesize());
  }

  Thread::Key<Coroutine> Coroutine::current;

}
