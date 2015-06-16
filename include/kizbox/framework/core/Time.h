/*
 * Time.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_TIME_H_
#define OVERKIZ_TIME_H_

#include <limits.h>
#include <time.h>

#include <kizbox/framework/core/Exception.h>

namespace Overkiz
{

  namespace Date
  {

    class Locale;
    class UTC;

  }

  namespace Time
  {

    class NotReliable: public Overkiz::Exception
    {
    public:
      NotReliable()
      {
      }

      virtual ~NotReliable()
      {
      }

      const char *getId() const
      {
        return "com.kizbox.Framework.Core.Time.NotReliable";
      }
    };

    /**
     * Structure representing time elapsed since the epoch (if real time)
     * or since the computer start (if monotonic time).
     */
    struct Elapsed
    {
      Elapsed()
      {
        seconds = 0;
        nanoseconds = 0;
      }

      Elapsed(time_t secs, long nsecs)
      {
        seconds = secs;
        nanoseconds = nsecs;
      }

      time_t seconds;
      long nanoseconds;

      static const Elapsed MAX;

    };

    /**
     * Class defining a Real time (time elapsed since the epoch)
     */
    class Real
    {
    public:

      /**
       * Constructor.
       *
       * @return a Real time object with the current time (same as Real::now ())
       */
      Real();

      /**
       * Constructor.
       *
       * @param time : the time to copy
       * @return a Real time object with the given time.
       */
      Real(const Real& time);

      /**
       * Constructor.
       *
       * @param time : time elapsed since epoch
       * @return a Real time object with the given time.
       */
      Real(const Elapsed& time);

      /**
       * Destructor.
       *
       * @return
       */
      virtual ~Real();

      /**
       * Function used to get the current time
       *
       * @return the current time.
       */
      static Real now();

      /**
       * Function used to retrieve this time in Elapsed time format
       * (time elpased since epoch).
       *
       * @param time : the structure containing the result.
       */
      operator Elapsed() const;

      /**
       * See constructor Real(const Real& time).
       *
       * @param time
       * @return
       */
      Real& operator = (const Real& time);

      /**
       * See constructor Real(const Elapsed& time).
       *
       * @param time
       * @return
       */
      Real& operator = (const Elapsed& time);

      /**
       * Operator -
       *
       * @param time
       * @return
       */
      Real operator - (const Real& time) const;

      Real operator - (const Elapsed& time) const;

      /**
       * Operator -=
       *
       * @param time
       * @return
       */
      Real& operator -= (const Real& time);

      Real& operator -= (const Elapsed& time);

      /**
       * Opeartor +
       *
       * @param time
       * @return
       */
      Real operator + (const Real& time) const;

      Real operator + (const Elapsed& epoch) const;

      /**
       * Opeartor +=
       *
       * @param time
       * @return
       */
      Real& operator += (const Real& time);

      Real& operator += (const Elapsed& epoch);

      /**
       * Operator <
       *
       * @param time
       * @return
       */
      bool operator < (const Real& time) const;

      bool operator < (const Elapsed& epoch) const;

      /**
       * Operator >
       *
       * @param time
       * @return
       */
      bool operator > (const Real& time) const;

      bool operator > (const Elapsed& epoch) const;

      /**
       * Operator <=
       *
       * @param time
       * @return
       */
      bool operator <= (const Real& time) const;

      bool operator <= (const Elapsed& epoch) const;

      /**
       * Operator >=
       *
       * @param time
       * @return
       */
      bool operator >= (const Real& time) const;

      bool operator >= (const Elapsed& epoch) const;

      /**
       * Operator !=
       *
       * @param time
       * @return
       */
      bool operator != (const Real& time) const;

      bool operator != (const Elapsed& epoch) const;

      /**
       * Operator ==
       *
       * @param time
       * @return
       */
      bool operator == (const Real& time) const;

      bool operator == (const Elapsed& epoch) const;

      /**
       *
       */
      static bool reliable();

    private:

      struct timespec ts;

    };

    /**
     * Class defining a Monotonic time (time elapsed since the epoch)
     */
    class Monotonic
    {
    public:
      /**
       * Constructor.
       *
       * @return a Monotonic time object with the current time.
       */
      Monotonic();

      /**
       * Constructor.
       *
       * @param time : the time to copy
       * @return a Monotonic time object with the given time.
       */
      Monotonic(const Monotonic& time);

      /**
       * Constructor.
       *
       * @param time : the time to set (time elapsed since the compute startup).
       * @return a Monotonic time object with the given time.
       */
      Monotonic(const Elapsed& time);

      /**
       * Destructor.
       *
       * @return
       */
      virtual ~Monotonic();

      /**
       * Function return the current time.
       *
       * @return
       */
      static Monotonic now();

      /**
       * Function used to retrieve this time in Elapsed time format
       * (time elpased since the computer startup).
       *
       */
      operator Elapsed() const;

      /**
       * See constructor.
       *
       * @param time
       * @return
       */
      Monotonic& operator = (const Monotonic& time);

      /**
       * See constructor.
       *
       * @param time
       * @return
       */
      Monotonic& operator = (const Elapsed& time);

      /**
       * Opeartor -
       *
       * @param time
       * @return
       */
      Monotonic operator - (const Monotonic& time) const;

      Monotonic operator - (const Elapsed& time) const;

      /**
       * Opeartor -=
       *
       * @param time
       * @return
       */
      Monotonic& operator -= (const Monotonic& time);

      Monotonic& operator -= (const Elapsed& time);

      /**
       * Operator +
       *
       * @param time
       * @return
       */
      Monotonic operator + (const Monotonic& time) const;

      Monotonic operator + (const Elapsed& time) const;

      /**
       * Operator +=
       *
       * @param time
       * @return
       */
      Monotonic& operator += (const Monotonic& time);

      Monotonic& operator += (const Elapsed& time);

      /**
       * Opeartor <
       *
       * @param time
       * @return
       */
      bool operator < (const Monotonic& time) const;

      bool operator < (const Elapsed& time) const;

      /**
       * Operator >
       *
       * @param time
       * @return
       */
      bool operator > (const Monotonic& time) const;

      bool operator > (const Elapsed& time) const;

      /**
       * Operator <=
       *
       * @param time
       * @return
       */
      bool operator <= (const Monotonic& time) const;

      bool operator <= (const Elapsed& time) const;

      /**
       * Operator >=
       *
       * @param time
       * @return
       */
      bool operator >= (const Monotonic& time) const;

      bool operator >= (const Elapsed& time) const;

      /**
       * Operator !=
       *
       * @param time
       * @return
       */
      bool operator != (const Monotonic& time) const;

      bool operator != (const Elapsed& time) const;

      /**
       * Operator ==
       *
       * @param time
       * @return
       */
      bool operator == (const Monotonic& time) const;

      bool operator == (const Elapsed& time) const;

    private:

      struct timespec ts;
    };

  }

}

#endif /* OVERKIZ_TIME_H_ */
