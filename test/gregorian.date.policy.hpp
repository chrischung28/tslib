///////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016  Whit Armstrong                                    //
//                                                                       //
// This program is free software: you can redistribute it and/or modify  //
// it under the terms of the GNU General Public License as published by  //
// the Free Software Foundation, either version 3 of the License, or     //
// (at your option) any later version.                                   //
//                                                                       //
// This program is distributed in the hope that it will be useful,       //
// but WITHOUT ANY WARRANTY; without even the implied warranty of        //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
// GNU General Public License for more details.                          //
//                                                                       //
// You should have received a copy of the GNU General Public License     //
// along with this program.  If not, see <http://www.gnu.org/licenses/>. //
///////////////////////////////////////////////////////////////////////////

// cause current source file to be included only once in a single compiliation
#pragma once
#include <sstream>
//#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

// template class to take in the type
template <typename T> class GregorianDate {
private:
  // static const boost::gregorian::date fromRDate(const T x) { return boost::gregorian::date(1970,1,1) +
  // boost::gregorian::date_duration(x); }

  // seems like RDates are ints that use 1970-01-01 as epoch

  // static function to return the boost date
  // input number of days and it gets you the date since epoch for that number of days
  static const boost::gregorian::date fromRDate(const T x) {
    return boost::gregorian::date(1970, 1, 1) + boost::gregorian::days(x);
  }
  // static function to return the number of days since epoch after inserting a date
  static const T toRDate(const boost::gregorian::date x) {
    return static_cast<int>(boost::gregorian::date_period(boost::gregorian::date(1970, 1, 1), x).length().days());
  }

public:
  // static const T toDate(const char* date, const char* format);

  // static function to convert to days since epoch with inputs from ms
  static const T toDate(const int year, const int month, const int day, const int hour = 0, const int minute = 0,
                        const int second = 0, const int millisecond = 0) {
    return toRDate(boost::gregorian::date(year, month, day));
  }
  // static function to get a string
  // inputs number of days since epoch - const char format seems to be useless, does not seem to be doing much
  // returns the string of the date
  static const std::string toString(const T x, const char *format) {
    std::stringstream ss;
    ss << fromRDate(x);
    return ss.str();
  }

  // static functions to return the second, minute, hour all as 0
  static const int second(const T x) { return 0; }
  static const int minute(const T x) { return 0; }
  static const int hour(const T x) { return 0; }
  // return day of week
  static const int dayofweek(const T x) { return static_cast<int>(fromRDate(x).day_of_week()); }
  // return day of month
  static const int dayofmonth(const T x) { return static_cast<int>(fromRDate(x).day()); }
  // return the month
  static const int month(const T x) { return static_cast<int>(fromRDate(x).month()); }
  // return the year
  static const int year(const T x) { return static_cast<int>(fromRDate(x).year()); }
  //return the last day of the month
  static const int last_day_of_month(const T x) { return toRDate(fromRDate(x).end_of_month()); }
  // return the number of days since epoch when providing a date and the number of years to increase by
  static const T AddYears(const T x, const int n) { return toRDate(fromRDate(x) + boost::gregorian::years(n)); }
  // return the number of days since epoch when providing a date and the number of months to increase by 
  static const T AddMonths(const T x, const int n) { return toRDate(fromRDate(x) + boost::gregorian::months(n)); }
  // return the number of days since epoch when providing a date and the number of days to increase by 
  static const T AddDays(const T x, const int n) { return toRDate(fromRDate(x) + boost::gregorian::days(n)); }
  // calculate the number of days between 2 days since epoch
  static const double daily_distance(const T x, const T y) { return x - y; }
};
