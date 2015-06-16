/*
 * Iterator.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_CORE_ITERATOR_H_
#define OVERKIZ_CORE_ITERATOR_H_

#include <vector>

#include <kizbox/framework/core/Exception.h>

namespace Overkiz
{

  namespace Iterator
  {

    namespace Exception
    {

      /**
       * Exception defining an Out of Bound Exception
       *
       */
      class Empty: public Overkiz::Exception
      {
      public:

        /**
         * Destructor.
         *
         */
        virtual ~Empty()
        {
        }

        /**
         * @see getId
         *
         */
        const char * getId() const
        {
          return "com.overkiz.Kizbox.Framework.Core.Iterator.Empty";
        }

      };

      /**
       * Exception defining an Out of Bound Exception
       *
       */
      class OutOfRange: public Overkiz::Exception
      {
      public:

        /**
         * Destructor.
         *
         */
        virtual ~OutOfRange()
        {
        }

        /**
         * @see getId
         *
         */
        const char * getId() const
        {
          return "com.overkiz.Kizbox.Framework.Core.Iterator.OutOfBound";
        }

      };

    }

    /**
     * Interface defining a generic iterator
     *
     */
    template<typename C>
    class Interface
    {
    public:

      /**
       * Destructor.
       *
       */
      virtual ~Interface()
      {
      }

      /**
       * reset resets the iterator.
       *
       * @return Itself
       */
      virtual Interface<C> & reset() = 0;

      /**
       * getNext returns the following element.
       *
       * @return Attribute element
       */
      virtual C getNext() = 0;

      /**
       * hasNext tests the end of the iterator.
       *
       * hasNext returns
       *  - true if the iterator still has an element
       *  - false if the iterator have reached the end
       *
       * @return boolean
       */
      virtual bool hasNext() = 0;

    };

    /**
     * Class implementing an empty iterator
     *
     */
    template<typename C>
    class Empty: public Interface<C>
    {
    public:

      /**
       * Constructor.
       *
       */
      Empty()
      {
      }

      /**
       * Destructor.
       *
       */
      virtual ~Empty()
      {
      }

      /**
       * @see reset
       *
       */
      Interface<C> & reset()
      {
        return *this;
      }

      /**
       * @see getNext
       *
       */
      C getNext()
      {
        Overkiz::Iterator::Exception::OutOfRange e;
        throw e;
      }

      /**
       * @see hasNext
       *
       */
      bool hasNext()
      {
        return false;
      }

    };

    /**
     * Class implementing a generic iterator with a single element
     *
     */
    template<typename C>
    class Single: public Interface<C>
    {
    public:

      /**
       * Constructor.
       *
       */
      Single(C element = NULL) :
        _element(element)
      {
        _next = (element != NULL);
      }

      /**
       * Destructor.
       *
       */
      virtual ~Single()
      {
      }

      /**
       * @see reset
       *
       */
      Interface<C> & reset()
      {
        if(_element)
        {
          _next = true;
          return *this;
        }
        else
        {
          Overkiz::Iterator::Exception::Empty e;
          throw e;
        }
      }

      /**
       * @see getNext
       *
       */
      C getNext()
      {
        if(_element)
        {
          if(_next)
          {
            _next = false;
            return _element;
          }
          else
          {
            Overkiz::Iterator::Exception::OutOfRange e;
            throw e;
          }
        }
        else
        {
          Overkiz::Iterator::Exception::Empty e;
          throw e;
        }
      }

      /**
       * @see hasNext
       *
       */
      bool hasNext()
      {
        if(_element)
        {
          return _next;
        }
        else
        {
          Overkiz::Iterator::Exception::Empty e;
          throw e;
        }
      }

    private:

      C _element;
      bool _next;

    };

    /**
     * Class implementing a generic iterator using an array
     *
     */
    template<typename C>
    class Array: public Interface<C>
    {
    public:

      /**
       * Constructor.
       *
       */
      Array(C * array = NULL, int * size = NULL) :
        _array(array), _index(0), _size(size)
      {
      }

      /**
       * Destructor.
       *
       */
      virtual ~Array()
      {
      }

      /**
       * set
       *
       */
      void set(C * array, int * size)
      {
        _array = array;
        _size = size;
      }

      /**
       * @see next
       *
       */
      Interface<C> & reset()
      {
        _index = 0;
        return *this;
      }

      /**
       * @see getNext
       *
       */
      C getNext()
      {
        if((_array) && (_size))
        {
          if(_index < *_size)
          {
            return _array[_index++];
          }
          else
          {
            Overkiz::Iterator::Exception::OutOfRange e;
            throw e;
          }
        }
        else
        {
          Overkiz::Iterator::Exception::Empty e;
          throw e;
        }
      }

      /**
       * @see hasNext
       *
       */
      bool hasNext()
      {
        if((_array) && (_size))
        {
          return (_index < *_size);
        }
        else
        {
          Overkiz::Iterator::Exception::Empty e;
          throw e;
        }
      }

    protected:

      C * _array;
      int _index;
      int * _size;

    };

    /**
     * Class implementing a generic iterator using an std container
     *
     */
    template<typename T, template<typename, typename > class C,
             typename A = std::allocator<T> >
    class Container: public Overkiz::Iterator::Interface<T>
    {
    public:

      typedef class C<T, A>::iterator It;

      /**
       * Constructor.
       *
       */
      Container(C<T, A> * container = NULL) :
        _container(container)
      {
        if(_container)
        {
          _iterator = _container->begin();
        }
      }

      /**
       * Destructor.
       *
       */
      virtual ~Container()
      {
      }

      /**
       * set
       *
       */
      void set(C<T, A> * container)
      {
        _container = container;

        if(_container)
        {
          _iterator = _container->begin();
        }
        else
        {
          Overkiz::Iterator::Exception::Empty e;
          throw e;
        }
      }

      /**
       * @see reset
       *
       */
      Interface<T> & reset()
      {
        if(_container)
        {
          _iterator = _container->begin();
          return *this;
        }
        else
        {
          Overkiz::Iterator::Exception::Empty e;
          throw e;
        }
      }

      /**
       * @see getNext
       *
       */
      T getNext()
      {
        if(_container)
        {
          if(_iterator != _container->end())
          {
            return * (_iterator++);
          }
          else
          {
            Overkiz::Iterator::Exception::OutOfRange e;
            throw e;
          }
        }
        else
        {
          Overkiz::Iterator::Exception::Empty e;
          throw e;
        }
      }

      /**
       * @see hasNext
       *
       */
      bool hasNext()
      {
        if(_container)
        {
          return (_iterator != _container->end());
        }
        else
        {
          Overkiz::Iterator::Exception::Empty e;
          throw e;
        }
      }

    protected:

      C<T, A> * _container;
      It _iterator;

    };

  }

}

#endif /* OVERKIZ_CORE_ITERATOR_H_ */
