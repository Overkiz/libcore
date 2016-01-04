/*
 * Log.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <stdlib.h>

#include "Log.h"

#define LOG_ENVNAME_LEVEL             "OVK_LOG_LVL"

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
      _facility(LOG_DAEMON), _printLevel(Priority::OVK_ERROR), _options(0)
    {
      openlog(NULL, _options, _facility);
      const char * env = getenv(LOG_ENVNAME_LEVEL);

      if(env == NULL)
        return;

      Overkiz::Log::Priority prio = Overkiz::Log::Logger::getPriority(env);

      if(prio != Overkiz::Log::Priority::OVK_UNKNOWN_PRIORITY)
        _printLevel = prio;
    }

    Logger::~Logger()
    {
      closelog();
    }

    Overkiz::Log::Priority Logger::getPriority(const std::string & priority)
    {
      unsigned int pri;
      int ret = sscanf(priority.c_str(), "%u", &pri);

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
      vsnprintf(buffer, sizeof(buffer), format, arguments);
      buffer[LOG_MAX_MESSAGE_SIZE - 1] = '\0';
      buffer[LOG_MAX_MESSAGE_SIZE - 2] = '.';
      buffer[LOG_MAX_MESSAGE_SIZE - 3] = '.';
      buffer[LOG_MAX_MESSAGE_SIZE - 4] = '.';

      if(strlen(buffer) > 0)
        syslog(LOG_MAKEPRI(_facility, priority), "%s", buffer);

      if(!(priority <= _printLevel
           && (_printLevel < OVK_SILENT && _printLevel > OVK_UNKNOWN_PRIORITY)))
        return;

      switch(priority)
      {
        case OVK_ALERT:
        case OVK_EMERGENCY:
        case OVK_CRITICAL:
        case OVK_ERROR:
          fprintf(stderr,
                  LOG_FORMAT_BOLD LOG_FORMAT_RED "%s" LOG_FORMAT_RESET_ALL"\n", buffer);
          break;

        case OVK_WARNING:
          fprintf(stderr, LOG_FORMAT_YELLOW "%s" LOG_FORMAT_RESET_ALL"\n",
                  buffer);
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
