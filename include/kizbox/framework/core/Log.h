/*
 * Log.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_LOG_H_
#define OVERKIZ_LOG_H_

#include <syslog.h>
#include <stdarg.h>
#include <string>
#include <string.h>
#include <sys/time.h>

#include <kizbox/framework/core/Shared.h>
#include <kizbox/framework/core/Thread.h>

/* This is the log API
 *
 * All this stuff send string to syslog.
 * On Kizos, all syslog messages may be transmitted to the "cloud" anytime, anywere ... (all levels and facilities)
 * In order to do that, all apps should send theirs messages continuously.
 * It use CPU and MEMORY, so be careful.
 */

/* Macros to configure facility, default is DAEMON */
#define LOG_SET_FACILITY(f)           Overkiz::Log::Logger::get()->setFacility(f)

#define LOG_SET_KERN        Overkiz::Log::Logger::get()->setFacility(LOG_KERN)
#define LOG_SET_USER        Overkiz::Log::Logger::get()->setFacility(LOG_USER)
#define LOG_SET_MAIL        Overkiz::Log::Logger::get()->setFacility(LOG_MAIL)
#define LOG_SET_DAEMON      Overkiz::Log::Logger::get()->setFacility(LOG_DAEMON)
#define LOG_SET_AUTH        Overkiz::Log::Logger::get()->setFacility(LOG_AUTH)
#define LOG_SET_SYSLOG      Overkiz::Log::Logger::get()->setFacility(LOG_SYSLOG)
#define LOG_SET_LPR         Overkiz::Log::Logger::get()->setFacility(LOG_LPR)
#define LOG_SET_NEWS        Overkiz::Log::Logger::get()->setFacility(LOG_NEWS)
#define LOG_SET_UUCP        Overkiz::Log::Logger::get()->setFacility(LOG_UUCP)
#define LOG_SET_CRON        Overkiz::Log::Logger::get()->setFacility(LOG_CRON)
#define LOG_SET_AUTHPRIV    Overkiz::Log::Logger::get()->setFacility(LOG_AUTHPRIV)
#define LOG_SET_FTP         Overkiz::Log::Logger::get()->setFacility(LOG_FTP)

/* Macros to configure stdout/stderr screen print, default is ERROR*/
#define LOG_ENABLE_FOREGROUND(level)  Overkiz::Log::Logger::get()->setPrintLevel(level)
#define LOG_GETCURRENT_FOREGROUND     Overkiz::Log::Logger::get()->getPrintLevel()

/* Macros to sending log */
/* These are syslog default level, all levels use MEM and CPU for logging in production builds. use sparingly. */
#define OVK_EMERGENCY(...)     Overkiz::Log::Logger::get()->print(Overkiz::Log::Priority::OVK_EMERGENCY, __VA_ARGS__)
#define OVK_ALERT(...)         Overkiz::Log::Logger::get()->print(Overkiz::Log::Priority::OVK_ALERT, __VA_ARGS__)
#define OVK_CRITICAL(...)      Overkiz::Log::Logger::get()->print(Overkiz::Log::Priority::OVK_CRITICAL, __VA_ARGS__)
#define OVK_ERROR(...)         Overkiz::Log::Logger::get()->print(Overkiz::Log::Priority::OVK_ERROR, __VA_ARGS__)
#define OVK_WARNING(...)       Overkiz::Log::Logger::get()->print(Overkiz::Log::Priority::OVK_WARNING, __VA_ARGS__)
#define OVK_NOTICE(...)        Overkiz::Log::Logger::get()->print(Overkiz::Log::Priority::OVK_NOTICE, __VA_ARGS__)
#define OVK_INFO(...)          Overkiz::Log::Logger::get()->print(Overkiz::Log::Priority::OVK_INFO, __VA_ARGS__)
#define OVK_DEBUG(...)         Overkiz::Log::Logger::get()->print(Overkiz::Log::Priority::OVK_DEBUG, __VA_ARGS__)

/* Macro for debuging purposes, this will not be include in production builds */
#ifdef HAVE_TRACE
  #define OVK_TRACE(...)               (fprintf(stderr, __VA_ARGS__))
  #define OVK_LTRACE(...)              (fprintf(stderr, "%s:%i: ", __FUNCTION__, __LINE__) + fprintf(stderr, __VA_ARGS__))
#else
  #define OVK_TRACE(...)               while(0)
  #define OVK_LTRACE(...)              while(0)
#endif

/* Utilities */
#define LOG_GET_PRIORITY(string)    Overkiz::Log::Logger::get()->getPriority(string)
#define LOG_GET_REF                 &(*Overkiz::Log::Logger::get())

/* Colors */
#define LOG_FORMAT_RESET_ALL "\e[0m"
#define LOG_FORMAT_BOLD      "\e[1m"
#define LOG_FORMAT_RED       "\e[31m"
#define LOG_FORMAT_GREEN     "\e[32m"
#define LOG_FORMAT_YELLOW    "\e[33m"
#define LOG_FORMAT_BLUE      "\e[34m"
#define LOG_FORMAT_MAGENTA   "\e[45m"
#define LOG_FORMAT_LIGHTCYAN "\e[96m"
#define LOG_FORMAT_LIGHTGRAY "\e[37m"

#define LOG_MAX_MESSAGE_SIZE 512

namespace Overkiz
{
  namespace Log
  {
    enum Priority
    {
      OVK_UNKNOWN_PRIORITY = -1, //!< Unknown priority (-1)
      OVK_EMERGENCY = LOG_EMERG, //!< System is unusable (0)
      OVK_ALERT = LOG_ALERT, //!< Action must be taken immediately (1)
      OVK_CRITICAL = LOG_CRIT, //!< Critical conditions (2)
      OVK_ERROR = LOG_ERR, //!< Error conditions (3)
      OVK_WARNING = LOG_WARNING, //!< Warning conditions (4)
      OVK_NOTICE = LOG_NOTICE, //!< Normal, but significant, condition (5)
      OVK_INFO = LOG_INFO, //!< Informational message (6)
      OVK_DEBUG = LOG_DEBUG, //!< Debug-level message (7)
      OVK_SILENT, //!< No message (8)
      OVK_PRIORITY_COUNT, //!<
    };


    enum TimedMsg
    {
      OVK_TIME_UNKNOWN = 0,
      OVK_TIME_FROM_START,
      OVK_TIME_BETWEEN_MESSAGE,
      OVK_TIME_COUNT,
    };

    static const std::string PRIORITY_STRING_UC[] = { "EMERGENCY", "ALERT",
                                                      "CRITICAL", "ERROR", "WARNING", "NOTICE", "INFO", "DEBUG", "SILENT",
                                                      "UNKNOWN"
                                                    }; //!< The priority strings in upper case

    static const std::string PRIORITY_STRING[] = { "Emergency", "Alert", "Critical",
                                                   "Error", "Warning", "Notice", "Info", "Debug", "Silent", "Unknown"
                                                 }; //!< The priority strings

    static const std::string PRIORITY_STRING_LC[] = { "emergency", "alert",
                                                      "critical", "error", "warning", "notice", "info", "debug", "silent",
                                                      "unknown"
                                                    }; //!< The priority strings in lower case

    class Logger
    {
    public:

      static Overkiz::Shared::Pointer<Overkiz::Log::Logger> & get();

      Logger();

      virtual ~Logger();

      static Overkiz::Log::Priority getPriority(const std::string & priority);

      static Overkiz::Log::TimedMsg getTimed(const std::string & timed);

      void updateIdent(const std::string & ident);

      void defaultIdent();

      #ifdef __GNUC__
      __attribute__((format(printf, 3, 4)))
      #endif
      void print(const Overkiz::Log::Priority priority, const char * format, ...);

      void setFacility(const Overkiz::Log::Priority facility);

      void setPrintLevel(const Overkiz::Log::Priority priority);

      Overkiz::Log::Priority getPrintLevel() const;

      #ifdef __GNUC__
      __attribute__((format(printf, 3, 0)))
      #endif
      void vprint(const Overkiz::Log::Priority priority, const char * format,
                  va_list arguments);

    private:

      unsigned char _facility;
      Overkiz::Log::Priority _printLevel;
      Overkiz::Log::TimedMsg timed;
      int _options;

      std::string _temporaryIdent;
      struct timeval tv1;
      static Thread::Key<Logger> logger;

    };

  }

}

#endif /* OVERKIZ_LOG_H_ */
