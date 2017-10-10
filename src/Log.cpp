/*
 * Log.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <stdlib.h>

#include "Log.h"

#define LOG_ENVNAME_LEVEL             "OVK_LOG_LVL"
#define LOG_ENVNAME_TIME              "OVK_LOG_TIME"

namespace Overkiz
{

  namespace Log
  {

    Overkiz::Shared::Pointer<Overkiz::Log::Logger> & Logger::get()
    {
      if(logger->empty())
      {
        logger = Shared::Pointer<Overkiz::Log::Logger>::create();
      }

      return *logger;
    }

    Logger::Logger() :
      _facility(LOG_DAEMON), _printLevel(Priority::OVK_ERROR), _options(0), timed(OVK_TIME_UNKNOWN)
    {
      openlog(NULL, _options, _facility);
      const char * envl = getenv(LOG_ENVNAME_LEVEL);
      const char * envt = getenv(LOG_ENVNAME_TIME);

      if(envl != NULL)
      {
        Overkiz::Log::Priority prio = Overkiz::Log::Logger::getPriority(envl);

        if(prio != Overkiz::Log::Priority::OVK_UNKNOWN_PRIORITY)
          _printLevel = prio;
      }

      if(envt != NULL)
      {
        timed = Overkiz::Log::Logger::getTimed(envt);
        gettimeofday(&tv1, NULL);
      }
      else
      {
        timerclear(&tv1);
      }
    }

    Logger::~Logger()
    {
      closelog();
    }

    Overkiz::Log::Priority Logger::getPriority(const std::string & priority)
    {
      unsigned int pri;
      int ret = sscanf(priority.c_str(), "%8u", &pri);

      if(ret == 1)
      {
        if(pri < OVK_PRIORITY_COUNT)
        {
          return (Overkiz::Log::Priority) pri;
        }
        else
        {
          return OVK_UNKNOWN_PRIORITY;
        }
      }

      for(int i = 0; i < OVK_PRIORITY_COUNT; i++)
      {
        if(priority.compare(PRIORITY_STRING[i]) == 0)
        {
          return (Overkiz::Log::Priority) i;
        }
        else if(priority.compare(PRIORITY_STRING_UC[i]) == 0)
        {
          return (Overkiz::Log::Priority) i;
        }
        else if(priority.compare(PRIORITY_STRING_LC[i]) == 0)
        {
          return (Overkiz::Log::Priority) i;
        }
      }

      return OVK_UNKNOWN_PRIORITY;
    }

    Overkiz::Log::TimedMsg Logger::getTimed(const std::string & timed)
    {
      int level = std::stoi(timed);

      if(level > 0 && level < OVK_TIME_COUNT)
      {
        return (Overkiz::Log::TimedMsg) level;
      }

      return OVK_TIME_UNKNOWN;
    }

    void Logger::updateIdent(const std::string & ident)
    {
      closelog();
      _temporaryIdent = ident;
      openlog(_temporaryIdent.c_str(), _options, _facility);
    }

    void Logger::defaultIdent()
    {
      closelog();
      _temporaryIdent.clear();
      openlog(NULL, _options, _facility);
    }

    void Logger::print(const Overkiz::Log::Priority priority, const char * format, ...)
    {
      if(!_temporaryIdent.empty())
      {
        defaultIdent();
      }

      va_list arguments;
      va_start(arguments, format);
      vsyslog(LOG_MAKEPRI(_facility, priority), format, arguments);
      va_end(arguments);

      if(priority <= _printLevel)
      {
        va_start(arguments, format);
        consoleOutput(priority, format, arguments);
        va_end(arguments);
      }
    }

    void Logger::vprint(const Overkiz::Log::Priority priority, const char * format,
                        va_list arguments)
    {
      if(!_temporaryIdent.empty())
      {
        defaultIdent();
      }

      va_list args2;
      va_copy(args2, arguments);
      vsyslog(LOG_MAKEPRI(_facility, priority), format, args2);
      va_end(args2);

      if(priority <= _printLevel)
      {
        consoleOutput(priority, format, arguments);
      }
    }

    void Logger::print(const std::string & ident, const Overkiz::Log::Priority priority,  const char * format, ...)
    {
      if(_temporaryIdent != ident)
      {
        updateIdent(ident);
      }

      va_list arguments;
      va_start(arguments, format);
      vsyslog(LOG_MAKEPRI(_facility, priority), format, arguments);
      va_end(arguments);

      if(priority <= _printLevel)
      {
        va_start(arguments, format);
        consoleOutput(priority, format, arguments);
        va_end(arguments);
      }
    }

    void Logger::setFacility(const Overkiz::Log::Priority facility)
    {
      _facility = facility;
    }

    void Logger::setPrintLevel(const Overkiz::Log::Priority priority)
    {
      _printLevel = priority;
    }

    Overkiz::Log::Priority Logger::getPrintLevel() const
    {
      return _printLevel;
    }

    void Logger::consoleOutput(const Overkiz::Log::Priority priority, const char * format,
                               va_list arguments)
    {
      if(timed != OVK_TIME_UNKNOWN)
      {
        struct timeval tv2, res;
        gettimeofday(&tv2, NULL);
        timersub(&tv2, &tv1, &res);

        if(timed == OVK_TIME_BETWEEN_MESSAGE)
        {
          gettimeofday(&tv1, NULL);
        }

        FILE * fs = priority < OVK_NOTICE ? stderr : stdout;
        fprintf(fs,"%08lu.%03lu ", res.tv_sec*1000 + res.tv_usec/1000 , res.tv_usec%1000);
      }

      switch(priority)
      {
        case OVK_ALERT:
        case OVK_EMERGENCY:
        case OVK_CRITICAL:
        case OVK_ERROR:
          fprintf(stderr, LOG_FORMAT_BOLD LOG_FORMAT_RED);
          vfprintf(stderr, format, arguments);
          fprintf(stderr, LOG_FORMAT_RESET_ALL"\n");
          break;

        case OVK_WARNING:
          fprintf(stderr, LOG_FORMAT_YELLOW);
          vfprintf(stderr, format, arguments);
          fprintf(stderr, LOG_FORMAT_RESET_ALL"\n");
          break;

        case OVK_NOTICE:
          fprintf(stdout, LOG_FORMAT_GREEN);
          vfprintf(stdout, format, arguments);
          fprintf(stdout, LOG_FORMAT_RESET_ALL"\n");
          break;

        case OVK_INFO:
          vfprintf(stdout, format, arguments);
          fprintf(stdout, "\n");
          break;

        case OVK_DEBUG:
          fprintf(stdout, LOG_FORMAT_BLUE);
          vfprintf(stdout, format, arguments);
          fprintf(stdout, LOG_FORMAT_RESET_ALL"\n");
          break;

        default:
          break;
      }
    }

    Thread::Key<Logger> Logger::logger;

  }

}
