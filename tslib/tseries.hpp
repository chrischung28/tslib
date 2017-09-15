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
#include <iostream>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <tslib/functors.hpp>
#include <tslib/intersection.map.hpp>

namespace tslib {

// date class is a template class
// class backend is typically the vector backend in the vector.backend.hpp
// DatePolicy is usually the GregorianDate
// NT is usually RNT for numeric traits
template <typename IDX, typename V, typename DIM, template <typename, typename, typename> class BACKEND,
          template <typename> class DatePolicy, template <typename> class NT>
class TSeries {
private:
  // constructor for the backend
  // usually is the vector backend
  BACKEND<IDX, V, DIM> tsdata_;

public:
  // define typenames for the backend's iterators for the index and the data
  typedef typename BACKEND<IDX, V, DIM>::const_index_iterator const_index_iterator;
  typedef typename BACKEND<IDX, V, DIM>::index_iterator index_iterator;
  typedef typename BACKEND<IDX, V, DIM>::const_data_iterator const_data_iterator;
  typedef typename BACKEND<IDX, V, DIM>::data_iterator data_iterator;
  // define row iterators to navigate the "matrix"
  typedef typename std::vector<const_data_iterator> const_row_iterator;
  typedef typename std::vector<data_iterator> row_iterator;

  // ctors

  // copy constructor, copy the backend class from the other constructor
  TSeries(const TSeries &T) : tsdata_(T.tsdata_) {}
  // constructor when backend class is given. Copy the backend vector
  TSeries(BACKEND<IDX, V, DIM> &tsdata) : tsdata_{tsdata} {}
  // constructor providing dimensions in order to create the backend vector
  TSeries(DIM nrow, DIM ncol) : tsdata_{nrow, ncol} {}
  // disable move constructor
  TSeries(TSeries &&) = default;

  // accessors

  // get the backend vector
  const BACKEND<IDX, V, DIM> &getBackend() const { return tsdata_; }
  // get the colnames of the backend vector
  const std::vector<std::string> getColnames() const { return tsdata_.getColnames(); }
  // get the number of column names in the backend vector
  const DIM getColnamesSize() const { return tsdata_.getColnamesSize(); }
  // check if column names exist
  const bool hasColnames() const { return getColnamesSize() > 0 ? true : false; }
  // set the column names
  const bool setColnames(const std::vector<std::string> &names) { return tsdata_.setColnames(names); }

  // get the nrow and ncol of the backend vector
  const DIM nrow() const { return tsdata_.nrow(); }
  const DIM ncol() const { return tsdata_.ncol(); }

  // get the iterators for the index (row nums) of the backend vector
  const_index_iterator index_begin() const { return tsdata_.index_begin(); }
  index_iterator index_begin() { return tsdata_.index_begin(); }
  const_index_iterator index_end() const { return tsdata_.index_end(); }
  index_iterator index_end() { return tsdata_.index_end(); }

  // get the column iterators for the index 
  const_data_iterator col_begin(DIM i) const { return tsdata_.col_begin(i); }
  data_iterator col_begin(DIM i) { return tsdata_.col_begin(i); }
  const_data_iterator col_end(DIM i) const { return tsdata_.col_end(i); }
  data_iterator col_end(DIM i) { return tsdata_.col_end(i); }

  // get the row iterator
  // will be base 0
  const_row_iterator getRow(DIM n) const {
    // vector of const row iterators has the same size as the number of columns
    const_row_iterator ans(ncol());

    // for all the columns in the matrix
    for (DIM i = 0; i < ncol(); ++i) {
      // go to the beginning of the column
      const_data_iterator this_col = col_begin(i);
      // move the iterator down times
      std::advance(this_col, n);
      // store this value in the row
      ans[i] = this_col;
    }
    return ans;
  }

  // returns a time series with a lag
  // const qualified so that it will not modify the class
  // passes in a lag number
  TSeries<IDX, V, DIM, BACKEND, DatePolicy, NT> lag(DIM n) const {

    // if ur lag is bigger than the number of rows, throw error
    if (n >= nrow()) { throw std::logic_error("lag: n > nrow of time series."); }
    // create a time series with dimensions of num rows - lag and same number of columns
    // removes forward NAs
    TSeries<IDX, V, DIM, BACKEND, DatePolicy, NT> ans(nrow() - n, ncol());

    // copy over dates

    // get the first row
    const_index_iterator beg{index_begin()};
    // advance the first iterator n times
    std::advance(beg, n);
    // copy from this into the new time series
    std::copy(beg, index_end(), ans.index_begin());

    // copy colnames
    ans.setColnames(getColnames());

    // for each column
    for (DIM i = 0; i < ncol(); ++i) {
      // get starting and ending iterators for the column
      const_data_iterator src_beg{col_begin(i)};
      const_data_iterator src_end{col_end(i)};
      // create a data iterator to be the ans time series iterator at the start of the column
      data_iterator dst_beg{ans.col_begin(i)};
      // move the end of the iterator, go back n times
      std::advance(src_end, -n);
      // copy that into the ans data iterator
      std::copy(src_beg, src_end, dst_beg);
    }
    return ans;
  }

  // operator overloards
  /* compound ops only for scalar ops, self-assignment doesn't make sense when nrow is changing */
  
  // return a time series of common type with the operator +=
  // why no reference to rhs
  // what happens when there is an NA
  template <typename S>
  TSeries<IDX, typename std::common_type<V, S>::type, DIM, BACKEND, DatePolicy, NT> &operator+=(S rhs) {
    // for each column
    for (DIM i = 0; i < ncol(); ++i) {
      // for each column iterator pair
      for (auto iter = col_begin(i); iter != col_end(i); ++iter) {
        // if the iterator is not an NA, then add rhs 
        if (!NT<V>::ISNA(*iter)) { *iter += rhs; }
      }
    }
    // return this time series
    return *this;
  }

  // same thing but for subtract
  template <typename S>
  TSeries<IDX, typename std::common_type<V, S>::type, DIM, BACKEND, DatePolicy, NT> &operator-=(S rhs) {
    for (DIM i = 0; i < ncol(); ++i) {
      for (auto iter = col_begin(i); iter != col_end(i); ++iter) {
        if (!NT<V>::ISNA(*iter)) { *iter -= rhs; }
      }
    }
    return *this;
  }

  // same thing but for multiply
  template <typename S>
  TSeries<IDX, typename std::common_type<V, S>::type, DIM, BACKEND, DatePolicy, NT> &operator*=(S rhs) {
    for (DIM i = 0; i < ncol(); ++i) {
      for (auto iter = col_begin(i); iter != col_end(i); ++iter) {
        if (!NT<V>::ISNA(*iter)) { *iter *= rhs; }
      }
    }
    return *this;
  }

  // same thing but for divide
  template <typename S>
  TSeries<IDX, typename std::common_type<V, S>::type, DIM, BACKEND, DatePolicy, NT> &operator/=(S rhs) {
    for (DIM i = 0; i < ncol(); ++i) {
      for (auto iter = col_begin(i); iter != col_end(i); ++iter) {
        if (!NT<V>::ISNA(*iter)) { *iter /= rhs; }
      }
    }
    return *this;
  }
};

// ostream << operator overload, needs an ostream as well as a time series
template <typename IDX, typename V, typename DIM, template <typename, typename, typename> class BACKEND,
          template <typename> class DatePolicy, template <typename> class NT>
std::ostream &operator<<(std::ostream &os, const TSeries<IDX, V, DIM, BACKEND, DatePolicy, NT> &ts) {
  // get the column names from the time series
  std::vector<std::string> cnames(ts.getColnames());

  // if there are column names
  if (cnames.size()) {
    // shift out to be in line w/ first column of values (space is for dates column)
    os << "\t";
    // print out column names
    for (auto name : cnames) { os << name << " "; }
  }

  // set const iterators for the index's start and end
  typename TSeries<IDX, V, DIM, BACKEND, DatePolicy, NT>::const_index_iterator idx{ts.index_begin()};
  typename TSeries<IDX, V, DIM, BACKEND, DatePolicy, NT>::const_index_iterator idx_end{ts.index_end()};
  // vector of first row of iterators of the df
  typename TSeries<IDX, V, DIM, BACKEND, DatePolicy, NT>::const_row_iterator row_iter(ts.getRow(0));

  // as long as the start iterator does not equal the end, increment it
  for (; idx != idx_end; ++idx) {

    // convert the date policy to string
    os << DatePolicy<IDX>::toString(*idx, "%Y-%m-%d %T") << "\t";
    // for each value in the row
    for (auto &value : row_iter) {
      // if NA, print NA
      if (NT<V>::ISNA(*value)) {
        os << "NA";
      } else { 
        // else print the value
        os << *value << " ";
      }
      // print a space (spacing not equal as NA do not have extra space)
      os << " ";
      ++value; // next row
    }
    os << "\n"; // no flush
  }
  return os;
}

// template function binary_opp that takes in 2 time series
// class pred seems to be the functor
template <template <typename, typename> class Pred, typename IDX, typename U, typename V, typename DIM,
          template <typename, typename, typename> class BACKEND, template <typename> class DatePolicy,
          template <typename> class NT>
auto binary_opp(const TSeries<IDX, U, DIM, BACKEND, DatePolicy, NT> &lhs,
                const TSeries<IDX, V, DIM, BACKEND, DatePolicy, NT> &rhs) {
  
  // define common type of the 2 time series
  typedef typename std::common_type<U, V>::type RV;
  // create class Pred (functor)
  Pred<U, V> pred;

  // make sure number of columns match and that there must be at least 1 column
  if (lhs.ncol() != rhs.ncol() && lhs.ncol() != 1 && rhs.ncol() != 1) {
    throw std::logic_error("Number of colums must match. or one time series must be a single column.");
  }
  // gets a vector of pairs showing the intrsection points where the values in the pairs are the distance from the beginning
  const auto rowmap{intersection_map(lhs.index_begin(), lhs.index_end(), rhs.index_begin(), rhs.index_end())};
  // print the number of intersections
  std::cout << rowmap.size() << std::endl;
  // for each record in the map, print it out
  for (auto m : rowmap) { std::cout << m.first << ":" << m.second << std::endl; }

  // FIXME: use Pred<U,V>::RT to define the return type

  // create a time series that uses the functors type with the number of rows being the size of the row map and the ncol being the max number of columns
  TSeries<IDX, typename Pred<U, V>::RT, DIM, BACKEND, DatePolicy, NT> res(rowmap.size(), std::max(lhs.ncol(), rhs.ncol()));

  // set colnames from larger of two args but prefer lhs
  if (lhs.getColnamesSize() >= rhs.getColnamesSize()) {
    // if lhs has more than or the same amount of colnames than the right, 
    // use the lhs names as the column names
    res.setColnames(lhs.getColnames());
  } else if (rhs.hasColnames()) {
    // if not true above and rhs has column names, then set the col names to right hand side
    res.setColnames(rhs.getColnames());
  }

  // set index vector from lhs 
  auto idx{res.index_begin()};
  // get the const index vector for the lhs begin
  const auto lhs_idx{lhs.index_begin()};
  for (auto m : rowmap) {
    // for every record in the map, find the index
    *idx = lhs_idx[m.first];
    // increment the iterator in idx to move to the next index point
    idx++;
  }

  // for all the columns
  for (DIM nc = 0; nc < res.ncol(); ++nc) {
    // get the 2 column iterators at the beginning
    // don't think there is a check in place if nc goes out of range of the columns
    const auto lhs_col{lhs.col_begin(nc)}, rhs_col{rhs.col_begin(nc)};
    // get the iterator for the resulting time series at the appropriate column
    auto res_col{res.col_begin(nc)};

    // for every record
    for (auto m : rowmap) {
      // get the values from the left and right hand side
      U lhs_val{lhs_col[m.first]};
      V rhs_val{rhs_col[m.second]};
      // have the resultant iterator be NA if either values are NA, or the functor's result
      *res_col = NT<V>::ISNA(lhs_val) || NT<U>::ISNA(rhs_val) ? NT<RV>::NA() : pred(lhs_val, rhs_val);
      // increment the iterator
      ++res_col;
    }
  }
  return res;
}

// overloaded operators for standard functors
template <typename IDX, typename V, typename U, typename DIM, template <typename, typename, typename> class BACKEND,
          template <typename> class DatePolicy, template <typename> class NT>
TSeries<IDX, typename std::common_type<V, U>::type, DIM, BACKEND, DatePolicy, NT>
operator+(const TSeries<IDX, V, DIM, BACKEND, DatePolicy, NT> &lhs,
          const TSeries<IDX, U, DIM, BACKEND, DatePolicy, NT> &rhs) {
  return binary_opp<PlusFunctor>(lhs, rhs);
}

template <typename IDX, typename V, typename U, typename DIM, template <typename, typename, typename> class BACKEND,
          template <typename> class DatePolicy, template <typename> class NT>
TSeries<IDX, typename std::common_type<V, U>::type, DIM, BACKEND, DatePolicy, NT>
operator-(const TSeries<IDX, V, DIM, BACKEND, DatePolicy, NT> &lhs,
          const TSeries<IDX, U, DIM, BACKEND, DatePolicy, NT> &rhs) {
  return binary_opp<MinusFunctor>(lhs, rhs);
}

template <typename IDX, typename V, typename U, typename DIM, template <typename, typename, typename> class BACKEND,
          template <typename> class DatePolicy, template <typename> class NT>
TSeries<IDX, typename std::common_type<V, U>::type, DIM, BACKEND, DatePolicy, NT>
operator*(const TSeries<IDX, V, DIM, BACKEND, DatePolicy, NT> &lhs,
          const TSeries<IDX, U, DIM, BACKEND, DatePolicy, NT> &rhs) {
  return binary_opp<MultiplyFunctor>(lhs, rhs);
}

template <typename IDX, typename V, typename U, typename DIM, template <typename, typename, typename> class BACKEND,
          template <typename> class DatePolicy, template <typename> class NT>
TSeries<IDX, typename std::common_type<V, U>::type, DIM, BACKEND, DatePolicy, NT>
operator/(const TSeries<IDX, V, DIM, BACKEND, DatePolicy, NT> &lhs,
          const TSeries<IDX, U, DIM, BACKEND, DatePolicy, NT> &rhs) {
  return binary_opp<DivideFunctor>(lhs, rhs);
}

} // namespace tslib
