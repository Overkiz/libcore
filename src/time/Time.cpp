/*
 * Time.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>

#include "Time.h"

namespace Overkiz
{
  namespace Time
  {

    const Elapsed Elapsed::MAX(LONG_MAX, 999999999);

    Real::Real()
    {
      clock_gettime(CLOCK_REALTIME, &ts);
    }

    Real::Real(const Real& time)
    {
      *this = time;
    }

    Real::Real(const Elapsed& epoch)
    {
      *this = epoch;
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
      return *this;
    }

    Real& Real::operator = (const Elapsed& epoch)
    {
      ts.tv_sec = epoch.seconds;
      ts.tv_nsec = epoch.nanoseconds;
      return *this;
    }

    Real Real::operator - (const Real& time) const
    {
      Real newTime(*this);
      newTime.ts.tv_nsec -= time.ts.tv_nsec;

      if(newTime.ts.tv_nsec < 0)
      {
        newTime.ts.tv_nsec += 1000000000;
        newTime.ts.tv_sec--;
      }
      else if(newTime.ts.tv_nsec >= 1000000000)
      {
        newTime.ts.tv_nsec -= 1000000000;
        newTime.ts.tv_sec++;
      }

      newTime.ts.tv_sec -= time.ts.tv_sec;

      if(newTime.ts.tv_sec < 0)
      {
        newTime.ts.tv_sec = 0;
      }

      return newTime;
    }

    Real Real::operator - (const Elapsed& epoch) const
    {
      Real newTime(*this);
      newTime.ts.tv_nsec -= epoch.nanoseconds;

      if(newTime.ts.tv_nsec < 0)
      {
        newTime.ts.tv_nsec += 1000000000;
        newTime.ts.tv_sec--;
      }
      else if(newTime.ts.tv_nsec >= 1000000000)
      {
        newTime.ts.tv_nsec -= 1000000000;
        newTime.ts.tv_sec++;
      }

      newTime.ts.tv_sec -= epoch.seconds;

      if(newTime.ts.tv_sec < 0)
      {
        newTime.ts.tv_sec = 0;
      }

      return newTime;
    }

    Real& Real::operator -= (const Real& time)
    {
      ts.tv_nsec -= time.ts.tv_nsec;

      if(ts.tv_nsec < 0)
      {
        ts.tv_nsec += 1000000000;
        ts.tv_sec--;
      }
      else if(ts.tv_nsec >= 1000000000)
      {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec++;
      }

      ts.tv_sec -= time.ts.tv_sec;

      if(ts.tv_sec < 0)
      {
        ts.tv_sec = 0;
      }

      return *this;
    }

    Real& Real::operator -= (const Elapsed& epoch)
    {
      ts.tv_nsec -= epoch.nanoseconds;

      if(ts.tv_nsec < 0)
      {
        ts.tv_nsec += 1000000000;
        ts.tv_sec--;
      }
      else if(ts.tv_nsec >= 1000000000)
      {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec++;
      }

      ts.tv_sec -= epoch.seconds;

      if(ts.tv_sec < 0)
      {
        ts.tv_sec = 0;
      }

      return *this;
    }

    Real Real::operator + (const Real& time) const
    {
      Real newTime(*this);
      newTime.ts.tv_nsec += time.ts.tv_nsec;

      if(newTime.ts.tv_nsec < 0)
      {
        newTime.ts.tv_nsec += 1000000000;
        newTime.ts.tv_sec--;
      }
      else if(newTime.ts.tv_nsec >= 1000000000)
      {
        newTime.ts.tv_nsec -= 1000000000;
        newTime.ts.tv_sec++;
      }

      newTime.ts.tv_sec += time.ts.tv_sec;
      return newTime;
    }

    Real Real::operator + (const Elapsed& epoch) const
    {
      Real newTime(*this);
      newTime.ts.tv_nsec += epoch.nanoseconds;

      if(newTime.ts.tv_nsec < 0)
      {
        newTime.ts.tv_nsec += 1000000000;
        newTime.ts.tv_sec--;
      }
      else if(newTime.ts.tv_nsec >= 1000000000)
      {
        newTime.ts.tv_nsec -= 1000000000;
        newTime.ts.tv_sec++;
      }

      newTime.ts.tv_sec += epoch.seconds;
      return newTime;
    }

    Real& Real::operator += (const Real& time)
    {
      ts.tv_nsec += time.ts.tv_nsec;

      if(ts.tv_nsec < 0)
      {
        ts.tv_nsec += 1000000000;
        ts.tv_sec--;
      }
      else if(ts.tv_nsec >= 1000000000)
      {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec++;
      }

      ts.tv_sec += time.ts.tv_sec;
      return *this;
    }

    Real& Real::operator += (const Elapsed& epoch)
    {
      ts.tv_nsec += epoch.nanoseconds;

      if(ts.tv_nsec < 0)
      {
        ts.tv_nsec += 1000000000;
        ts.tv_sec--;
      }
      else if(ts.tv_nsec >= 1000000000)
      {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec++;
      }

      ts.tv_sec += epoch.seconds;
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
      if(ts.tv_sec != time.ts.tv_sec)
        return true;

      return ts.tv_nsec != time.ts.tv_nsec;
    }

    bool Real::operator != (const Elapsed& epoch) const
    {
      if(ts.tv_sec != epoch.seconds)
        return true;

      return ts.tv_nsec != epoch.nanoseconds;
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
      int tmp = 0;
      bool ret;
      #ifdef RTC_VL_READ
      int rtc = open("/dev/rtc", O_RDONLY);
      int res = ioctl(rtc, RTC_VL_READ, &tmp);

      if(tmp || res < 0)
      {
        ret = false;
      }
      else
      {
        ret = true;
      }

      close(rtc);
      #else
      ret = true;
      #endif
      return ret;
    }

    Monotonic::Monotonic()
    {
      clock_gettime(CLOCK_MONOTONIC, &ts);
    }

    Monotonic::Monotonic(const Monotonic& time)
    {
      ts = time.ts;
    }

    Monotonic::Monotonic(const Elapsed& time)
    {
      ts.tv_sec = time.seconds;
      ts.tv_nsec = time.nanoseconds;
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
      return *this;
    }

    Monotonic& Monotonic::operator = (const Elapsed& time)
    {
      ts.tv_sec = time.seconds;
      ts.tv_nsec = time.nanoseconds;
      return *this;
    }

    Monotonic Monotonic::operator - (const Monotonic& time) const
    {
      Monotonic newTime(*this);
      newTime.ts.tv_nsec -= time.ts.tv_nsec;

      if(newTime.ts.tv_nsec < 0)
      {
        newTime.ts.tv_sec--;
        newTime.ts.tv_nsec += 1000000000;
      }
      else if(newTime.ts.tv_nsec > 1000000000)
      {
        newTime.ts.tv_sec++;
        newTime.ts.tv_nsec -= 1000000000;
      }

      newTime.ts.tv_sec -= time.ts.tv_sec;
      return newTime;
    }

    Monotonic Monotonic::operator - (const Elapsed& time) const
    {
      Monotonic newTime(*this);
      newTime.ts.tv_nsec -= time.nanoseconds;

      if(newTime.ts.tv_nsec < 0)
      {
        newTime.ts.tv_sec--;
        newTime.ts.tv_nsec += 1000000000;
      }
      else if(newTime.ts.tv_nsec > 1000000000)
      {
        newTime.ts.tv_sec++;
        newTime.ts.tv_nsec -= 1000000000;
      }

      newTime.ts.tv_sec -= time.seconds;
      return newTime;
    }

    Monotonic& Monotonic::operator -= (const Monotonic& time)
    {
      ts.tv_nsec -= time.ts.tv_nsec;

      if(ts.tv_nsec < 0)
      {
        ts.tv_sec--;
        ts.tv_nsec += 1000000000;
      }
      else if(ts.tv_nsec > 1000000000)
      {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
      }

      ts.tv_sec -= time.ts.tv_sec;
      return *this;
    }

    Monotonic& Monotonic::operator -= (const Elapsed& time)
    {
      ts.tv_nsec -= time.nanoseconds;

      if(ts.tv_nsec < 0)
      {
        ts.tv_sec--;
        ts.tv_nsec += 1000000000;
      }
      else if(ts.tv_nsec > 1000000000)
      {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
      }

      ts.tv_sec -= time.seconds;
      return *this;
    }

    Monotonic Monotonic::operator + (const Monotonic& time) const
    {
      Monotonic newTime(*this);
      newTime.ts.tv_nsec += time.ts.tv_nsec;

      if(newTime.ts.tv_nsec < 0)
      {
        newTime.ts.tv_sec--;
        newTime.ts.tv_nsec += 1000000000;
      }
      else if(newTime.ts.tv_nsec > 1000000000)
      {
        newTime.ts.tv_sec++;
        newTime.ts.tv_nsec -= 1000000000;
      }

      newTime.ts.tv_sec += time.ts.tv_sec;
      return newTime;
    }

    Monotonic Monotonic::operator + (const Elapsed& time) const
    {
      Monotonic newTime(*this);
      newTime.ts.tv_nsec += time.nanoseconds;

      if(newTime.ts.tv_nsec < 0)
      {
        newTime.ts.tv_sec--;
        newTime.ts.tv_nsec += 1000000000;
      }
      else if(newTime.ts.tv_nsec > 1000000000)
      {
        newTime.ts.tv_sec++;
        newTime.ts.tv_nsec -= 1000000000;
      }

      newTime.ts.tv_sec += time.seconds;
      return newTime;
    }

    Monotonic& Monotonic::operator += (const Monotonic& time)
    {
      ts.tv_nsec += time.ts.tv_nsec;

      if(ts.tv_nsec < 0)
      {
        ts.tv_sec--;
        ts.tv_nsec += 1000000000;
      }
      else if(ts.tv_nsec > 1000000000)
      {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
      }

      ts.tv_sec += time.ts.tv_sec;
      return *this;
    }

    Monotonic& Monotonic::operator += (const Elapsed& time)
    {
      ts.tv_nsec += time.nanoseconds;

      if(ts.tv_nsec < 0)
      {
        ts.tv_sec--;
        ts.tv_nsec += 1000000000;
      }
      else if(ts.tv_nsec > 1000000000)
      {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
      }

      ts.tv_sec += time.seconds;
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
      if(ts.tv_sec <= time.ts.tv_sec)
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
      if(ts.tv_sec >= time.ts.tv_sec)
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
      if(ts.tv_sec != time.ts.tv_sec)
        return true;

      return ts.tv_nsec != time.ts.tv_nsec;
    }

    bool Monotonic::operator != (const Elapsed& time) const
    {
      if(ts.tv_sec != time.seconds)
        return true;

      return ts.tv_nsec != time.nanoseconds;
    }

    bool Monotonic::operator == (const Monotonic& time) const
    {
      return !(operator != (time));
    }

    bool Monotonic::operator == (const Elapsed& time) const
    {
      return !(operator != (time));
    }

  }

}