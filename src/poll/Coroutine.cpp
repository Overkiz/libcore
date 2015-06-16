/*
 * Coroutine.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <config.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>

extern "C" {
  #ifdef VALGRIND
#include <valgrind/valgrind.h>
  #endif
}

#include "Coroutine.h"

#ifndef MPROTECT_SIZE
  #define MPROTECT_SIZE 1
#endif

namespace Overkiz
{

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
    stack.base = mmap(NULL, size, PROT_READ | PROT_WRITE,
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
    stack.top = Architecture::Context::init(
                  (unsigned char *) stack.base + size - (MPROTECT_SIZE * pgsize) - 1,
                  this);
  }

  Coroutine::Coroutine()
  {
    stack.base = NULL;
    size = 0;
    state = Status::RUNNING;
    #ifdef VALGRIND
    valgrind = 0;
    #endif
  }

  Coroutine::~Coroutine()
  {
    #ifdef VALGRIND
    VALGRIND_STACK_DEREGISTER(valgrind);
    #endif

    if(stack.base)
    {
      if(state != Status::STOPPED)
      {
        throw;
      }

      munmap(stack.base, size);
    }
  }

  void Coroutine::resume(Shared::Pointer<Coroutine>& coro)
  {
    if(!coro->stack.base)
    {
      fprintf(stderr, "Coroutine not initialized\n");
      throw;
    }

    if(coro->state != Status::STOPPED && coro->state != Status::PAUSED)
    {
      fprintf(stderr, "Coroutine is running\n");
      throw;
    }

    coro->caller = self();
    Shared::Pointer<Coroutine> coroutine = coro;
    coroutine->caller->state = Status::WAITING;
    *current = coroutine;

    if(coroutine->state == Status::PAUSED)
    {
      coroutine->restore();
    }

    Architecture::Context::resume(coroutine->stack.top);
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
      Architecture::Context::yield(coro->stack.top);
      coro->state = Status::RUNNING;
    }
    else
    {
      fprintf(stderr, "Coroutine not supported\n");
      throw;
    }
  }

  void Coroutine::launch()
  {
    state = Status::RUNNING;
    entry();
    state = Status::STOPPED;
    int pgsize = getpagesize();
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
