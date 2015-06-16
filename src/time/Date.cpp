/*
 * Date.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <stdio.h>

#include "Date.h"

namespace Overkiz
{

  namespace Date
  {

    bool Relative::Day::operator == (const Day& val) const
    {
      Day tmp1 = val;
      Day tmp2 = *this;
      tmp1.normalize();
      tmp2.normalize();
      return (tmp2.hour == tmp1.hour && tmp2.minute == tmp1.minute
              && tmp2.second == tmp1.second && tmp2.nanosecond == tmp1.nanosecond);
    }

    bool Relative::Day::operator != (const Day& val) const
    {
      return !operator == (val);
    }

    bool Relative::Day::operator < (const Day& val) const
    {
      Day tmp1 = val;
      Day tmp2 = *this;
      tmp1.normalize();
      tmp2.normalize();
      return (tmp2.hour < tmp1.hour
              || ((tmp2.hour == tmp1.hour) && (tmp2.minute < tmp1.minute))
              || ((tmp2.hour == tmp1.hour) && (tmp2.minute == tmp1.minute)
                  && (tmp2.second < tmp1.second))
              || ((tmp2.hour == tmp1.hour) && (tmp2.minute == tmp1.minute)
                  && (tmp2.second == tmp1.second)
                  && (tmp2.nanosecond < tmp1.nanosecond)));
    }

    bool Relative::Day::operator > (const Day& val) const
    {
      Day tmp1 = val;
      Day tmp2 = *this;
      tmp1.normalize();
      tmp2.normalize();
      return (tmp2.hour > tmp1.hour
              || ((tmp2.hour == tmp1.hour) && (tmp2.minute > tmp1.minute))
              || ((tmp2.hour == tmp1.hour) && (tmp2.minute == tmp1.minute)
                  && (tmp2.second > tmp1.second))
              || ((tmp2.hour == tmp1.hour) && (tmp2.minute == tmp1.minute)
                  && (tmp2.second == tmp1.second)
                  && (tmp2.nanosecond > tmp1.nanosecond)));
    }

    bool Relative::Day::operator <= (const Day& val) const
    {
      return (operator < (val) || operator == (val));
    }

    bool Relative::Day::operator >= (const Day& val) const
    {
      return (operator > (val) || operator == (val));
    }

    void Relative::Day::normalize()
    {
      uint32_t tmp;
      tmp = nanosecond / 1000000000;
      nanosecond %= 1000000000;
      tmp += second;
      second = tmp % 60;
      tmp /= 60;
      tmp += minute;
      minute = tmp % 60;
      tmp /= 60;
      tmp += hour;
      hour = tmp % 24;
    }

    bool Relative::Week::operator == (const Week& val) const
    {
      Week tmp1 = val;
      Week tmp2 = *this;
      tmp1.normalize();
      tmp2.normalize();
      return (tmp2.day == tmp1.day && tmp2.hour == tmp1.hour
              && tmp2.minute == tmp1.minute && tmp2.second == tmp1.second
              && tmp2.nanosecond == tmp1.nanosecond);
    }

    bool Relative::Week::operator != (const Week& val) const
    {
      return !operator == (val);
    }

    bool Relative::Week::operator < (const Week& val) const
    {
      Week tmp1 = val;
      Week tmp2 = *this;
      tmp1.normalize();
      tmp2.normalize();
      return (tmp2.day < tmp1.day
              || ((tmp2.day == tmp1.day) && (tmp2.hour < tmp1.hour))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute < tmp1.minute))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute == tmp1.minute)
                  && (tmp2.second < tmp1.second))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute == tmp1.minute)
                  && (tmp2.second == tmp1.second)
                  && (tmp2.nanosecond < tmp1.nanosecond)));
    }

    bool Relative::Week::operator > (const Week& val) const
    {
      Week tmp1 = val;
      Week tmp2 = *this;
      tmp1.normalize();
      tmp2.normalize();
      return (tmp2.day > tmp1.day
              || ((tmp2.day == tmp1.day) && (tmp2.hour > tmp1.hour))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute > tmp1.minute))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute == tmp1.minute)
                  && (tmp2.second > tmp1.second))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute == tmp1.minute)
                  && (tmp2.second == tmp1.second)
                  && (tmp2.nanosecond > tmp1.nanosecond)));
    }

    bool Relative::Week::operator <= (const Week& val) const
    {
      return (operator < (val) || operator == (val));
    }

    bool Relative::Week::operator >= (const Week& val) const
    {
      return (operator > (val) || operator == (val));
    }

    void Relative::Week::normalize()
    {
      uint32_t tmp;
      tmp = nanosecond / 1000000000;
      nanosecond %= 1000000000;
      tmp += second;
      second = tmp % 60;
      tmp /= 60;
      tmp += minute;
      minute = tmp % 60;
      tmp /= 60;
      tmp += hour;
      hour = tmp % 24;
      tmp /= 24;
      tmp += day;
      day = tmp % 7;
    }

    bool Relative::Month::operator == (const Month& val) const
    {
      Month tmp1 = val;
      Month tmp2 = *this;
      tmp1.normalize();
      tmp2.normalize();
      return (tmp2.day == tmp1.day && tmp2.hour == tmp1.hour
              && tmp2.minute == tmp1.minute && tmp2.second == tmp1.second
              && tmp2.nanosecond == tmp1.nanosecond);
    }

    bool Relative::Month::operator != (const Month& val) const
    {
      return !operator == (val);
    }

    bool Relative::Month::operator < (const Month& val) const
    {
      Month tmp1 = val;
      Month tmp2 = *this;
      tmp1.normalize();
      tmp2.normalize();
      return (tmp2.day < tmp1.day
              || ((tmp2.day == tmp1.day) && (tmp2.hour < tmp1.hour))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute < tmp1.minute))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute == tmp1.minute)
                  && (tmp2.second < tmp1.second))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute == tmp1.minute)
                  && (tmp2.second == tmp1.second)
                  && (tmp2.nanosecond < tmp1.nanosecond)));
    }

    bool Relative::Month::operator > (const Month& val) const
    {
      Month tmp1 = val;
      Month tmp2 = *this;
      tmp1.normalize();
      tmp2.normalize();
      return (tmp2.day > tmp1.day
              || ((tmp2.day == tmp1.day) && (tmp2.hour > tmp1.hour))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute > tmp1.minute))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute == tmp1.minute)
                  && (tmp2.second > tmp1.second))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute == tmp1.minute)
                  && (tmp2.second == tmp1.second)
                  && (tmp2.nanosecond > tmp1.nanosecond)));
    }

    bool Relative::Month::operator <= (const Month& val) const
    {
      return (operator < (val) || operator == (val));
    }

    bool Relative::Month::operator >= (const Month& val) const
    {
      return (operator > (val) || operator == (val));
    }

    void Relative::Month::normalize()
    {
      uint32_t tmp;
      tmp = nanosecond / 1000000000;
      nanosecond %= 1000000000;
      tmp += second;
      second = tmp % 60;
      tmp /= 60;
      tmp += minute;
      minute = tmp % 60;
      tmp /= 60;
      tmp += hour;
      hour = tmp % 24;
      tmp /= 24;
      tmp += day;
      day = tmp % 31;
    }

    bool Relative::Year::Day::operator == (const Day& val) const
    {
      Day tmp1 = val;
      Day tmp2 = *this;
      tmp1.normalize();
      tmp2.normalize();
      return (tmp2.day == tmp1.day && tmp2.hour == tmp1.hour
              && tmp2.minute == tmp1.minute && tmp2.second == tmp1.second
              && tmp2.nanosecond == tmp1.nanosecond);
    }

    bool Relative::Year::Day::operator != (const Day& val) const
    {
      return !operator == (val);
    }

    bool Relative::Year::Day::operator < (const Day& val) const
    {
      Day tmp1 = val;
      Day tmp2 = *this;
      tmp1.normalize();
      tmp2.normalize();
      return (tmp2.day < tmp1.day
              || ((tmp2.day == tmp1.day) && (tmp2.hour < tmp1.hour))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute < tmp1.minute))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute == tmp1.minute)
                  && (tmp2.second < tmp1.second))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute == tmp1.minute)
                  && (tmp2.second == tmp1.second)
                  && (tmp2.nanosecond < tmp1.nanosecond)));
    }

    bool Relative::Year::Day::operator > (const Day& val) const
    {
      Day tmp1 = val;
      Day tmp2 = *this;
      tmp1.normalize();
      tmp2.normalize();
      return (tmp2.day > tmp1.day
              || ((tmp2.day == tmp1.day) && (tmp2.hour > tmp1.hour))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute > tmp1.minute))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute == tmp1.minute)
                  && (tmp2.second > tmp1.second))
              || ((tmp2.day == tmp1.day) && (tmp2.hour == tmp1.hour)
                  && (tmp2.minute == tmp1.minute)
                  && (tmp2.second == tmp1.second)
                  && (tmp2.nanosecond > tmp1.nanosecond)));
    }

    bool Relative::Year::Day::operator <= (const Day& val) const
    {
      return (operator < (val) || operator == (val));
    }

    bool Relative::Year::Day::operator >= (const Day& val) const
    {
      return (operator > (val) || operator == (val));
    }

    void Relative::Year::Day::normalize()
    {
      uint32_t tmp;
      tmp = nanosecond / 1000000000;
      nanosecond %= 1000000000;
      tmp += second;
      second = tmp % 60;
      tmp /= 60;
      tmp += minute;
      minute = tmp % 60;
      tmp /= 60;
      tmp += hour;
      hour = tmp % 24;
      tmp /= 24;
      tmp += day;
      day = tmp % 366;
    }

    bool Relative::Year::Month::operator == (const Month& val) const
    {
      Month tmp1 = val;
      Month tmp2 = *this;
      tmp1.normalize();
      tmp2.normalize();
      return (tmp2.month == tmp1.month && tmp2.day == tmp1.day
              && tmp2.hour == tmp1.hour && tmp2.minute == tmp1.minute
              && tmp2.second == tmp1.second && tmp2.nanosecond == tmp1.nanosecond);
    }

    bool Relative::Year::Month::operator != (const Month& val) const
    {
      return !operator == (val);
    }

    bool Relative::Year::Month::operator < (const Month& val) const
    {
      Month tmp1 = val;
      Month tmp2 = *this;
      tmp1.normalize();
      tmp2.normalize();
      return (tmp2.month < tmp1.month
              || ((tmp2.month == tmp1.month) && (tmp2.day < tmp1.day))
              || ((tmp2.month == tmp1.month) && (tmp2.day == tmp1.day)
                  && (tmp2.hour < tmp1.hour))
              || ((tmp2.month == tmp1.month) && (tmp2.day == tmp1.day)
                  && (tmp2.hour == tmp1.hour) && (tmp2.minute < tmp1.minute))
              || ((tmp2.month == tmp1.month) && (tmp2.day == tmp1.day)
                  && (tmp2.hour == tmp1.hour) && (tmp2.minute == tmp1.minute)
                  && (tmp2.second < tmp1.second))
              || ((tmp2.month == tmp1.month) && (tmp2.day == tmp1.day)
                  && (tmp2.hour == tmp1.hour) && (tmp2.minute == tmp1.minute)
                  && (tmp2.second == tmp1.second)
                  && (tmp2.nanosecond < tmp1.nanosecond)));
    }

    bool Relative::Year::Month::operator > (const Month& val) const
    {
      Month tmp1 = val;
      Month tmp2 = *this;
      tmp1.normalize();
      tmp2.normalize();
      return (tmp2.month > tmp1.month
              || ((tmp2.month == tmp1.month) && (tmp2.day > tmp1.day))
              || ((tmp2.month == tmp1.month) && (tmp2.day == tmp1.day)
                  && (tmp2.hour > tmp1.hour))
              || ((tmp2.month == tmp1.month) && (tmp2.day == tmp1.day)
                  && (tmp2.hour == tmp1.hour) && (tmp2.minute > tmp1.minute))
              || ((tmp2.month == tmp1.month) && (tmp2.day == tmp1.day)
                  && (tmp2.hour == tmp1.hour) && (tmp2.minute == tmp1.minute)
                  && (tmp2.second > tmp1.second))
              || ((tmp2.month == tmp1.month) && (tmp2.day == tmp1.day)
                  && (tmp2.hour == tmp1.hour) && (tmp2.minute == tmp1.minute)
                  && (tmp2.second == tmp1.second)
                  && (tmp2.nanosecond > tmp1.nanosecond)));
    }

    bool Relative::Year::Month::operator <= (const Month& val) const
    {
      return (operator < (val) || operator == (val));
    }

    bool Relative::Year::Month::operator >= (const Month& val) const
    {
      return (operator > (val) || operator == (val));
    }

    void Relative::Year::Month::normalize()
    {
      uint32_t tmp;
      tmp = nanosecond / 1000000000;
      nanosecond %= 1000000000;
      tmp += second;
      second = tmp % 60;
      tmp /= 60;
      tmp += minute;
      minute = tmp % 60;
      tmp /= 60;
      tmp += hour;
      hour = tmp % 24;
      tmp /= 24;
      tmp += day;
      month %= 12;

      while(tmp)
      {
        if(month == 1)
        {
          if(tmp < 29)
          {
            break;
          }

          tmp -= 29;
        }
        else if((month < 7 && month % 2) || (month >= 7 && !(month % 2)))
        {
          if(tmp < 30)
          {
            break;
          }

          tmp -= 30;
        }
        else
        {
          if(tmp < 31)
          {
            break;
          }

          tmp -= 31;
        }

        if(month < 11)
        {
          month++;
        }
        else
        {
          month = 0;
        }
      }

      day = tmp;
    }

    bool Absolute::operator == (const Absolute& val) const
    {
      return (year == val.year && month == val.month && day == val.day
              && hour == val.hour && minute == val.minute && second == val.second
              && nanosecond == val.nanosecond);
    }

    bool Absolute::operator != (const Absolute& val) const
    {
      return !operator == (val);
    }

    bool Absolute::operator < (const Absolute& val) const
    {
      return (year < val.year || ((year == val.year) && (month < val.month))
              || ((year == val.year) && (month == val.month) && (day < val.day))
              || ((year == val.year) && (month == val.month) && (day == val.day)
                  && (hour < val.hour))
              || ((year == val.year) && (month == val.month) && (day == val.day)
                  && (hour == val.hour) && (minute < val.minute))
              || ((year == val.year) && (month == val.month) && (day == val.day)
                  && (hour == val.hour) && (minute == val.minute)
                  && (second < val.second))
              || ((year == val.year) && (month == val.month) && (day == val.day)
                  && (hour == val.hour) && (minute == val.minute)
                  && (second == val.second) && (nanosecond < val.nanosecond)));
    }

    bool Absolute::operator > (const Absolute& val) const
    {
      return (year > val.year || ((year == val.year) && (month > val.month))
              || ((year == val.year) && (month == val.month) && (day > val.day))
              || ((year == val.year) && (month == val.month) && (day == val.day)
                  && (hour > val.hour))
              || ((year == val.year) && (month == val.month) && (day == val.day)
                  && (hour == val.hour) && (minute > val.minute))
              || ((year == val.year) && (month == val.month) && (day == val.day)
                  && (hour == val.hour) && (minute == val.minute)
                  && (second > val.second))
              || ((year == val.year) && (month == val.month) && (day == val.day)
                  && (hour == val.hour) && (minute == val.minute)
                  && (second == val.second) && (nanosecond > val.nanosecond)));
    }

    bool Absolute::operator <= (const Absolute& val) const
    {
      return (operator < (val) || operator == (val));
    }

    bool Absolute::operator >= (const Absolute& val) const
    {
      return (operator > (val) || operator == (val));
    }

    Absolute::operator Relative::Day() const
    {
      Relative::Day ret;
      ret.hour = hour;
      ret.minute = minute;
      ret.second = second;
      ret.nanosecond = nanosecond;
      return ret;
    }

    Absolute Absolute::next(const Relative::Day& value) const
    {
      struct tm tm;
      Absolute ret;
      Relative::Day current = *this;
      Relative::Day tmp = value;
      tmp.normalize();
      current.normalize();
      tm.tm_year = year - 1900;
      tm.tm_mon = month;
      tm.tm_mday = day + 1;

      if(current >= tmp)
      {
        tm.tm_mday++;
      }

      tm.tm_hour = value.hour;
      tm.tm_min = value.minute;
      tm.tm_sec = value.second;
      timegm(&tm);
      ret.year = tm.tm_year + 1900;
      ret.month = tm.tm_mon;
      ret.day = tm.tm_mday - 1;
      ret.hour = tm.tm_hour;
      ret.minute = tm.tm_min;
      ret.second = tm.tm_sec;
      ret.nanosecond = tmp.nanosecond;
      return ret;
    }

    Absolute Absolute::previous(const Relative::Day& value) const
    {
      struct tm tm;
      Absolute ret;
      Relative::Day current = *this;
      Relative::Day tmp = value;
      tmp.normalize();
      current.normalize();
      tm.tm_year = year - 1900;
      tm.tm_mon = month;
      tm.tm_mday = day + 1;

      if(current <= tmp)
      {
        tm.tm_mday--;
      }

      tm.tm_hour = value.hour;
      tm.tm_min = value.minute;
      tm.tm_sec = value.second;
      timegm(&tm);
      ret.year = tm.tm_year + 1900;
      ret.month = tm.tm_mon;
      ret.day = tm.tm_mday - 1;
      ret.hour = tm.tm_hour;
      ret.minute = tm.tm_min;
      ret.second = tm.tm_sec;
      ret.nanosecond = tmp.nanosecond;
      return ret;
    }

    Absolute::operator Relative::Week() const
    {
      Relative::Week ret;
      struct tm tm;
      tm.tm_year = year - 1900;
      tm.tm_mon = month;
      tm.tm_mday = day + 1;
      tm.tm_hour = hour;
      tm.tm_min = minute;
      tm.tm_sec = second;
      timegm(&tm);
      ret.day = tm.tm_wday;
      ret.hour = hour;
      ret.minute = minute;
      ret.second = second;
      ret.nanosecond = nanosecond;
      return ret;
    }

    Absolute Absolute::next(const Relative::Week& value) const
    {
      struct tm tm;
      Absolute ret;
      Relative::Week current = *this;
      Relative::Week tmp = value;
      tmp.normalize();
      current.normalize();
      tm.tm_year = year - 1900;
      tm.tm_mon = month;
      tm.tm_mday = day + 1;
      tm.tm_hour = tmp.hour;
      tm.tm_min = tmp.minute;
      tm.tm_sec = tmp.second;
      timegm(&tm);

      if(current >= tmp)
      {
        tm.tm_mday += 7;
      }

      tm.tm_mday += tmp.day - current.day;
      tm.tm_hour = tmp.hour;
      tm.tm_min = tmp.minute;
      tm.tm_sec = tmp.second;
      timegm(&tm);
      ret.year = tm.tm_year + 1900;
      ret.month = tm.tm_mon;
      ret.day = tm.tm_mday - 1;
      ret.hour = tm.tm_hour;
      ret.minute = tm.tm_min;
      ret.second = tm.tm_sec;
      ret.nanosecond = tmp.nanosecond;
      return ret;
    }

    Absolute Absolute::previous(const Relative::Week& value) const
    {
      struct tm tm;
      Absolute ret;
      Relative::Week current = *this;
      Relative::Week tmp = value;
      tmp.normalize();
      current.normalize();
      tm.tm_year = year - 1900;
      tm.tm_mon = month;
      tm.tm_mday = day + 1;
      tm.tm_hour = tmp.hour;
      tm.tm_min = tmp.minute;
      tm.tm_sec = tmp.second;
      timegm(&tm);

      if(current <= tmp)
      {
        tm.tm_mday -= 7;
      }

      tm.tm_mday -= tmp.day - current.day;
      tm.tm_mday += tm.tm_wday;
      tm.tm_mday -= tmp.day;
      tm.tm_hour = tmp.hour;
      tm.tm_min = tmp.minute;
      tm.tm_sec = tmp.second;
      timegm(&tm);
      ret.year = tm.tm_year + 1900;
      ret.month = tm.tm_mon;
      ret.day = tm.tm_mday - 1;
      ret.hour = tm.tm_hour;
      ret.minute = tm.tm_min;
      ret.second = tm.tm_sec;
      ret.nanosecond = tmp.nanosecond;
      return ret;
    }

    Absolute::operator Relative::Month() const
    {
      Relative::Month ret;
      ret.day = day;
      ret.hour = hour;
      ret.minute = minute;
      ret.second = second;
      ret.nanosecond = nanosecond;
      return ret;
    }

    Absolute Absolute::next(const Relative::Month& value) const
    {
      struct tm tm;
      Absolute ret;
      Relative::Month current = *this;
      Relative::Month tmp = value;
      tmp.normalize();
      current.normalize();
      tm.tm_year = year - 1900;
      tm.tm_mon = month;
      tm.tm_mday = tmp.day + 1;
      tm.tm_hour = tmp.hour;
      tm.tm_min = tmp.minute;
      tm.tm_sec = tmp.second;

      if(current >= tmp)
      {
        tm.tm_mon++;
      }

      timegm(&tm);

      switch(tmp.day)
      {
        case 30:
          if((tm.tm_mon < 7 && tm.tm_mon % 2)
             || (tm.tm_mon >= 7 && !(tm.tm_mon % 2)))
          {
            tm.tm_mon++;
          }

          break;

        case 29:
          if(tm.tm_mon == 1)
          {
            tm.tm_mon++;
          }

          break;

        case 28:
          if(tm.tm_mon == 1)
          {
            if((tm.tm_year % 4)
               || (!(tm.tm_year % 100) && (tm.tm_year % 400)))
            {
              tm.tm_mon++;
            }
          }

          break;

        default:
          break;
      }

      timegm(&tm);
      ret.year = tm.tm_year + 1900;
      ret.month = tm.tm_mon;
      ret.day = tm.tm_mday - 1;
      ret.hour = tm.tm_hour;
      ret.minute = tm.tm_min;
      ret.second = tm.tm_sec;
      ret.nanosecond = tmp.nanosecond;
      return ret;
    }

    Absolute Absolute::previous(const Relative::Month& value) const
    {
      struct tm tm;
      Absolute ret;
      Relative::Month current = *this;
      Relative::Month tmp = value;
      tmp.normalize();
      current.normalize();
      tm.tm_year = year - 1900;
      tm.tm_mon = month;
      tm.tm_mday = tmp.day + 1;
      tm.tm_hour = tmp.hour;
      tm.tm_min = tmp.minute;
      tm.tm_sec = tmp.second;

      if(current <= tmp)
      {
        tm.tm_mon--;
      }

      timegm(&tm);

      switch(tmp.day)
      {
        case 30:
          if((tm.tm_mon < 7 && tm.tm_mon % 2)
             || (tm.tm_mon >= 7 && !(tm.tm_mon % 2)))
          {
            tm.tm_mon--;
          }

          break;

        case 29:
          if(tm.tm_mon == 1)
          {
            tm.tm_mon--;
          }

          break;

        case 28:
          if(tm.tm_mon == 1)
          {
            if((tm.tm_year % 4)
               || (!(tm.tm_year % 100) && (tm.tm_year % 400)))
            {
              tm.tm_mon--;
            }
          }

          break;

        default:
          break;
      }

      timegm(&tm);
      ret.year = tm.tm_year + 1900;
      ret.month = tm.tm_mon;
      ret.day = tm.tm_mday - 1;
      ret.hour = tm.tm_hour;
      ret.minute = tm.tm_min;
      ret.second = tm.tm_sec;
      ret.nanosecond = tmp.nanosecond;
      return ret;
    }

    Absolute::operator Relative::Year::Day() const
    {
      Relative::Year::Day ret;
      struct tm tm;
      tm.tm_year = year - 1900;
      tm.tm_mon = month;
      tm.tm_mday = day + 1;
      tm.tm_hour = hour;
      tm.tm_min = minute;
      tm.tm_sec = second;
      timegm(&tm);
      ret.day = tm.tm_yday;
      ret.hour = hour;
      ret.minute = minute;
      ret.second = second;
      ret.nanosecond = nanosecond;
      return ret;
    }

    Absolute Absolute::next(const Relative::Year::Day& value) const
    {
      struct tm tm;
      Absolute ret;
      Relative::Year::Day current = *this;
      Relative::Year::Day tmp = value;
      tmp.normalize();
      current.normalize();
      tm.tm_year = year - 1900;
      tm.tm_mon = 0;
      tm.tm_yday = tmp.day;
      tm.tm_hour = hour;
      tm.tm_min = minute;
      tm.tm_sec = second;

      if(current >= tmp)
      {
        tm.tm_year++;
      }

      if(tmp.day == 365)
      {
        while((tm.tm_year % 4) || (!(tm.tm_year % 100) && (tm.tm_year % 400)))
        {
          tm.tm_year++;
        }
      }

      while(tm.tm_yday)
      {
        int nday;

        if(tm.tm_mon == 1)
        {
          if((tm.tm_year % 4) || (!(tm.tm_year % 100) && (tm.tm_year % 400)))
            nday = 29;
          else
            nday = 28;
        }
        else
        {
          if((tm.tm_mon % 2 && tm.tm_mon < 7)
             || (!(tm.tm_mon % 2) && tm.tm_mon >= 7))
            nday = 30;
          else
            nday = 31;
        }

        if(tm.tm_yday < nday)
          break;

        tm.tm_yday -= nday;
        tm.tm_mon++;
      }

      tm.tm_mday = tm.tm_yday + 1;
      timegm(&tm);
      ret.year = tm.tm_year + 1900;
      ret.month = tm.tm_mon;
      ret.day = tm.tm_mday - 1;
      ret.hour = tm.tm_hour;
      ret.minute = tm.tm_min;
      ret.second = tm.tm_sec;
      ret.nanosecond = value.nanosecond;
      return ret;
    }

    Absolute Absolute::previous(const Relative::Year::Day& value) const
    {
      struct tm tm;
      Absolute ret;
      Relative::Year::Day current = *this;
      Relative::Year::Day tmp = value;
      tmp.normalize();
      current.normalize();
      tm.tm_year = year - 1900;
      tm.tm_mon = 0;
      tm.tm_yday = tmp.day;
      tm.tm_hour = hour;
      tm.tm_min = minute;
      tm.tm_sec = second;

      if(current <= tmp)
      {
        tm.tm_year--;
      }

      if(tmp.day == 365)
      {
        while((tm.tm_year % 4) || (!(tm.tm_year % 100) && (tm.tm_year % 400)))
        {
          tm.tm_year--;
        }
      }

      while(tm.tm_yday)
      {
        int nday;

        if(tm.tm_mon == 1)
        {
          if((tm.tm_year % 4) || (!(tm.tm_year % 100) && (tm.tm_year % 400)))
            nday = 29;
          else
            nday = 28;
        }
        else
        {
          if((tm.tm_mon % 2 && tm.tm_mon < 7)
             || (!(tm.tm_mon % 2) && tm.tm_mon >= 7))
            nday = 30;
          else
            nday = 31;
        }

        if(tm.tm_yday < nday)
          break;

        tm.tm_yday -= nday;
        tm.tm_mon++;
      }

      tm.tm_mday = tm.tm_yday + 1;
      timegm(&tm);
      ret.year = tm.tm_year + 1900;
      ret.month = tm.tm_mon;
      ret.day = tm.tm_mday - 1;
      ret.hour = tm.tm_hour;
      ret.minute = tm.tm_min;
      ret.second = tm.tm_sec;
      ret.nanosecond = tmp.nanosecond;
      return ret;
    }

    Absolute::operator Relative::Year::Month() const
    {
      Relative::Year::Month ret;
      struct tm tm;
      ret.month = month;
      ret.day = day;
      ret.hour = hour;
      ret.minute = minute;
      ret.second = second;
      ret.nanosecond = nanosecond;
      return ret;
    }

    Absolute Absolute::next(const Relative::Year::Month& value) const
    {
      struct tm tm;
      Absolute ret;
      Relative::Year::Month current = *this;
      Relative::Year::Month tmp = value;
      tmp.normalize();
      current.normalize();
      tm.tm_year = year - 1900;
      tm.tm_mon = tmp.month;
      tm.tm_mday = tmp.day + 1;
      tm.tm_hour = tmp.hour;
      tm.tm_min = tmp.minute;
      tm.tm_sec = tmp.second;

      if(current >= value)
      {
        tm.tm_year++;
      }

      if(value.month == 1 && value.day == 28)
      {
        while((tm.tm_year % 4) || (!(tm.tm_year % 100) && (tm.tm_year % 400)))
        {
          tm.tm_year++;
        }
      }

      timegm(&tm);
      ret.year = tm.tm_year + 1900;
      ret.month = tm.tm_mon;
      ret.day = tm.tm_mday - 1;
      ret.hour = tm.tm_hour;
      ret.minute = tm.tm_min;
      ret.second = tm.tm_sec;
      ret.nanosecond = tmp.nanosecond;
      return ret;
    }

    Absolute Absolute::previous(const Relative::Year::Month& value) const
    {
      struct tm tm;
      Absolute ret;
      Relative::Year::Month current = *this;
      Relative::Year::Month tmp = value;
      tmp.normalize();
      current.normalize();
      tm.tm_year = year - 1900;
      tm.tm_mon = month;
      tm.tm_mday = tmp.day + 1;
      tm.tm_hour = tmp.hour;
      tm.tm_min = tmp.minute;
      tm.tm_sec = tmp.second;

      if(current <= tmp)
      {
        tm.tm_year--;
      }

      if(tmp.month == 1 && tmp.day == 28)
      {
        while((tm.tm_year % 4) || (!(tm.tm_year % 100) && (tm.tm_year % 400)))
        {
          tm.tm_year--;
        }
      }

      timegm(&tm);
      ret.year = tm.tm_year + 1900;
      ret.month = tm.tm_mon;
      ret.day = tm.tm_mday - 1;
      ret.hour = tm.tm_hour;
      ret.minute = tm.tm_min;
      ret.second = tm.tm_sec;
      ret.nanosecond = tmp.nanosecond;
      return ret;
    }

    void Absolute::normalize()
    {
      uint32_t tmp;
      tmp = nanosecond / 1000000000;
      nanosecond %= 1000000000;
      tmp += second;
      second = tmp % 60;
      tmp /= 60;
      tmp += minute;
      minute = tmp % 60;
      tmp /= 60;
      tmp += hour;
      hour = tmp % 24;
      tmp /= 24;
      tmp += day;
      year += month / 12;
      month %= 12;

      while(tmp)
      {
        if(month == 1)
        {
          if(tmp < 29)
          {
            break;
          }

          tmp -= 29;
        }
        else if((month < 7 && month % 2) || (month >= 7 && !(month % 2)))
        {
          if(tmp < 30)
          {
            break;
          }

          tmp -= 30;
        }
        else
        {
          if(tmp < 31)
          {
            break;
          }

          tmp -= 31;
        }

        if(month < 11)
        {
          month++;
        }
        else
        {
          year++;
          month = 0;
        }
      }

      day = tmp;
    }

    Base::Base()
    {
      nanosecond = 0;
    }

    Base::~Base()
    {
    }

    Base::operator Absolute() const
    {
      Absolute ret;
      ret.year = tm.tm_year + 1900;
      ret.month = tm.tm_mon;
      ret.day = tm.tm_mday - 1;
      ret.hour = tm.tm_hour;
      ret.minute = tm.tm_min;
      ret.second = tm.tm_sec;
      ret.nanosecond = nanosecond;
      return ret;
    }

    Base& Base::operator = (const Absolute& value)
    {
      tm.tm_year = value.year - 1900;
      tm.tm_mon = value.month;
      tm.tm_mday = value.day + 1;
      tm.tm_hour = value.hour;
      tm.tm_min = value.minute;
      tm.tm_sec = value.second;
      nanosecond = value.nanosecond;
      return *this;
    }

    Locale::Locale(const Locale& value)
    {
      operator = (value);
    }

    Locale::Locale(const UTC& value)
    {
      operator = (value);
    }

    Locale::Locale(const Time::Real& value)
    {
      operator = (value);
    }

    Locale::Locale(const Absolute& value)
    {
      operator = (value);
    }

    Locale::~Locale()
    {
    }

    Locale& Locale::operator = (const Locale& value)
    {
      tm = value.tm;
      nanosecond = value.nanosecond;
      return *this;
    }

    Locale& Locale::operator = (const UTC& value)
    {
      struct tm date = value.tm;
      time_t time = timegm(&date);
      localtime_r(&time, &tm);
      nanosecond = value.nanosecond;
      return *this;
    }

    Locale& Locale::operator = (const Time::Real& value)
    {
      Time::Elapsed el = value;
      localtime_r(&el.seconds, &tm);
      nanosecond = el.nanoseconds;
      return *this;
    }

    Locale& Locale::operator = (const Absolute& value)
    {
      Base::operator = (value);
      return *this;
    }

    Locale::operator Time::Real() const
    {
      Time::Real ret;
      Time::Elapsed el;
      struct tm date = tm;
      date.tm_isdst = -1;
      el.seconds = mktime(&date);
      el.nanoseconds = nanosecond;
      ret = el;
      return ret;
    }

    UTC::UTC(const UTC& value)
    {
      operator = (value);
    }

    UTC::UTC(const Locale& value)
    {
      operator = (value);
    }

    UTC::UTC(const Time::Real& value)
    {
      operator = (value);
    }

    UTC::UTC(const Absolute& value)
    {
      operator = (value);
    }

    UTC::~UTC()
    {
    }

    UTC& UTC::operator = (const UTC& value)
    {
      tm = value.tm;
      nanosecond = value.nanosecond;
      return *this;
    }

    UTC& UTC::operator = (const Locale& value)
    {
      struct tm date = value.tm;
      time_t time = mktime(&date);
      gmtime_r(&time, &tm);
      nanosecond = value.nanosecond;
      return *this;
    }

    UTC& UTC::operator = (const Time::Real& value)
    {
      Time::Elapsed el = value;
      gmtime_r(&el.seconds, &tm);
      nanosecond = el.nanoseconds;
      return *this;
    }

    UTC& UTC::operator = (const Absolute& value)
    {
      Base::operator = (value);
      return *this;
    }

    UTC::operator Time::Real() const
    {
      Time::Real ret;
      struct tm date = tm;
      Time::Elapsed el;
      el.seconds = timegm(&date);
      el.nanoseconds = nanosecond;
      ret = el;
      return ret;
    }

  }

}
