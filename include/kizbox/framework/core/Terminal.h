/*
 * Terminal.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_TERMINAL_H_
#define OVERKIZ_TERMINAL_H_

#include <string>
#include <termios.h>
#include <fcntl.h>

#include <kizbox/framework/core/Stream.h>
#include <kizbox/framework/core/Watcher.h>
#include <kizbox/framework/core/Exception.h>

namespace Overkiz
{

  namespace Terminal
  {

    namespace Exception
    {

      class Open: public Overkiz::Exception
      {
      public:

        Open(int errorId);
        virtual ~Open()
        {
        }
        const char *getId() const;

      private:

        int errorId;
      };

      class Close: public Overkiz::Exception
      {
      public:

        virtual ~Close()
        {
        }
        const char *getId() const;
      };

      class Configuration: public Overkiz::Exception
      {
      public:

        virtual ~Configuration()
        {
        }
        const char *getId() const;
      };

      class Read: public Overkiz::Exception
      {
      public:

        Read(int errorId);
        virtual ~Read()
        {
        }
        const char *getId() const;

      private:

        int errorId;
      };

      class Write: public Overkiz::Exception
      {
      public:

        Write(int errorId);
        virtual ~Write()
        {
        }
        const char *getId() const;

      private:

        int errorId;
      };

    }

    class Base: public virtual Stream::Base, public Watcher
    {
    public:

      /**
       * Constructor
       */
      Base();

      /**
       * Destructor
       */
      virtual ~Base() = 0;

      /**
       * Function used to set the status the Terminal has to wait for
       * @param status  the status the Terminal has to wait for
       * @return  the current status of the Terminal
       */
      virtual int wait(int status);

      /**
       * Method used to open the Terminal
       */
      void open() throw(Exception::Open);

      /**
       * Method used to close the Terminal
       */
      void close() throw(Exception::Close);

      /**
       * Function used to get the current status of the Terminal
       * @return  the current status of the Terminal
       */
      int getStatus() const;

      /**
       * Function used to get the status the Terminal is waiting for
       * @return  the status the Terminal is waiting for
       */
      int getWaitStatus() const;

      /**
       * Method used to set a Stream::Delegate to the Terminal
       *
       * @param newDelagate : the Delegate to attach.
       */
      void setDelegate(Stream::Delegate * newDelagate);

      /**
       * Method used to set the O_NOCTTY flags on the Terminal's configuration
       * Should be used before calling "open" or won't be applied
       */
      void setNoCTtyFlag();

      /**
       * Method used to set the control mode flags on the Terminal's configuration
       * Should be used after calling "open"
       */
      void setControlMode(tcflag_t c_cflag) throw(Exception::Configuration);

      /**
       * Method used to set the local mode flags on the Terminal's configuration
       * Should be used after calling "open"
       */
      void setLocalMode(tcflag_t c_lflag) throw(Exception::Configuration);

      /**
       * Method used to set the line flags on the Terminal's configuration
       * Should be used after calling "open"
       */
      void setLine(cc_t c_line) throw(Exception::Configuration);

      /**
       * Method used to set the control characters on the Terminal's configuration
       * Should be used after calling "open"
       */
      void setControlCharacters(int index, char value)
      throw(Exception::Configuration);

    protected:

      /**
       * Function used to apply the configuration set with other methods
       * Should be used after calling "open"
       * @param when  defines when the configuration has to be applied
       */
      void applyConfiguration(int when = TCSANOW) throw(Exception::Configuration);

      /**
       * pathname of the Terminal
       */
      std::string path;

      /**
       * access mode of the Terminal
       */
      int flags;

      /**
       * configuration of the Terminal
       */
      struct termios config;

      /**
       * delegate of the Terminal
       */
      Stream::Delegate * delegate;

      /**
       * current status of the Terminal
       */
      int currentStatus;

      /**
       * status the Terminal is waiting for
       */
      int waitStatus;
    };

    class Input: public virtual Base, public Stream::Input
    {
    public:

      /**
       * Constructor
       * @param path  pathname of the Terminal
       */
      Input(const std::string & path);

      /**
       * Destructor
       */
      virtual ~Input();

      /**
       * Function used to set the status the Terminal has to wait for
       * @param status  the status the Terminal has to wait for
       * @return  the current status of the Terminal
       */
      virtual int wait(int status);

      /**
       * Method called when one or more events have been received.
       * If EPOLLERR or EPOLLHUP has been received, the Terminal will be closed
       * @param events  the new events.
       */
      virtual void process(uint32_t evts);

      /**
       * Function used to read data from the Terminal.
       *
       * @param data  buffer to store data.
       * @param size  size of the buffer.
       * @return the size of read data
       */
      size_t read(void *data, size_t size) throw(Exception::Read);

      /**
       * Method used to set the input mode flags on the Terminal's configuration
       * Should be used after calling "open"
       */
      void setInputMode(tcflag_t c_iflag) throw(Exception::Configuration);

      /**
       * Method used to set the input speed on the Terminal's configuration
       * Should be used after calling "open"
       */
      void setInputSpeed(speed_t c_ispeed) throw(Exception::Configuration);
    };

    class Output: public virtual Base, public Stream::Output
    {
    public:

      /**
       * Constructor
       * @param path  pathname of the Terminal
       */
      Output(const std::string & path);

      /**
       * Destructor
       */
      virtual ~Output();

      /**
       * Function used to set the status the Terminal has to wait for
       * @param status  the status the Terminal has to wait for
       * @return  the current status of the Terminal
       */
      virtual int wait(int status);

      /**
       * Method called when one or more events have been received.
       * If EPOLLERR or EPOLLHUP has been received, the Terminal will be closed
       * @param events  the new events.
       */
      virtual void process(uint32_t evts);

      /**
       * Function used to write data to the Terminal.
       *
       * @param data  buffer of data to write
       * @param size  size of the buffer.
       * @return the size of written data
       */
      size_t write(const void *data, size_t size, bool more = false)
      throw(Exception::Write);

      /**
       * Method used to set the output mode flags on the Terminal's configuration
       * Should be used after calling "open"
       */
      void setOutputMode(tcflag_t c_oflag) throw(Exception::Configuration);

      /**
       * Method used to set the output speed on the Terminal's configuration
       * Should be used after calling "open"
       */
      void setOutputSpeed(speed_t c_ospeed) throw(Exception::Configuration);
    };

    class InputOutput: public Input, public Output
    {
    public:
      /**
       * Constructor
       * @param path  pathname of the Terminal
       */
      InputOutput(const std::string & path);

      /**
       * Destructor
       */
      virtual ~InputOutput();

      /**
       * Function used to set the status the Terminal has to wait for
       * @param status  the status the Terminal has to wait for
       * @return  the current status of the Terminal
       */
      virtual int wait(int status);

      /**
       * Method called when one or more events have been received.
       * If EPOLLERR or EPOLLHUP has been received, the Terminal will be closed
       * @param events  the new events.
       */
      virtual void process(uint32_t evts);
    };

  }

}

#endif /* OVERKIZ_TERMINAL_H_ */
