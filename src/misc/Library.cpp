/*
 * Library.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <string.h>
#include <dlfcn.h>

#include "Library.h"

namespace Overkiz
{

  Library::Symbol::Symbol()
  {
  }

  Library::Symbol::~Symbol()
  {
  }

  bool Library::Symbol::Comparator::operator()(const Symbol *a,
      const Symbol *b) const
  {
    return strcmp(a->name(), b->name()) < 0;
  }

  Library::Library()
  {
    handler = NULL;
  }

  Library::Library(const Library& val)
  {
    handler = NULL;
    operator = (val);
  }

  Library::~Library()
  {
    if(handler)
    {
      Library::lock.acquire();
      handler->count--;

      if(!handler->count)
      {
        Library::handlers.erase(handler->handler);
      }

      Library::lock.release();
    }
  }

  Library& Library::operator = (const Library& val)
  {
    handler = val.handler;

    if(handler)
    {
      Library::lock.acquire();
      handler->count++;
      Library::lock.release();
    }

    return *this;
  }

  bool Library::valid() const
  {
    return handler != NULL;
  }

  Library Library::open(const char *path)
  {
    Library ret;
    lock.acquire();
    Handler h;
    current = &h;
    void *handler = dlopen(path, RTLD_LAZY | RTLD_GLOBAL);

    if(handler)
    {
      std::map<void *, Handler>::iterator i = handlers.find(handler);

      if(i != handlers.end())
      {
        ret.handler = & (i->second);
        ret.handler->count++;
      }
      else
      {
        handlers[handler] = h;
        ret.handler = & (handlers[handler]);
        ret.handler->count++;
      }
    }
    else
    {
      printf("dlopen error = %s\n", dlerror());
    }

    current = NULL;
    lock.release();
    return ret;
  }

  Library::Handler::Handler()
  {
    count = 0;
    handler = 0;
  }

  Library::Handler::~Handler()
  {
    current = this;

    if(handler)
    {
      dlclose(handler);
    }

    current = NULL;
  }

  std::map<void *, Library::Handler> Library::handlers;
  Library::Handler *Library::current;
  Thread::Lock Library::lock;

}
