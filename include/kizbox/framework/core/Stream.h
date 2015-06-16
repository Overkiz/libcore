/*
 * Stream.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_STREAM_H_
#define OVERKIZ_STREAM_H_

#include <assert.h>
#include <string.h>
#include <sys/types.h>

namespace Overkiz
{

  namespace Stream
  {

    /**
     * Stream Status enum
     */
    typedef enum
    {
      OPENED = 0x01,  //!< OPENED
      INPUT_READY = 0x02, //!< INPUT_READY
      OUTPUT_READY = 0x04,  //!< OUTPUT_READY
      ERROR = 0x08, //!< ERROR
    } Status;

    /**
     * Interface used to define the a Stream::Delegate.
     */
    class Delegate
    {
    public:

      Delegate()
      {
      }

      /**
       * Empty virtual destructor
       *
       */
      virtual ~Delegate()
      {
      }

      /**
       * Method called by a Stream::Device when status change.
       *
       * @param status
       */
      virtual void ready(int status) = 0;

    };

    /**
     * Interface used to define a Stream
     */
    class Base
    {
    public:

      /**
       * Empty virtual destructor
       *
       */
      virtual ~Base()
      {
      }

      /**
       * Method used to open a Stream.
       * This methods is asynchronous : the Stream is not necessarily opened when it returns.
       * The device must call the Delegate::opened method when Stream is opened.
       */
      virtual void open() = 0;

      /**
       * Method used to close a Stream.
       * This methods is asynchronous : the Stream is not necessarily closed when it returns.
       * The device must call the Delegate::closed method when Stream is closed.
       */
      virtual void close() = 0;

      /**
       * Method used to get the current Stream status.
       *
       * @return the current status.
       */
      virtual int getStatus() const = 0;

      /**
       * Method used to get the waiting Stream status.
       *
       * @return the waiting status
       */
      virtual int getWaitStatus() const = 0;

      /**
       * Method used to set a Stream::Delegate to the Stream::Device
       *
       * @param newDelegate : the Delegate to attach.
       */
      virtual void setDelegate(Delegate *newDelegate) = 0;

      /**
       * Method used to inform the Stream::Device that we wait for some
       * status change.
       *
       * @param status : the status change we're waiting for.
       * @return the current status.
       */
      virtual int wait(int status) = 0;

    };

    /**
     * Interface used to define an Input Stream
     */
    class Input: public virtual Base
    {
    public:

      /**
       * Empty virtual destructor
       *
       */
      virtual ~Input()
      {
      }

      /**
       * Method used to read Data from the Stream.
       *
       * @param data : buffer to store data.
       * @param size : size of the buffer.
       * @return the read size.
       */
      virtual size_t read(void *data, size_t size) = 0;

    };

    /**
     * Interface used to define a Peekable Input Stream
     */
    class PeekableInput: public virtual Input
    {
    public:

      /**
       * Empty virtual destructor
       *
       */
      virtual ~PeekableInput()
      {
      }

      /**
       * Method used to peek Data from the Stream.
       *
       * @param data : buffer to store data.
       * @param size : size of the buffer.
       * @return the peeked size.
       */
      virtual size_t peek(void *data, size_t size) = 0;

    };

    /**
     * Interface used to define an Output Stream
     */
    class Output: public virtual Base
    {
    public:

      /**
       * Empty virtual destructor
       *
       */
      virtual ~Output()
      {
      }

      /**
       * Method used to write Data to the Stream.
       *
       * @param data : the buffer containing Data to write.
       * @param size : the size of the buffer.
       * @param more : set to true if more data need to be written else leave to false.
       * @return the written size
       */
      virtual size_t write(const void *data, size_t size, bool more = false) = 0;

    };

    /**
     * Interface used to define an Input/Output Stream
     */
    class InputOutput: public virtual Input, public virtual Output
    {

    };

    namespace Wrapper
    {

      /**
       * class used to define a Stream Wrapper.
       */
      class Base
      {
      public:

        /**
         * Empty virtual destructor
         *
         */
        virtual ~Base()
        {
        }

        /**
         * Method used to reset the Wrapper context.
         */
        virtual void reset() = 0;

      };

      /**
       * Template used to define an Input Stream Wrapper.
       */
      template<typename T>
      class Input: public virtual Base
      {
      public:

        /**
         * Empty virtual destructor
         *
         */
        virtual ~Input()
        {
        }

        /**
         * Method used to convert input stream data into T object.
         *
         * @param input : the input stream.
         * @param obj : the object to fill.
         * @return true if the object has been fully filled else return false.
         */
        virtual bool read(Overkiz::Stream::Input *input, T& obj) = 0;

        /**
         * Method used to convert peekable input stream data into T object.
         *
         * @param input : the peekable input stream.
         * @param obj : the object to fill.
         * @return true if the object has been fully filled else return false.
         */
        virtual bool read(Overkiz::Stream::PeekableInput *input, T& obj)
        {
          Overkiz::Stream::Input *in = input;
          return read(in, obj);
        }

      };

      /**
       * Template used to define an Output Stream Wrapper.
       */
      template<typename T>
      class Output: public virtual Base
      {
      public:

        /**
         * Empty virtual destructor
         *
         */
        virtual ~Output()
        {
        }

        /**
         * Method used to convert T object into output stream data.
         *
         * @param output
         * @param object
         * @return
         */
        virtual bool write(Overkiz::Stream::Output *output, const T& object) = 0;

      };

    }

  }

}

#endif /* OVERKIZ_STREAM_H_ */
