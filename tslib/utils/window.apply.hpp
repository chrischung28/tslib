///////////////////////////////////////////////////////////////////////////
// Copyright (C) 2008  Whit Armstrong                                    //
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

#ifndef WINDOW_APPLY_HPP
#define WINDOW_APPLY_HPP

#include <iterator>
#include <tslib/utils/numeric.traits.hpp>

namespace tslib {
  // template class that needs a return type and another templated class (functor?)
  template<typename ReturnType,
	   template<class> class F>
  class windowApply {
  public:
    // templated static function to apply
    // U are iterators of the data 
    // T is an iterator of the result
    template<typename T, typename U>
    static inline void apply(T ans, U beg, U end, const size_t window) {
      // advance the beginning iterator by the window -1
      std::advance(beg, window - 1);
      // while beginning does not equal end
      while(beg != end) {
        // use the functors apply to work on the current iterator location - (window-1) and the next iterator location
        // ie. window size 5, beg starts at 0, increment 4 times, so beg is not at 4
        // beg - (window-1) is 0 and beg + 1 is 5, use the beg+1 to be the ending vector
        *ans = F<ReturnType>::apply( beg - (window - 1), beg+1);
        // increment iterators
        ++beg;
        ++ans;
      }
    }
  };
} // namespace tslib

#endif // WINDOW_APPLY_HPP
