/*
 * Twilight.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_TWILIGHT_H_
#define OVERKIZ_TWILIGHT_H_

#include <kizbox/framework/core/Date.h>
#include <map>

namespace Overkiz
{

  class Twilight
  {
  public:

#define DEFAULT_ANGLE CIVIL

    enum
    {
      DAWN, DUSK,
    } Type;

    typedef enum
    {
      SOLAR,
      CIVIL,
      NAUTICAL,
      ASTRONOMICAL,
      MAX
    } Angle;

    static const std::map<const std::string,const Twilight::Angle> AngleMap;

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
                          double longitude, const Angle angle=DEFAULT_ANGLE);

    static Date::UTC dusk(const Date::Absolute& time, double latitude,
                          double longitude, const Angle angle=DEFAULT_ANGLE);

    static const Twilight::Angle getAngleFromString(const std::string& angle);

    static const std::string getStringFromAngle(const Twilight::Angle angle);

  private:

    static double solarDeclination(const Date::Relative::Year::Day& year);

    static double equationOfTime(const Date::Relative::Year::Day& year);

    static double sunHourAngle(const Date::Relative::Year::Day& year,
                               double lat, const Angle angle);

    static void convert(Date::Absolute& date, double result);
  };

}

#endif /* OVERKIZ_TWILIGHT_H_ */
