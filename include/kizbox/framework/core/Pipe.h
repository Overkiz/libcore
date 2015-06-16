/*
 * Pipe.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_PIPE_H_
#define OVERKIZ_PIPE_H_

#include <string>

#include <kizbox/framework/core/Watcher.h>
#include <kizbox/framework/core/Shared.h>
#include <kizbox/framework/core/Subject.h>
#include <kizbox/framework/core/Exception.h>

namespace Overkiz
{

  namespace Pipe
  {

    class PipeException: public Overkiz::Exception
    {
    public:
      PipeException()
      {
      }

      virtual ~PipeException()
      {
      }

      const char *getId() const
      {
        return ("com.overkiz.Kizbox.Framework.Core.Pipe");
      }
    };

    typedef enum
    {
      INPUT_READY = 0x001,
      OUTPUT_READY = 0x002,
      DISCONNECTED = 0x004,
      ERROR = 0x008,
      OUTPUT_ERROR = 0x020,
    } Event;

    class Listener
    {
    public:
      virtual ~Listener()
      {
      }

      virtual void notified(Overkiz::Subject<Overkiz::Pipe::Listener> * subject,
                            const uint32_t event) = 0;
    };

    namespace Named
    {
      class Base: protected Overkiz::Watcher, public Overkiz::Subject <
        Overkiz::Pipe::Listener >
      {
      public:
        Base();

        virtual ~Base();

        virtual void open(const std::string & path);

        virtual void close();

        void setStackSize(size_t stkSize);

      protected:

        void open(const std::string & path, int openFcntl);

      private:
        static bool fileExists(const std::string & path);

        void process(uint32_t evts);
      };

      class Server: public Base
      {
      public:
        Server();

        virtual ~Server();

        int receive(char * buff, size_t maxLen);

      };

      class Client: public Base
      {
      public:
        Client();

        virtual ~Client();

        void open(const std::string & path);

        int send(const char * buff, size_t len);
      };

    }

  }

}

#endif /* OVERKIZ_PIPE_H_ */
