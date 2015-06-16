/*
 * Twilight.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_TWILIGHT_H_
#define OVERKIZ_TWILIGHT_H_

#include <kizbox/framework/core/Date.h>

namespace Overkiz
{

  class Twilight
  {
  public:

    enum
    {
      DAWN, DUSK,
    } Type;

    class Invalid: public Exception
    {
    public:
      Invalid()
      {
      }

      virtual ~Invalid()
      {
      }

      const char *getId() const
      {
        return "com.kizbox.Framework.Core.Twilight.Invalid";
      }
    };

    static Date::UTC dawn(const Date::Absolute& date, double latitude,
                          double longitude);

    static Date::UTC dusk(const Date::Absolute& time, double latitude,
                          double longitude);
  private:

    static double solarDeclination(const Date::Relative::Year::Day& year);

    static double equationOfTime(const Date::Relative::Year::Day& year);

    static double sunHourAngle(const Date::Relative::Year::Day& year,
                               double lat);

    static void convert(Date::Absolute& date, double result);
  };

}

#endif /* OVERKIZ_TWILIGHT_H_ */
