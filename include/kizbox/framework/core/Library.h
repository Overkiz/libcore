/*
 * Library.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_LIBRARY_H_
#define OVERKIZ_LIBRARY_H_

#include <set>
#include <map>
#include <stdio.h>
#include <string.h>
#include <typeinfo>

#include <kizbox/framework/core/Exception.h>
#include <kizbox/framework/core/Shared.h>
#include <kizbox/framework/core/Thread.h>

namespace Overkiz
{

  const std::string demangle(const char *name);

  class Library
  {
  private:

    class Handler;

  public:

    class Exception: public Overkiz::Exception
    {
    public:

      Exception()
      {
      }

      virtual ~Exception()
      {
      }

      virtual const char *getId() const
      {
        return "com.overkiz.Framework.Core.Library.Exception";
      }

    private:

    };

    template<typename S>
    class Export
    {
    public:
      Export()
      {
      }

      virtual ~Export()
      {
      }
    private:
      __attribute__((constructor)) static void add()
      {
        if(_current)
        {
          _current->_symbols.insert(&symbol);
        }
      }

      __attribute__((destructor)) static void remove()
      {
        if(_current)
        {
          _current->_symbols.erase(&symbol);
        }
      }

      static S symbol;

    };

    class Symbol
    {
    public:

      template<typename S>
      class Reference
      {
      public:
      public:
        Reference()
        {
          _handler = NULL;
          _symbol = NULL;
        }

        Reference(const Reference& val)
        {
          _handler = NULL;
          _symbol = NULL;

          if(val._handler)
          {
            Library::_lock.acquire();
            _handler = val._handler;
            _handler->_count++;
            _symbol = val._symbol;
            Library::_lock.release();
          }
        }

        template<typename T>
        Reference(const Reference<T>& val)
        {
          _handler = NULL;
          _symbol = NULL;

          if(val.symbol && val.handler)
          {
            S *tmp = dynamic_cast<S *>(val.symbol);

            if(tmp)
            {
              Library::_lock.acquire();
              _handler = val.handler;
              _handler->_count++;
              _symbol = tmp;
              Library::_lock.release();
            }
          }
        }

        virtual ~Reference()
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

        S& operator *()
        {
          return *_symbol;
        }

        const S& operator *() const
        {
          return *_symbol;
        }

        S *operator ->()
        {
          return _symbol;
        }

        const S *operator ->() const
        {
          return _symbol;
        }

        bool valid() const
        {
          return _symbol != NULL;
        }

      private:

        S *_symbol;
        Handler *_handler;

        friend class Library;
      };

      class Comparator
      {
      public:
        Comparator()
        {
        }

        virtual ~Comparator()
        {
        }

        bool operator()(const Symbol *a, const Symbol *b) const;
      private:

      };

      Symbol();

      virtual ~Symbol();

      virtual const char *name() const = 0;

    };

    Library();

    Library(const Library& val);

    virtual ~Library();

    Library& operator = (const Library& val);

    template<typename T>
    Symbol::Reference<T> find(const char *name)
    {
      Symbol::Reference<T> ret;

      for(std::set<Symbol *>::iterator i = _handler->_symbols.begin();
          i != _handler->_symbols.end(); ++i)
      {
        if(!strcmp(name, (*i)->name()))
        {
          T *tmp = dynamic_cast<T *>(*i);

          if(tmp)
          {
            ret._handler = _handler;
            Library::_lock.acquire();
            _handler->_count++;
            Library::_lock.release();
            ret._symbol = tmp;
            break;
          }
        }
      }

      return ret;
    }

    static Library open(const char *path);

    bool valid() const;

  private:

    class Handler
    {
    public:
      Handler();

      virtual ~Handler();

    private:
      int _count;
      void *_handler;
      std::set<Symbol *, Symbol::Comparator> _symbols;
      friend class Symbol;
      friend class Library;

    };

    Handler *_handler;

    static std::map<void *, Handler> _handlers;
    static Handler *_current;
    static Thread::Lock _lock;

    friend class Sym;
  };

  template<typename S>
  S Library::Export<S>::symbol;

}

#endif /* OVERKIZ_LIBRARY_H_ */
