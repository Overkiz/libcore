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
        if(current)
        {
          current->symbols.insert(&symbol);
        }
      }

      __attribute__((destructor)) static void remove()
      {
        if(current)
        {
          current->symbols.erase(&symbol);
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
          handler = NULL;
          symbol = NULL;
        }

        Reference(const Reference& val)
        {
          handler = NULL;
          symbol = NULL;

          if(val.handler)
          {
            Library::lock.acquire();
            handler = val.handler;
            handler->count++;
            symbol = val.symbol;
            Library::lock.release();
          }
        }

        template<typename T>
        Reference(const Reference<T>& val)
        {
          handler = NULL;
          symbol = NULL;

          if(val.symbol && val.handler)
          {
            S *tmp = dynamic_cast<S *>(val.symbol);

            if(tmp)
            {
              Library::lock.acquire();
              handler = val.handler;
              handler->count++;
              symbol = tmp;
              Library::lock.release();
            }
          }
        }

        virtual ~Reference()
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

        S& operator *()
        {
          return *symbol;
        }

        const S& operator *() const
        {
          return *symbol;
        }

        S *operator ->()
        {
          return symbol;
        }

        const S *operator ->() const
        {
          return symbol;
        }

        bool valid() const
        {
          return symbol != NULL;
        }

      private:

        S *symbol;
        Handler *handler;

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

      for(std::set<Symbol *>::iterator i = handler->symbols.begin();
          i != handler->symbols.end(); ++i)
      {
        if(!strcmp(name, (*i)->name()))
        {
          T *tmp = dynamic_cast<T *>(*i);

          if(tmp)
          {
            ret.handler = handler;
            Library::lock.acquire();
            handler->count++;
            Library::lock.release();
            ret.symbol = tmp;
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
      int count;
      void *handler;
      std::set<Symbol *, Symbol::Comparator> symbols;
      friend class Symbol;
      friend class Library;

    };

    Handler *handler;

    static std::map<void *, Handler> handlers;
    static Handler *current;
    static Thread::Lock lock;

    friend class Sym;
  };

  template<typename S>
  S Library::Export<S>::symbol;

}

#endif /* OVERKIZ_LIBRARY_H_ */
