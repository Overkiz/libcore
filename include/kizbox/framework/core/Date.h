/*
 * Date.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef DATE_H_
#define DATE_H_

#include <time.h>
#include <stdint.h>

#include <kizbox/framework/core/Time.h>
#include <kizbox/framework/core/Exception.h>

namespace Overkiz
{

  namespace Date
  {

    class UTC;
    class Locale;

    class InvalidException: public Exception
    {
    public:
      InvalidException()
      {
      }

      virtual ~InvalidException()
      {
      }

      const char *getId() const
      {
        return "com.overkiz.Framework.Core.Date.InvalidException";
      }
    };

    namespace Relative
    {
      struct Day
      {
        uint32_t hour : 5;
        uint32_t minute : 6;
        uint32_t second : 6;
        uint32_t nanosecond : 30;

        bool operator == (const Day& val) const;
        bool operator != (const Day& val) const;
        bool operator < (const Day& val) const;
        bool operator > (const Day& val) const;
        bool operator <= (const Day& val) const;
        bool operator >= (const Day& val) const;

        void normalize();
      };

      struct Week
      {
        uint32_t day : 3;
        uint32_t hour : 5;
        uint32_t minute : 6;
        uint32_t second : 6;
        uint32_t nanosecond : 30;

        bool operator == (const Week& val) const;
        bool operator != (const Week& val) const;
        bool operator < (const Week& val) const;
        bool operator > (const Week& val) const;
        bool operator <= (const Week& val) const;
        bool operator >= (const Week& val) const;

        void normalize();
      };

      struct Month
      {
        uint32_t day : 5;
        uint32_t hour : 5;
        uint32_t minute : 6;
        uint32_t second : 6;
        uint32_t nanosecond : 30;

        bool operator == (const Month& val) const;
        bool operator != (const Month& val) const;
        bool operator < (const Month& val) const;
        bool operator > (const Month& val) const;
        bool operator <= (const Month& val) const;
        bool operator >= (const Month& val) const;

        void normalize();
      };

      namespace Year
      {
        struct Day
        {
          uint32_t day : 9;
          uint32_t hour : 5;
          uint32_t minute : 6;
          uint32_t second : 6;
          uint32_t nanosecond : 30;

          bool operator == (const Day& val) const;
          bool operator != (const Day& val) const;
          bool operator < (const Day& val) const;
          bool operator > (const Day& val) const;
          bool operator <= (const Day& val) const;
          bool operator >= (const Day& val) const;

          void normalize();
        };

        struct Month
        {
          uint32_t month : 4;
          uint32_t day : 5;
          uint32_t hour : 5;
          uint32_t minute : 6;
          uint32_t second : 6;
          uint32_t nanosecond : 30;

          bool operator == (const Month& val) const;
          bool operator != (const Month& val) const;
          bool operator < (const Month& val) const;
          bool operator > (const Month& val) const;
          bool operator <= (const Month& val) const;
          bool operator >= (const Month& val) const;

          void normalize();
        };

      }

    }

    struct Absolute
    {
      int year;
      uint32_t month : 4;
      uint32_t day : 5;
      uint32_t hour : 5;
      uint32_t minute : 6;
      uint32_t second : 6;
      uint32_t nanosecond : 30;

      bool operator == (const Absolute& val) const;
      bool operator != (const Absolute& val) const;
      bool operator < (const Absolute& val) const;
      bool operator > (const Absolute& val) const;
      bool operator <= (const Absolute& val) const;
      bool operator >= (const Absolute& val) const;

      operator Relative::Day() const;

      Absolute next(const Relative::Day& value) const;

      Absolute previous(const Relative::Day& value) const;

      operator Relative::Week() const;

      Absolute next(const Relative::Week& value) const;

      Absolute previous(const Relative::Week& value) const;

      operator Relative::Month() const;

      Absolute next(const Relative::Month& value) const;

      Absolute previous(const Relative::Month& value) const;

      operator Relative::Year::Day() const;

      Absolute next(const Relative::Year::Day& value) const;

      Absolute previous(const Relative::Year::Day& value) const;

      operator Relative::Year::Month() const;

      Absolute next(const Relative::Year::Month& value) const;

      Absolute previous(const Relative::Year::Month& value) const;

      void normalize();
    };

    class Base
    {
    public:
      Base();

      virtual ~Base();

      virtual operator Time::Real() const = 0;

      operator Absolute() const;

      Base& operator = (const Absolute& value);

    protected:
      struct tm tm;
      long nanosecond;

      friend class UTC;
      friend class Locale;
    };

    class Locale: public Base
    {
    public:
      Locale(const Locale& value);

      Locale(const UTC& value);

      Locale(const Time::Real& value = Time::Real::now());

      Locale(const Absolute& value);

      virtual ~Locale();

      Locale& operator = (const Locale& value);

      Locale& operator = (const UTC& value);

      Locale& operator = (const Time::Real& value);

      Locale& operator = (const Absolute& value);

      operator Time::Real() const;

    };

    class UTC: public Base
    {
    public:
      UTC(const UTC& value);

      UTC(const Locale& value);

      UTC(const Time::Real& value = Time::Real::now());

      UTC(const Absolute& value);

      virtual ~UTC();

      UTC& operator = (const UTC& value);

      UTC& operator = (const Locale& value);

      UTC& operator = (const Time::Real& value);

      UTC& operator = (const Absolute& value);

      operator Time::Real() const;
    };
  }

}

#endif /* DATE_H_ */

