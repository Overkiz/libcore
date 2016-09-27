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
      _temporaryIdent = ident;
      closelog();
      openlog(_temporaryIdent.c_str(), _options, _facility);
    }

    void Logger::defaultIdent()
    {
      _temporaryIdent.clear();
      closelog();
      openlog(NULL, _options, _facility);
    }

    void Logger::print(const Overkiz::Log::Priority priority, const char * format,
                       ...)
    {
      va_list arguments;
      va_start(arguments, format);
      vprint(priority, format, arguments);
      va_end(arguments);
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

    void Logger::vprint(const Overkiz::Log::Priority priority, const char * format,
                        va_list arguments)
    {
      char buffer[LOG_MAX_MESSAGE_SIZE];
      int nb = vsnprintf(buffer, sizeof(buffer), format, arguments);
      buffer[LOG_MAX_MESSAGE_SIZE - 1] = '\0';
      buffer[LOG_MAX_MESSAGE_SIZE - 2] = '.';
      buffer[LOG_MAX_MESSAGE_SIZE - 3] = '.';
      buffer[LOG_MAX_MESSAGE_SIZE - 4] = '.';

      // no error and data
      if(nb < 1)
        return;

      syslog(LOG_MAKEPRI(_facility, priority), "%s", buffer);

      if(!(priority <= _printLevel
           && (_printLevel < OVK_SILENT && _printLevel > OVK_UNKNOWN_PRIORITY)))
        return;

      if(timed != OVK_TIME_UNKNOWN)
      {
        struct timeval tv2, res;
        gettimeofday(&tv2, NULL);
        timersub(&tv2, &tv1, &res);

        if(timed == OVK_TIME_BETWEEN_MESSAGE)
        {
          gettimeofday(&tv1, NULL);
        }

        FILE * fs = priority < OVK_WARNING ? stderr : stdout;
        fprintf(fs,"%08lu.%03lu ", res.tv_sec*1000 + res.tv_usec/1000 , res.tv_usec%1000);
      }

      switch(priority)
      {
        case OVK_ALERT:
        case OVK_EMERGENCY:
        case OVK_CRITICAL:
        case OVK_ERROR:
          fprintf(stderr, LOG_FORMAT_BOLD LOG_FORMAT_RED "%s" LOG_FORMAT_RESET_ALL"\n", buffer);
          break;

        case OVK_WARNING:
          fprintf(stderr, LOG_FORMAT_YELLOW "%s" LOG_FORMAT_RESET_ALL"\n", buffer);
          break;

        case OVK_NOTICE:
          fprintf(stdout, LOG_FORMAT_GREEN "%s" LOG_FORMAT_RESET_ALL"\n", buffer);
          break;

        case OVK_INFO:
          fprintf(stdout, "%s\n", buffer);
          break;

        case OVK_DEBUG:
          fprintf(stdout, LOG_FORMAT_BLUE "%s" LOG_FORMAT_RESET_ALL"\n", buffer);
          break;

        default:
          break;
      }
    }

    Thread::Key<Logger> Logger::logger;

  }

}
