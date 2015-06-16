/*
 * Shared.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_SHARED_H_
#define OVERKIZ_SHARED_H_

#include <assert.h>
#include <utility>
#include <typeinfo>
#include <type_traits>
#include <stdio.h>

#include <kizbox/framework/core/Exception.h>

namespace Overkiz
{

  namespace Shared
  {

    namespace Exception
    {

      class Nil: public Overkiz::Exception
      {
      public:

        virtual ~Nil()
        {
        }

        const char * getId() const
        {
          return "com.overkiz.Framework.Core.Shared.Nil";
        }

      };

      class InvalidCast: public Overkiz::Exception
      {
      public:

        InvalidCast(const char * typeName) :
          typeName(typeName)
        {
        }

        virtual ~InvalidCast()
        {
        }

        const char * getTypeName() const
        {
          return typeName;
        }

        const char * getId() const
        {
          return "com.overkiz.Framework.Core.Shared.InvalidCast";
        }

      protected:

        const char * typeName;

      };

    }

    class Counter
    {
    public:
      Counter() :
        count(1)
      {
      }

      virtual ~Counter()
      {
      }

    protected:
      int count;

      template<typename A> friend class Pointer;
    };

    template<typename T>
    class Pointer
    {
    public:
      class Factory;
      class Self;

    private:
      class SelfWrapper;

      class SelfReference
      {
      public:
        SelfReference() :
          container(NULL)
        {
        }
        SelfReference(SelfWrapper *cont) :
          container(cont)
        {
        }
        SelfReference(const SelfReference& src) :
          container(NULL)
        {
        }
        virtual ~SelfReference()
        {
        }

        SelfReference& operator = (const SelfReference& val)
        {
          return *this;
        }

        operator Shared::Pointer<T>()
        {
          Shared::Pointer<T> ret;

          if(container)
          {
            container->count++;
            ret.counter = container;
            ret.value = & (container->value);
          }

          return ret;
        }

        operator Shared::Pointer<const T>() const
        {
          Shared::Pointer<const T> ret;

          if(container)
          {
            container->count++;
            ret.counter = container;
            ret.value = & (container->value);
          }

          return ret;
        }

        template<typename T1>
        operator Shared::Pointer<T1>()
        {
          return Shared::Pointer<T> (*this);
        }

        template<typename T1>
        operator Shared::Pointer<const T1>() const
        {
          return Shared::Pointer<const T> (*this);
        }

      protected:
        SelfWrapper *container;
      };

      class SelfInitializer: public virtual SelfReference
      {
      public:
        virtual ~SelfInitializer()
        {
        }

      };

      class SelfCleaner: public virtual SelfReference
      {
      public:
        virtual ~SelfCleaner()
        {
          SelfReference::container = NULL;
        }

      };

      class SelfWrapper: public Shared::Counter
      {
      public:
        class Container: public SelfInitializer, public T, public SelfCleaner
        {
        public:
          template<typename ...P>
          Container(SelfWrapper *count, P&&... params) :
            SelfReference(count), SelfInitializer(), T(
              std::forward<P> (params)...)
          {
          }
          virtual ~Container()
          {
          }
        private:
        };

        template<typename ...P>
        SelfWrapper(P&&... params) :
          value(this, std::forward<P> (params)...)
        {
        }
        virtual ~SelfWrapper()
        {
        }

      private:
        Container value;

        friend class Factory;
        friend class SelfReference;
      };

      class Wrapper: public Shared::Counter
      {
      public:
        template<typename ...P>
        Wrapper(P&&... params) :
          value(std::forward<P> (params)...)
        {
        }

        virtual ~Wrapper()
        {
        }

      protected:
        T value;

        friend class Factory;
        friend class Self;
      };

    public:

      class Self: public virtual SelfReference
      {
      public:
        virtual ~Self()
        {
        }
      };

      class Factory
      {
      public:
        template<typename ...P>
        static Pointer create(P&&... params)
        {
          Pointer ret;

          if(!std::is_base_of<Self, T>::value)
          {
            Wrapper *w = new Wrapper(std::forward<P> (params)...);
            ret.counter = w;
            ret.value = & (w->value);
          }
          else
          {
            SelfWrapper *w = new SelfWrapper(std::forward<P> (params)...);
            ret.counter = w;
            ret.value = & (w->value);
          }

          return ret;
        }

      };

      Pointer() :
        value(NULL), counter(NULL)
      {
      }

      Pointer(const Pointer& val) :
        value(NULL), counter(NULL)
      {
        counter = val.counter;

        if(counter)
        {
          (counter->count)++;
          value = val.value;
        }
      }

      template<typename T1>
      Pointer(const Pointer<T1>& val) :
        value(NULL), counter(NULL)
      {
        if(val.value)
        {
          value = dynamic_cast<T *>(val.value);

          if(value)
          {
            counter = val.counter;
            (counter->count)++;
          }
          else
          {
            throw Exception::InvalidCast(typeid(value).name());
          }
        }
      }

      virtual ~Pointer()
      {
        if(value)
        {
          value = NULL;
          (counter->count)--;

          if(counter->count < 1)
          {
            delete counter;
          }

          counter = NULL;
        }
      }

      T *operator ->() const
      {
        if(!value)
        {
          throw Exception::Nil();
        }

        return value;
      }

      T& operator *() const
      {
        if(!value)
        {
          throw Exception::Nil();
        }

        return *value;
      }

      Pointer& operator = (const Pointer& val)
      {
        if(this != &val)
        {
          if(value)
          {
            value = NULL;
            (counter->count)--;

            if(counter->count < 1)
            {
              delete counter;
            }

            counter = NULL;
          }

          value = val.value;

          if(value)
          {
            counter = val.counter;
            (counter->count)++;
          }
        }

        return *this;
      }

      template<typename T1>
      Pointer& operator = (const Pointer<T1>& val)
      {
        T *tmp = dynamic_cast<T *>(val.value);

        if(val.value && !tmp)
        {
          throw Exception::InvalidCast(typeid(value).name());
        }

        if(value != tmp)
        {
          if(value)
          {
            value = NULL;
            (counter->count)--;

            if(counter->count < 1)
            {
              delete counter;
            }

            counter = NULL;
          }

          if(tmp)
          {
            value = tmp;
            counter = val.counter;
            (counter->count)++;
          }
        }

        return *this;
      }

      bool empty() const
      {
        return (value == NULL);
      }

      bool operator == (const Pointer& val) const
      {
        return val.value == value;
      }

      template<typename T1>
      bool operator == (const Pointer<T1>& val) const
      {
        T *tmp = dynamic_cast<T *>(val.value);
        return tmp == value;
      }

      bool operator == (const T *val) const
      {
        return val == value;
      }

      template<typename T1>
      bool operator == (const T1 *val) const
      {
        T *tmp = dynamic_cast<T *>(val);
        return tmp == value;
      }

      bool operator != (const Pointer& val) const
      {
        return !(val.value == value);
      }

      template<typename T1>
      bool operator != (const Pointer<T1>& val) const
      {
        return !(val.value == value);
      }

      bool operator != (const T *val) const
      {
        return !(val == value);
      }

      template<typename T1>
      bool operator != (const T1 *val) const
      {
        T *tmp = dynamic_cast<T *>(val);
        return !(tmp == value);
      }

      template<typename ...P>
      static Pointer create(P&&... params)
      {
        Pointer ret = Factory::create(std::forward<P> (params)...);
        return ret;
      }

    protected:
      T *value;
      Shared::Counter *counter;

      template<typename A> friend class Pointer;
      friend class Self;
    };

  }

}

#endif /* OVERKIZ_SHARED_H_ */
