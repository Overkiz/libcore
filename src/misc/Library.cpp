/*
 * Library.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <cstring>
#include <dlfcn.h>
#include <cxxabi.h>

#include "Library.h"

namespace Overkiz
{

  const std::string demangle(const char *name)
  {
    int status = -4; /* Meaningless value. Real ones start at -3 */
    char *res = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    const std::string ret_val((status == 0) ? res : name);
    free(res);
    return ret_val;
  }

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
    _handler = nullptr;
  }

  Library::Library(const Library& val)
  {
    _handler = nullptr;
    operator = (val);
  }

  Library::~Library()
  {
    if(_handler)
    {
      Library::_lock.acquire();
      _handler->_count--;

      if(!_handler->_count)
      {
        Library::_handlers.erase(_handler->_handler);
      }

      Library::_lock.release();
    }
  }

  Library& Library::operator = (const Library& val)
  {
    _handler = val._handler;

    if(_handler)
    {
      Library::_lock.acquire();
      _handler->_count++;
      Library::_lock.release();
    }

    return *this;
  }

  bool Library::valid() const
  {
    return _handler != nullptr;
  }

  Library Library::open(const char *path)
  {
    Library ret;
    _lock.acquire();
    Handler h;
    _current = &h;
    void *handler = dlopen(path, RTLD_LAZY | RTLD_GLOBAL);

    if(handler)
    {
      auto i = _handlers.find(handler);

      if(i != _handlers.end())
      {
        ret._handler = & (i->second);
        ret._handler->_count++;
      }
      else
      {
        _handlers[handler] = h;
        ret._handler = & (_handlers[handler]);
        ret._handler->_count++;
      }
    }
    else
    {
      printf("dlopen error = %s\n", dlerror());
    }

    _current = nullptr;
    _lock.release();
    return ret;
  }

  Library::Handler::Handler()
  {
    _count = 0;
    _handler = nullptr;
  }

  Library::Handler::~Handler()
  {
    _current = this;

    if(_handler)
    {
      dlclose(_handler);
    }

    _current = nullptr;
  }

  std::map<void *, Library::Handler> Library::_handlers;
  Library::Handler *Library::_current;
  Thread::Lock Library::_lock;

}
