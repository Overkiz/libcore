/*
 * Buffer.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_STREAM_BUFFER_H_
#define OVERKIZ_STREAM_BUFFER_H_

#include <assert.h>
#include <string.h>
#include <sys/types.h>

#include <kizbox/framework/core/Stream.h>

#ifndef MIN
  #define MIN(a, b) ((a) < (b)) ? (a) : (b)
#endif

#ifndef MAX
  #define MAX(a, b) ((a) > (b)) ? (a) : (b)
#endif

namespace Overkiz
{

  namespace Stream
  {

    namespace Buffer
    {

      /**
       * Interface used to define a Buffer Stream
       */
      class Base: public virtual Stream::Base
      {
      public:

        /**
         * Constructror.
         */
        Base() :
          status(0)
        {
        }

        /**
         * Destructor.
         */
        virtual ~Base()
        {
        }

        /**
         * Method used to reset the buffered Stream.
         */
        virtual void reset() = 0;

        /**
         * Method used to open a Stream.
         * This methods is asynchronous : the Stream is not necessarily opened when it returns.
         * The device must call the Delegate::opened method when Stream is opened.
         */
        virtual void open()
        {
          if((status & Overkiz::Stream::OPENED) == 0)
          {
            status |= Overkiz::Stream::OPENED;
          }
        }

        /**
         * Method used to close a Stream.
         * This methods is asynchronous : the Stream is not necessarily closed when it returns.
         * The device must call the Delegate::closed method when Stream is closed.
         */
        virtual void close()
        {
          if(status & Overkiz::Stream::OPENED)
          {
            status = 0;
          }
        }

        /**
         * Method used to get the current Stream status.
         *
         * @return the current status.
         */
        virtual int getStatus() const
        {
          return status;
        }

        /**
         * Method used to get the waiting Stream status.
         *
         * @return the waiting status
         */
        virtual int getWaitStatus() const
        {
          return 0;
        }

        /**
         * Method used to set a Stream::Delegate to the Stream::Device
         *
         * @param newDelegate : the Delegate to attach.
         */
        virtual void setDelegate(Delegate * newDelegate)
        {
        }

        /**
         * Method used to inform the Stream::Device that we wait for some
         * status change.
         *
         * @param status : the status change we're waiting for.
         * @return the current status.
         */
        virtual int wait(int status)
        {
          return this->status;
        }

      protected:

        int status;

      };

      /**
       * Interface used to define a Peekable Input Stream
       */
      class Input: public Buffer::Base, public virtual Stream::PeekableInput
      {
      public:

        /**
         * Constructror.
         */
        Input() :
          Buffer::Base(), inputData(NULL), inputIndex(0), inputSize(0)
        {
        }

        /**
         * Constructror.
         */
        Input(const char * inputData, size_t inputSize) :
          Buffer::Base(), inputData(inputData), inputIndex(0), inputSize(
            inputSize)
        {
        }

        /**
         * Destructor.
         */
        virtual ~Input()
        {
        }

        /**
         * Method used to reset a the Stream.
         */
        virtual void reset()
        {
          if(inputData)
            inputIndex = 0;
        }

        /**
         * setInputData.
         *
         */
        virtual void setInputData(const char * data, size_t size)
        {
          if(size <= 0)
          {
            throw "Invalid data size";
          }

          inputData = data;
          inputIndex = 0;
          inputSize = size;

          if((status & Overkiz::Stream::OPENED) && inputData
             && (inputSize > 0))
          {
            status |= Overkiz::Stream::INPUT_READY;
          }
          else
          {
            status &= ~Overkiz::Stream::INPUT_READY;
          }
        }

        /**
         * Method used to open a Stream.
         * This methods is asynchronous : the Stream is not necessarily opened when it returns.
         * The device must call the Delegate::opened method when Stream is opened.
         */
        virtual void open()
        {
          if((status & Overkiz::Stream::OPENED) == 0)
          {
            status |= Overkiz::Stream::OPENED;

            if(inputData && (inputIndex < inputSize))
            {
              status |= Overkiz::Stream::INPUT_READY;
            }
          }
        }

        /**
         * Method used to read Data from the Stream.
         *
         * @param data : buffer to store data.
         * @param size : size of the buffer.
         * @return the read size.
         */
        virtual size_t read(void * data, size_t size)
        {
          size_t read = peek(data, size);
          inputIndex += read;

          // The stream stops to be available in input when the index buffer reach the end
          if(inputIndex >= inputSize)
          {
            // The input buffer index is not supposed to be greater than the input buffer size
            assert(inputIndex == inputSize);
            status &= ~Overkiz::Stream::INPUT_READY;
          }

          return read;
        }

        /**
         * Method used to peek Data from the Stream.
         *
         * @param data : buffer to store data.
         * @param size : size of the buffer.
         * @return the peeked size.
         */
        virtual size_t peek(void * data, size_t size)
        {
          assert(status & Overkiz::Stream::INPUT_READY);
          assert(data);
          assert(inputSize);
          assert(inputData);
          size_t read = MIN(size, (inputSize - inputIndex));

          if(read > 0)
          {
            memcpy(data,
                   (const void *)(size_t (inputData) + size_t (inputIndex)),
                   read);
          }

          return read;
        }

        /**
         * Method used to get the size.
         *
         * @return the size
         */
        size_t getSize() const
        {
          return inputIndex;
        }

      protected:

        const char * inputData;
        size_t inputIndex;
        size_t inputSize;

      };

      /**
       * Interface used to define an Output Stream
       */
      class Output: public Buffer::Base, public virtual Stream::Output
      {
      public:

        /**
         * Constructror.
         */
        Output() :
          Buffer::Base(), outputData(NULL), outputIndex(0), outputSize(0)
        {
        }

        /**
         * Constructror.
         */
        Output(char * outputData, size_t outputSize) :
          Buffer::Base(), outputData(outputData), outputIndex(0), outputSize(
            outputSize)
        {
        }

        /**
         * Destructor.
         */
        virtual ~Output()
        {
        }

        /**
         * Method used to reset a the Stream.
         */
        virtual void reset()
        {
          if(outputData)
            outputIndex = 0;
        }

        /**
         * setOutputData.
         *
         */
        virtual void setOutputData(char* data, int size)
        {
          if(size <= 0)
          {
            throw "Invalid data size";
          }

          outputData = data;
          outputIndex = 0;
          outputSize = size;

          if((status & Overkiz::Stream::OPENED) && outputData
             && (outputSize > 0))
          {
            status |= Overkiz::Stream::OUTPUT_READY;
          }
          else
          {
            status &= ~Overkiz::Stream::OUTPUT_READY;
          }
        }

        /**
         * Method used to open a Stream.
         * This methods is asynchronous : the Stream is not necessarily opened when it returns.
         * The device must call the Delegate::opened method when Stream is opened.
         */
        virtual void open()
        {
          if((status & Overkiz::Stream::OPENED) == 0)
          {
            status |= Overkiz::Stream::OPENED;

            if(outputData && (outputIndex < outputSize))
            {
              status |= Overkiz::Stream::OUTPUT_READY;
            }
          }
        }

        /**
         * Method used to write Data to the Stream.
         *
         * @param data : the buffer containing Data to write.
         * @param size : the size of the buffer.
         * @param more : set to true if more data need to be written else leave to false.
         * @return the written size
         */
        virtual size_t write(const void * data, size_t size, bool more = false)
        {
          assert(status & Overkiz::Stream::OUTPUT_READY);
          assert(data);
          assert(outputSize);
          assert(outputData);
          size_t wrote = MIN(size, (outputSize - outputIndex));

          if(wrote > 0)
          {
            memcpy((void *)(size_t (outputData) + size_t (outputIndex)), data,
                   wrote);
            outputIndex += wrote;
          }

          // The stream stops to be available in output when the index buffer reach the end
          if(outputIndex >= outputSize)
          {
            // The output buffer index is not supposed to be greater than the output buffer size
            assert(outputIndex == outputSize);
            status &= ~Overkiz::Stream::OUTPUT_READY;
          }

          return wrote;
        }

        /**
         * Method used to get the size.
         *
         * @return the size
         */
        size_t getSize() const
        {
          return outputIndex;
        }

        /**
         * Method used to get the available size.
         *
         * @return the available size
         */
        size_t getAvailableSize() const
        {
          return outputSize - outputIndex;
        }

      protected:

        char * outputData;
        size_t outputIndex;
        size_t outputSize;

      };

    }

    namespace AllocatedBuffer
    {

      /**
       * Class used to define an Input Allocated Buffer
       */
      class Input: public Buffer::Input
      {
      public:

        /**
         * Constructor.
         */
        Input() :
          Buffer::Input()
        {
        }

        /**
         * Constructor.
         */
        Input(int size) :
          Buffer::Input(new char[size], size)
        {
        }

        /**
         * Destructor.
         */
        virtual ~Input()
        {
          if(inputData)
          {
            delete[] inputData;
            inputData = NULL;
          }
        }

        /**
         * set buffer
         */
        virtual void setInputData(char * buffer, int size)
        {
          if(size < 0)
          {
            throw "Invalid size";
          }

          if(inputData)
          {
            delete[] inputData;
            inputData = NULL;
          }

          inputData = new char[size];
          inputSize = size;
        }

      };

      /**
       * Class used to define an Output Allocated Buffer
       */
      class Output: public Buffer::Output
      {
      public:

        /**
         * Constructor.
         */
        Output() :
          Buffer::Output()
        {
        }

        /**
         * Constructor.
         */
        Output(int size) :
          Buffer::Output(new char[size], size)
        {
        }

        /**
         * Destructor.
         */
        virtual ~Output()
        {
          if(outputData)
          {
            delete[] outputData;
            outputData = NULL;
          }
        }

        /**
         * set buffer
         */
        virtual void setOutputData(char * buffer, int size)
        {
          if(size < 0)
          {
            throw "Invalid size";
          }

          if(outputData)
          {
            delete[] outputData;
            outputData = NULL;
          }

          outputData = new char[size];
          outputSize = size;
        }

      };

    }

  }

}

#endif /* OVERKIZ_STREAM_BUFFER_H_ */
