/*
 * Time.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <cerrno>

#include "Time.h"

#ifndef RTC_SANITY_CHECK_YEAR
  #define RTC_SANITY_CHECK_YEAR 2016
#endif

namespace Overkiz
{
  namespace Time
  {
    /* http://man7.org/linux/man-pages/man2/timerfd_create.2.html#ERRORS -> EINVAL for timerfd_settime */
    const long maxNanoSecondsAllowed = 999999999;
    const Elapsed Elapsed::MAX(LONG_MAX, maxNanoSecondsAllowed);

    Base::Base()
    {
    }

    Base::~Base()
    {
    }

    void Base::normalize()
    {
      /*
       * nsec can't be greater than maxNanoSecondsAllowed -> 999,999,999
       * It means that each time that we have a nano time greater than a second,
       * we increment the sec attributes.
       *
       * If we add two object, the result must not be greater than 999ms.
       * */
      ts.tv_sec += ts.tv_nsec / NANO_TO_SECOND;
      ts.tv_nsec = ts.tv_nsec % NANO_TO_SECOND;
    }

    Elapsed Base::normalize(const Elapsed & elapsed)
    {
      Elapsed e = elapsed;
      e.seconds     += elapsed.seconds / NANO_TO_SECOND;
      e.nanoseconds = elapsed.nanoseconds % NANO_TO_SECOND;
      return e;
    }

    std::string Base::toString(const time_t & seconds, const long & nanoseconds)
    {
      std::string out;

      if(seconds)
      {
        out += std::to_string(seconds) + " s";
      }

      long rest = nanoseconds;
      long ms = rest / NANO_TO_MILLI_SECOND;
      rest %= NANO_TO_MILLI_SECOND;
      long us = rest / NANO_TO_MICRO_SECOND;
      rest %=NANO_TO_MICRO_SECOND;

      if(ms)
      {
        if(!out.empty())
        {
          out += ", ";
        }

        out += std::to_string(ms) + " ms";
      }

      if(us)
      {
        if(!out.empty())
        {
          out += ", ";
        }

        out += std::to_string(us) + " us";
      }

      if(rest)
      {
        if(!out.empty())
        {
          out += ", ";
        }

        out += std::to_string(rest) + " ns";
      }

      return out;
    }

    std::string Base::toString(const timespec & ts)
    {
      return toString(ts.tv_sec, ts.tv_nsec);
    }

    Real::Real()
    {
      clock_gettime(CLOCK_REALTIME, &ts);
    }

    Real::Real(const Real& time)
    {
      *this = time;
      normalize();
    }

    Real::Real(const Elapsed& epoch)
    {
      *this = epoch;
      normalize();
    }

    Real::~Real()
    {
    }

    Real Real::now()
    {
      return Real();
    }

    Real::operator Elapsed() const
    {
      Elapsed epoch;
      epoch.seconds = ts.tv_sec;
      epoch.nanoseconds = ts.tv_nsec;
      return epoch;
    }

    Real& Real::operator = (const Real& time)
    {
      ts = time.ts;
      normalize();
      return *this;
    }

    Real& Real::operator = (const Elapsed& epoch)
    {
      ts.tv_sec = epoch.seconds;
      ts.tv_nsec = epoch.nanoseconds;
      normalize();
      return *this;
    }

    Real Real::operator - (const Real& time) const
    {
      Real newTime(*this);
      Real timeN(time);
      newTime.ts.tv_nsec -= timeN.ts.tv_nsec;

      if(newTime.ts.tv_nsec < 0)
      {
        newTime.ts.tv_nsec += NANO_TO_SECOND;
        newTime.ts.tv_sec--;
      }
      else if(newTime.ts.tv_nsec >= NANO_TO_SECOND)
      {
        newTime.ts.tv_nsec -= NANO_TO_SECOND;
        newTime.ts.tv_sec++;
      }

      newTime.ts.tv_sec -= timeN.ts.tv_sec;

      if(newTime.ts.tv_sec < 0)
      {
        newTime.ts.tv_sec = 0;
      }

      newTime.normalize();
      return newTime;
    }

    Real Real::operator - (const Elapsed& epoch) const
    {
      Real newTime(*this);
      newTime.ts.tv_nsec -= normalize(epoch).nanoseconds;

      if(newTime.ts.tv_nsec < 0)
      {
        newTime.ts.tv_nsec += NANO_TO_SECOND;
        newTime.ts.tv_sec--;
      }
      else if(newTime.ts.tv_nsec >= NANO_TO_SECOND)
      {
        newTime.ts.tv_nsec -= NANO_TO_SECOND;
        newTime.ts.tv_sec++;
      }

      newTime.ts.tv_sec -= normalize(epoch).seconds;

      if(newTime.ts.tv_sec < 0)
      {
        newTime.ts.tv_sec = 0;
      }

      newTime.normalize();
      return newTime;
    }

    Real& Real::operator -= (const Real& time)
    {
      normalize();
      Real timeN(time);
      ts.tv_nsec -= timeN.ts.tv_nsec;

      if(ts.tv_nsec < 0)
      {
        ts.tv_nsec += NANO_TO_SECOND;
        ts.tv_sec--;
      }
      else if(ts.tv_nsec >= NANO_TO_SECOND)
      {
        ts.tv_nsec -= NANO_TO_SECOND;
        ts.tv_sec++;
      }

      ts.tv_sec -= timeN.ts.tv_sec;

      if(ts.tv_sec < 0)
      {
        ts.tv_sec = 0;
      }

      normalize();
      return *this;
    }

    Real& Real::operator -= (const Elapsed& epoch)
    {
      normalize();
      ts.tv_nsec -= normalize(epoch).nanoseconds;

      if(ts.tv_nsec < 0)
      {
        ts.tv_nsec += NANO_TO_SECOND;
        ts.tv_sec--;
      }
      else if(ts.tv_nsec >= NANO_TO_SECOND)
      {
        ts.tv_nsec -= NANO_TO_SECOND;
        ts.tv_sec++;
      }

      ts.tv_sec -= normalize(epoch).seconds;

      if(ts.tv_sec < 0)
      {
        ts.tv_sec = 0;
      }

      normalize();
      return *this;
    }

    Real Real::operator + (const Real& time) const
    {
      Real newTime(*this);
      Real timeN(time);
      newTime.ts.tv_nsec += timeN.ts.tv_nsec;

      if(newTime.ts.tv_nsec < 0)
      {
        newTime.ts.tv_nsec += NANO_TO_SECOND;
        newTime.ts.tv_sec--;
      }
      else if(newTime.ts.tv_nsec >= NANO_TO_SECOND)
      {
        newTime.ts.tv_nsec -= NANO_TO_SECOND;
        newTime.ts.tv_sec++;
      }

      newTime.ts.tv_sec += timeN.ts.tv_sec;
      newTime.normalize();
      return newTime;
    }

    Real Real::operator + (const Elapsed& epoch) const
    {
      Real newTime(*this);
      newTime.ts.tv_nsec += normalize(epoch).nanoseconds;

      if(newTime.ts.tv_nsec < 0)
      {
        newTime.ts.tv_nsec += NANO_TO_SECOND;
        newTime.ts.tv_sec--;
      }
      else if(newTime.ts.tv_nsec >= NANO_TO_SECOND)
      {
        newTime.ts.tv_nsec -= NANO_TO_SECOND;
        newTime.ts.tv_sec++;
      }

      newTime.ts.tv_sec += normalize(epoch).seconds;
      newTime.normalize();
      return newTime;
    }

    Real& Real::operator += (const Real& time)
    {
      normalize();
      Real timeN(time);
      ts.tv_nsec += timeN.ts.tv_nsec;

      if(ts.tv_nsec < 0)
      {
        ts.tv_nsec += NANO_TO_SECOND;
        ts.tv_sec--;
      }
      else if(ts.tv_nsec >= NANO_TO_SECOND)
      {
        ts.tv_nsec -= NANO_TO_SECOND;
        ts.tv_sec++;
      }

      ts.tv_sec += timeN.ts.tv_sec;
      normalize();
      return *this;
    }

    Real& Real::operator += (const Elapsed& epoch)
    {
      normalize();
      ts.tv_nsec += normalize(epoch).nanoseconds;

      if(ts.tv_nsec < 0)
      {
        ts.tv_nsec += NANO_TO_SECOND;
        ts.tv_sec--;
      }
      else if(ts.tv_nsec >= NANO_TO_SECOND)
      {
        ts.tv_nsec -= NANO_TO_SECOND;
        ts.tv_sec++;
      }

      ts.tv_sec += normalize(epoch).seconds;
      normalize();
      return *this;
    }

    bool Real::operator < (const Real& time) const
    {
      if(ts.tv_sec != time.ts.tv_sec)
        return ts.tv_sec < time.ts.tv_sec;

      return ts.tv_nsec < time.ts.tv_nsec;
    }

    bool Real::operator < (const Elapsed& epoch) const
    {
      if(ts.tv_sec != epoch.seconds)
        return ts.tv_sec < epoch.seconds;

      return ts.tv_nsec < epoch.nanoseconds;
    }

    bool Real::operator > (const Real& time) const
    {
      if(ts.tv_sec != time.ts.tv_sec)
        return ts.tv_sec > time.ts.tv_sec;

      return ts.tv_nsec > time.ts.tv_nsec;
    }

    bool Real::operator > (const Elapsed& epoch) const
    {
      if(ts.tv_sec != epoch.seconds)
        return ts.tv_sec > epoch.seconds;

      return ts.tv_nsec > epoch.nanoseconds;
    }

    bool Real::operator <= (const Real& time) const
    {
      if(ts.tv_sec != time.ts.tv_sec)
        return ts.tv_sec <= time.ts.tv_sec;

      return ts.tv_nsec <= time.ts.tv_nsec;
    }

    bool Real::operator <= (const Elapsed& epoch) const
    {
      if(ts.tv_sec != epoch.seconds)
        return ts.tv_sec <= epoch.seconds;

      return ts.tv_nsec <= epoch.nanoseconds;
    }

    bool Real::operator >= (const Real& time) const
    {
      if(ts.tv_sec != time.ts.tv_sec)
        return ts.tv_sec >= time.ts.tv_sec;

      return ts.tv_nsec >= time.ts.tv_nsec;
    }

    bool Real::operator >= (const Elapsed& epoch) const
    {
      if(ts.tv_sec != epoch.seconds)
        return ts.tv_sec >= epoch.seconds;

      return ts.tv_nsec >= epoch.nanoseconds;
    }

    bool Real::operator != (const Real& time) const
    {
      return ts.tv_sec != time.ts.tv_sec || ts.tv_nsec != time.ts.tv_nsec;
    }

    bool Real::operator != (const Elapsed& epoch) const
    {
      return ts.tv_sec != epoch.seconds || ts.tv_nsec != epoch.nanoseconds;
    }

    bool Real::operator == (const Real& time) const
    {
      return !(operator != (time));
    }

    bool Real::operator == (const Elapsed& time) const
    {
      return !(operator != (time));
    }

    bool Real::reliable()
    {
      bool ret = false;
      int rtc = open("/dev/rtc", O_RDONLY);

      if(rtc >= 0)
      {
        #ifdef RTC_VL_READ
        // First check VL flag
        int tmp;

        if(ioctl(rtc, RTC_VL_READ, &tmp) < 0)
        {
          // VL may be not implemented
          if(errno != ENOTTY)
          {
            close(rtc);
            return false;
          }
        }
        else if(tmp)
        {
          // Check VL flag
          close(rtc);
          return false;
        }

        #endif
        struct rtc_time rtc_tm;

        // Then check if time is valid
        if(ioctl(rtc, RTC_RD_TIME, &rtc_tm) >= 0)
        {
          //tm_year = years since 1900 see linux/time.h
          if(rtc_tm.tm_year >= (int)(RTC_SANITY_CHECK_YEAR - 1900))
            ret = true;
        }

        close(rtc);
      }

      return ret;
    }

    std::string Real::toString() const
    {
      return Base::toString(ts);
    }

    Monotonic::Monotonic()
    {
      clock_gettime(CLOCK_MONOTONIC, &ts);
    }

    Monotonic::Monotonic(const Monotonic& time)
    {
      ts = time.ts;
      normalize();
    }

    Monotonic::Monotonic(const Elapsed& time)
    {
      ts.tv_sec = time.seconds;
      ts.tv_nsec = time.nanoseconds;
      normalize();
    }

    Monotonic::~Monotonic()
    {
    }

    Monotonic Monotonic::now()
    {
      Monotonic time;
      clock_gettime(CLOCK_MONOTONIC, &time.ts);
      return time;
    }

    Monotonic::operator Elapsed() const
    {
      Elapsed time;
      time.seconds = ts.tv_sec;
      time.nanoseconds = ts.tv_nsec;
      return time;
    }

    Monotonic& Monotonic::operator = (const Monotonic& time)
    {
      ts = time.ts;
      normalize();
      return *this;
    }

    Monotonic& Monotonic::operator = (const Elapsed& time)
    {
      ts.tv_sec = time.seconds;
      ts.tv_nsec = time.nanoseconds;
      normalize();
      return *this;
    }

    Monotonic Monotonic::operator - (const Monotonic& time) const
    {
      Monotonic newTime(*this);
      Monotonic timeN(time);
      newTime.ts.tv_nsec -= timeN.ts.tv_nsec;

      if(newTime.ts.tv_nsec < 0)
      {
        newTime.ts.tv_sec--;
        newTime.ts.tv_nsec += NANO_TO_SECOND;
      }
      else if(newTime.ts.tv_nsec > NANO_TO_SECOND)
      {
        newTime.ts.tv_sec++;
        newTime.ts.tv_nsec -= NANO_TO_SECOND;
      }

      newTime.ts.tv_sec -= timeN.ts.tv_sec;
      newTime.normalize();
      return newTime;
    }

    Monotonic Monotonic::operator - (const Elapsed& time) const
    {
      Monotonic newTime(*this);
      newTime.ts.tv_nsec -= normalize(time).nanoseconds;

      if(newTime.ts.tv_nsec < 0)
      {
        newTime.ts.tv_sec--;
        newTime.ts.tv_nsec += NANO_TO_SECOND;
      }
      else if(newTime.ts.tv_nsec > NANO_TO_SECOND)
      {
        newTime.ts.tv_sec++;
        newTime.ts.tv_nsec -= NANO_TO_SECOND;
      }

      newTime.ts.tv_sec -= normalize(time).seconds;
      newTime.normalize();
      return newTime;
    }

    Monotonic& Monotonic::operator -= (const Monotonic& time)
    {
      normalize();
      Monotonic timeN(time);
      ts.tv_nsec -= timeN.ts.tv_nsec;

      if(ts.tv_nsec < 0)
      {
        ts.tv_sec--;
        ts.tv_nsec += NANO_TO_SECOND;
      }
      else if(ts.tv_nsec > NANO_TO_SECOND)
      {
        ts.tv_sec++;
        ts.tv_nsec -= NANO_TO_SECOND;
      }

      ts.tv_sec -= timeN.ts.tv_sec;
      normalize();
      return *this;
    }

    Monotonic& Monotonic::operator -= (const Elapsed& time)
    {
      normalize();
      ts.tv_nsec -= normalize(time).nanoseconds;

      if(ts.tv_nsec < 0)
      {
        ts.tv_sec--;
        ts.tv_nsec += NANO_TO_SECOND;
      }
      else if(ts.tv_nsec > NANO_TO_SECOND)
      {
        ts.tv_sec++;
        ts.tv_nsec -= NANO_TO_SECOND;
      }

      ts.tv_sec -= normalize(time).seconds;
      normalize();
      return *this;
    }

    Monotonic Monotonic::operator + (const Monotonic& time) const
    {
      Monotonic newTime(*this);
      Monotonic timeN(time);
      newTime.ts.tv_nsec += timeN.ts.tv_nsec;

      if(newTime.ts.tv_nsec < 0)
      {
        newTime.ts.tv_sec--;
        newTime.ts.tv_nsec += NANO_TO_SECOND;
      }
      else if(newTime.ts.tv_nsec > NANO_TO_SECOND)
      {
        newTime.ts.tv_sec++;
        newTime.ts.tv_nsec -= NANO_TO_SECOND;
      }

      newTime.ts.tv_sec += timeN.ts.tv_sec;
      newTime.normalize();
      return newTime;
    }

    Monotonic Monotonic::operator + (const Elapsed& time) const
    {
      Monotonic newTime(*this);
      newTime.ts.tv_nsec += normalize(time).nanoseconds;

      if(newTime.ts.tv_nsec < 0)
      {
        newTime.ts.tv_sec--;
        newTime.ts.tv_nsec += NANO_TO_SECOND;
      }
      else if(newTime.ts.tv_nsec > NANO_TO_SECOND)
      {
        newTime.ts.tv_sec++;
        newTime.ts.tv_nsec -= NANO_TO_SECOND;
      }

      newTime.ts.tv_sec += normalize(time).seconds;
      newTime.normalize();
      return newTime;
    }

    Monotonic& Monotonic::operator += (const Monotonic& time)
    {
      normalize();
      Monotonic timeN(time);
      ts.tv_nsec += timeN.ts.tv_nsec;

      if(ts.tv_nsec < 0)
      {
        ts.tv_sec--;
        ts.tv_nsec += NANO_TO_SECOND;
      }
      else if(ts.tv_nsec > NANO_TO_SECOND)
      {
        ts.tv_sec++;
        ts.tv_nsec -= NANO_TO_SECOND;
      }

      ts.tv_sec += timeN.ts.tv_sec;
      normalize();
      return *this;
    }

    Monotonic& Monotonic::operator += (const Elapsed& time)
    {
      normalize();
      ts.tv_nsec += normalize(time).nanoseconds;

      if(ts.tv_nsec < 0)
      {
        ts.tv_sec--;
        ts.tv_nsec += NANO_TO_SECOND;
      }
      else if(ts.tv_nsec > NANO_TO_SECOND)
      {
        ts.tv_sec++;
        ts.tv_nsec -= NANO_TO_SECOND;
      }

      ts.tv_sec += normalize(time).seconds;
      normalize();
      return *this;
    }

    bool Monotonic::operator < (const Monotonic& time) const
    {
      if(ts.tv_sec != time.ts.tv_sec)
        return ts.tv_sec < time.ts.tv_sec;

      return ts.tv_nsec < time.ts.tv_nsec;
    }

    bool Monotonic::operator < (const Elapsed& time) const
    {
      if(ts.tv_sec != time.seconds)
        return ts.tv_sec < time.seconds;

      return ts.tv_nsec < time.nanoseconds;
    }

    bool Monotonic::operator > (const Monotonic& time) const
    {
      if(ts.tv_sec != time.ts.tv_sec)
        return ts.tv_sec > time.ts.tv_sec;

      return ts.tv_nsec > time.ts.tv_nsec;
    }

    bool Monotonic::operator > (const Elapsed& time) const
    {
      if(ts.tv_sec != time.seconds)
        return ts.tv_sec > time.seconds;

      return ts.tv_nsec > time.nanoseconds;
    }

    bool Monotonic::operator <= (const Monotonic& time) const
    {
      if(ts.tv_sec != time.ts.tv_sec)
        return ts.tv_sec <= time.ts.tv_sec;

      return ts.tv_nsec <= time.ts.tv_nsec;
    }

    bool Monotonic::operator <= (const Elapsed& time) const
    {
      if(ts.tv_sec != time.seconds)
        return ts.tv_sec <= time.seconds;

      return ts.tv_nsec <= time.nanoseconds;
    }

    bool Monotonic::operator >= (const Monotonic& time) const
    {
      if(ts.tv_sec != time.ts.tv_sec)
        return ts.tv_sec >= time.ts.tv_sec;

      return ts.tv_nsec >= time.ts.tv_nsec;
    }

    bool Monotonic::operator >= (const Elapsed& time) const
    {
      if(ts.tv_sec != time.seconds)
        return ts.tv_sec >= time.seconds;

      return ts.tv_nsec >= time.nanoseconds;
    }

    bool Monotonic::operator != (const Monotonic& time) const
    {
      return ts.tv_sec != time.ts.tv_sec || ts.tv_nsec != time.ts.tv_nsec;
    }

    bool Monotonic::operator != (const Elapsed& time) const
    {
      return ts.tv_sec != time.seconds || ts.tv_nsec != time.nanoseconds;
    }

    bool Monotonic::operator == (const Monotonic& time) const
    {
      return !(operator != (time));
    }

    bool Monotonic::operator == (const Elapsed& time) const
    {
      return !(operator != (time));
    }

    std::string Monotonic::toString() const
    {
      return Base::toString(ts);
    }
  }

}
