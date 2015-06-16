/*
 * Twilight.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <math.h>
#include <stdio.h>

#include "Twilight.h"

namespace Overkiz
{
#define SUN_OBLIQUITY           0.40910
#define CIVIL_TWILIGHT_ANGLE    (-6.0*M_PI/180)
#define TWILIGHT_ANGLE          CIVIL_TWILIGHT_ANGLE
#define E                       0.01671
#define C1                      (E*2 - E*E*E/4)
#define C2                      (E*E*5/4)
#define C3                      (E*E*E*13/12)
#define Y                       tan(SUN_OBLIQUITY/2)
#define R1                      (-Y*Y)
#define R2                      (Y*Y*Y*Y/2)
#define R3                      (-Y*Y*Y*Y*Y*Y/3)

  double Twilight::solarDeclination(const Date::Relative::Year::Day& year)
  {
    double M = M_PI / 180 * (357.5291 + 0.98560028 * (year.day + 1));
    double C = (C1 * sin(M)) + (C2 * sin(2 * M)) + (C3 * sin(3 * M));
    double L = M_PI / 180 * (280.4665 + 0.98564736 * (year.day + 1)) + C;
    return asin(sin(SUN_OBLIQUITY) * sin(L));
  }

  double Twilight::equationOfTime(const Date::Relative::Year::Day& year)
  {
    double M = M_PI / 180 * (357.5291 + 0.98560028 * (year.day + 1));
    double C = C1 * sin(M) + C2 * sin(2 * M) + C3 * sin(3 * M);
    double L = M_PI / 180 * (280.4665 + 0.98564736 * (year.day + 1)) + C;
    double R = R1 * sin(2 * L) + R2 * sin(4 * L) + R3 * sin(6 * L);
    return 4 * (C + R) * 180 / M_PI;
  }

  double Twilight::sunHourAngle(const Date::Relative::Year::Day& year,
                                double latitude)
  {
    double solarDec = solarDeclination(year);
    double q = (TWILIGHT_ANGLE - sin(solarDec) * sin(latitude))
               / (cos(solarDec) * cos(latitude));

    if(q < -1 || q > 1)
      throw Invalid();

    return acos(q);
  }

  void Twilight::convert(Date::Absolute& date, double result)
  {
    Time::Real t;
    Time::Elapsed el;
    Date::UTC utc;
    time_t tmp;
    date.hour = 0;
    date.minute = 0;
    date.second = 0;
    date.nanosecond = 0;
    utc = date;
    t = utc;
    el = t;
    result *= 3600;
    tmp = result;
    el.seconds += tmp;
    result -= tmp;
    result *= 1000000000;
    tmp = result;
    el.nanoseconds += tmp;
    t = el;
    utc = t;
    date = utc;
  }

  Date::UTC Twilight::dawn(const Date::Absolute& date, double latitude,
                           double longitude)
  {
    Date::Absolute ret = date;
    Date::Relative::Year::Day year = date;
    double sHA = sunHourAngle(year, latitude * M_PI / 180);
    double ET = equationOfTime(year);
    double result = 12.0 - sHA * 180 / 15 / M_PI + ET / 60 - longitude / 15;
    convert(ret, result);
    return ret;
  }

  Date::UTC Twilight::dusk(const Date::Absolute& date, double latitude,
                           double longitude)
  {
    Date::Absolute ret = date;
    Date::Relative::Year::Day year = date;
    double sHA = sunHourAngle(year, latitude * M_PI / 180);
    double ET = equationOfTime(year);
    double result = 12 + sHA * 180 / 15 / M_PI + ET / 60 - longitude / 15;
    convert(ret, result);
    return ret;
  }

}
