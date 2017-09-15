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

namespace tslib {

// templated functors
//  have the common type of X and Y be referred to as RT
// return operations +, -, *, and /

template <typename X, typename Y> class PlusFunctor {
public:
  typedef typename std::common_type<X, Y>::type RT;
  RT operator()(const X &x, const Y &y) { return x + y; }
};

template <typename X, typename Y> class MinusFunctor {
public:
  typedef typename std::common_type<X, Y>::type RT;
  RT operator()(const X &x, const Y &y) { return x - y; }
};

template <typename X, typename Y> class MultiplyFunctor {
public:
  typedef typename std::common_type<X, Y>::type RT;
  RT operator()(const X &x, const Y &y) { return x * y; }
};

template <typename X, typename Y> class DivideFunctor {
public:
  typedef typename std::common_type<X, Y>::type RT;
  RT operator()(const X &x, const Y &y) { return x / y; }
};

} // namespace tslib
