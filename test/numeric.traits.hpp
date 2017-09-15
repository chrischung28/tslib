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

#pragma once
#include <limits>
#include <cmath>

// for numeric types
template <typename T> class RNT;

// template specialization for double type
template <> class RNT<double> {
private:
  // to be conformant to R's NA value
  
  // static function
  static double calculate_NA() {
    // create a union. will either hold a double or an unsigned int
    union {
      double value;
      unsigned int word[2];
    };
    // do this b/c don't have a good way to determine
    // endianness at compile time

    // set value to NA
    value = std::numeric_limits<double>::quiet_NaN();

    // store 1954 in either word[0] or word[1] depending on if
    // word[0] is currently 0
    if (word[0] == 0) {
      word[0] = 1954;
    } else {
      word[1] = 1954;
    }
    // return the NA
    return value;
  }

public:
  // has_NA set to true
  static const bool has_NA = true;

  // get the NA value and return it
  static inline double NA() {
    static double na_value = calculate_NA();
    return na_value;
  }

  // function to check if a value is NA
  static inline bool ISNA(double x) { return std::isnan(x); }
};

// generic template for class
template <> class RNT<int> {
public:
  // set has_NA to be true
  static const bool has_NA = true;
  // NA function will return the lowest value possible in an int
  static inline int NA() { return std::numeric_limits<int>::min(); }
  // will check if a value is NA 
  static inline bool ISNA(int x) { return x == std::numeric_limits<int>::min() ? true : false; }
};
