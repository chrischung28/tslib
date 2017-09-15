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
#include <algorithm>
#include <utility>
#include <vector>

namespace tslib {

// templated function that takes in 4 iterators (2 begins and 2 ends) of time series' indices
// finds the locations in the 2 vectors that intersect
template <typename T> std::vector<std::pair<size_t, size_t>> intersection_map(T xbeg, T xend, T ybeg, T yend) {
  // max size of intersection is the smaller of the two series
  typename std::iterator_traits<T>::difference_type max_size{
      std::min(std::distance(xbeg, xend), std::distance(ybeg, yend))};
  // create a vector of pairs of 2 sizes call res    
  std::vector<std::pair<size_t, size_t>> res;
  // reserve space for the maximum inputs
  res.reserve(max_size);
  // create iterators be the 2 beginning iterators
  T xp{xbeg}, yp{ybeg};

  // while the 2 iterators are not both at the end
  while (xp != xend && yp != yend) {
    // if the index of x is smaller than the index of y, increment x
    if (*xp < *yp) {
      ++xp;
    } else if (*yp < *xp) {
      // if the index of y is smaller than x, increment y
      ++yp;
    } else {
      // if they are equal, add to the res the distance between the beginning and the current iterator location
      res.push_back(std::make_pair(std::distance(xbeg, xp), std::distance(ybeg, yp)));
      // increment both x and y
      ++xp;
      ++yp;
    }
  }
  return res;
}

} // namespace tslib
