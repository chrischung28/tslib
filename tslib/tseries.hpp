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

#ifndef TSERIES_HPP
#define TSERIES_HPP

#include <set>
#include <cstdlib>
#include <algorithm>
#include <numeric>
#include <functional>
#include <iterator>
#include <stdexcept>

#include <tslib/tseries.data.hpp>
#include <tslib/range.specifier/rangeSpecifier.hpp>
#include <tslib/ts.opps/ts.opps.hpp>
#include <tslib/utils/window.apply.hpp>
#include <tslib/utils/window.function.hpp>
#include <tslib/utils/breaks.hpp>
#include <tslib/vector.transform.hpp>
#include <tslib/date.policies/posix.date.policy.hpp>
#include <tslib/date.policies/julian.date.policy.hpp>
#include <tslib/date.policies/date.partition.hpp>

namespace tslib {
  // to handle run time errors from the TSerries class
  class TSeriesError : public std::runtime_error {
  public:
    // send the error message 
    TSeriesError(const std::string& msg = "") : std::runtime_error(msg) {}
  };
  // templated class with a typename for Date, Data, Dimensions, needs a backend class, as well as a Date Policy
  template <typename TDATE, typename TDATA,
            typename TSDIM,
            template<typename,typename,typename> class TSDATABACKEND,
            template<typename> class DatePolicy>
  class TSeries {
  private:
    // create a backend class to store all the data
    TSDATABACKEND<TDATE,TDATA,TSDIM> tsdata_;
    // create a function to do an offset to get the proper point in the "matrix"
    const TSDIM offset(const TSDIM row, const TSDIM col) const {
      return row + col*nrow();
    }
  public:
    // ctors dtors

    // default constructors
    TSeries();
    // construct that takes in a backend vector
    TSeries(TSDATABACKEND<TDATE,TDATA,TSDIM>& tsdata);
    // constructor that takes in dimensions for the time series
    TSeries(const TSDIM rows, const TSDIM cols);
    // copy constructor for the time series
    TSeries(const TSeries& T);
    // constructor that takes in a pointer to arrays of data and dates, provide nrows, ncols, and the release conditions
    TSeries(TDATA* external_data, TDATE* external_dates, const TSDIM nrows, const TSDIM ncols, const bool release);

    // accessors
    // access private data
    const TSDATABACKEND<TDATE,TDATA,TSDIM>* getIMPL() const;
    std::vector<std::string> getColnames() const;
    const TSDIM nrow() const;
    const TSDIM ncol() const;
    TDATA* getData() const;
    TDATE* getDates() const;

    // mutators
    // set the column names
    bool setColnames(const std::vector<std::string>& cnames);

    // transforms
    // need a window size
    template<typename ReturnType, template<class> class F>
    const TSeries<TDATE,ReturnType,TSDIM,TSDATABACKEND,DatePolicy> window(const size_t window) const;
    // need time window
    template<typename ReturnType, template<class> class F, template<class, template<typename> class> class PFUNC>
    const TSeries<TDATE,ReturnType,TSDIM,TSDATABACKEND,DatePolicy> time_window(const int n = 1) const;
    // transform function
    template<typename ReturnType, template<class> class F>
    const TSeries<TDATE,ReturnType,TSDIM,TSDATABACKEND,DatePolicy> transform() const;
    // transform 1 argument 
    template<typename ReturnType, template<class> class F, typename T>
    const TSeries<TDATE,ReturnType,TSDIM,TSDATABACKEND,DatePolicy> transform_1arg(T arg1) const;

    // frequency conversion (only highfreq to lowfreq conversion)
    template<template<class, template<typename> class> class PFUNC>
    const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> freq(const int n = 1) const;

    // subsets
    template<typename T>
    const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> row_subset(T beg, T end) const;

    // pad
    template<typename T>
    const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> pad(T beg, T end) const;

    //operators
    // assignment operator with a time series
    TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>& operator=(const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>& x);
    // assignment operator with data
    TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>& operator=(const TDATA x);

    // lag lead
    const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> operator() (const TSDIM n) const;
    const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> lag(const TSDIM n) const;
    const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> lead(const TSDIM n) const;
    // difference
    const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> diff(const TSDIM n) const;

    // matrix index
    // value
    const TDATA operator() (const TSDIM row, const TSDIM col) const;
    // reference value
    TDATA& operator() (const TSDIM row, const TSDIM col);
    // friend function for std::ostream << operator (templace speciailiations started)
    friend std::ostream& operator<< <> (std::ostream& os, const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>& ts);
  };

  // default constructor just creates
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::TSeries() {}

  // constructor that provides the data has the backend vector copy the data
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::TSeries(TSDATABACKEND<TDATE,TDATA,TSDIM>& tsdata): tsdata_(tsdata) {}

  // constructor that sets the backend vector to have the dimensions specified
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::TSeries(const TSDIM rows, const TSDIM cols): tsdata_(rows,cols) {}

  // when a time series is copied, only provide the backend vector
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::TSeries(const TSeries& T): tsdata_(T.tsdata_) {}
  // using the data provided, load up a new backend vector
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::TSeries(TDATA* external_data, TDATE* external_dates, const TSDIM nrows, const TSDIM ncols, const bool release):
    tsdata_(external_data, external_dates, nrows, ncols, release) {}

  // get the backend vector
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  inline
  const TSDATABACKEND<TDATE,TDATA,TSDIM>* TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::getIMPL() const {
    return &tsdata_;
  }
  // assignment operator with a time series
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>& TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::operator=(const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>& rhs) {

    // self assignment
    // return this current one
    if(this == &rhs) {
      return *this;
    }
    // assign backend vector to new data and return
    tsdata_ = rhs.tsdata_;
    return *this;
  }
  // asisgnment operator with a backend data
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>& TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::operator=(const TDATA rhs) {
    // get the data from the current pointer
    TDATA* data = getData();

    // for all the values, set to rhs
    for(TSDIM i = 0; i < nrow()*ncol(); i++) {
      data[i] = rhs;
    }
    return *this;
  }
  // lag lead overloaded operator () where a dimension is provided
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::operator() (const TSDIM n) const {
    // if  n is bigger than 0, it is a lag, if not, it is a lead
    if( n > 0 ) {
      return lag(n);
    } else if( n < 0) {
      return lead(abs(n));
    }
    return *this;
  }
  // lag function
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::lag(const TSDIM n) const {
    // must have have a lag value that is less than the number of rows
    if(n >= nrow()) { throw std::logic_error("lag: n > nrow of time series."); }
    // new size of the pointer array is the number of rows subtracted by n
    const TSDIM new_size = nrow() - n;
    // create a new time series with this new size and the same number of columns
    TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> ans(new_size, ncol());
    // get the data array of the new time series
    TDATA* ans_data = ans.getData();
    // get the data array of the current data series
    const TDATA* data = getData();

    // copy over dates
    // get the dates that are after the lag and the size of the new vector
    std::copy(getDates() + n, getDates() + n + new_size, ans.getDates());

    // set new colnames for the new time series
    ans.setColnames(getColnames());

    // for each column
    for(TSDIM c = 0; c < ncol(); c++) {
      // copy the data with its relevant size to the new time series
      std::copy(data, data + new_size, ans_data);
      // increment the pointer to the next 'column'
      ans_data += ans.nrow();
      data += nrow();
    }
    return ans;
  }

  // lead function
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::lead(const TSDIM n) const {
    // for lead, the leading number must have less than the number of rows
    if(n >= nrow()) { throw std::logic_error("lead: n > nrow of time series."); }
    // calculate the new size of the data
    const TSDIM new_size = nrow() - n;
    // create a new time series
    TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> ans(new_size, ncol());
    // gets the array of data of the new time series and the current time series
    TDATA* ans_data = ans.getData();
    const TDATA* data = getData();

    // copy over dates
    // get the appropriate dates for the lead and set it in the new time series
    std::copy(getDates(), getDates() + new_size, ans.getDates());

    // set new colnames for the new time series
    ans.setColnames(getColnames());
    // for each column
    for(TSDIM c = 0; c < ncol(); c++) {
      // copy the leading data into the new time series
      std::copy(data + n, data + n + new_size, ans_data);
      // increment the pointer to the next column
      ans_data += ans.nrow();
      data += nrow();
    }
    return ans;
  }

  // diff function to get the difference
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::diff(const TSDIM n) const {
    // must have the dim specified as less than the number of rows
    if(n >= nrow()) { throw std::logic_error("diff: n > nrow of time series."); }
    // get the new size of the data
    const TSDIM new_size = nrow() - n;
    // create new time series
    TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> ans(new_size, ncol());
    // get the data from the new and old time series
    TDATA* ans_data = ans.getData();
    const TDATA* data = getData();

    // copy over dates
    // get the dates that are shifted by n entries
    std::copy(getDates() + n, getDates() + n + new_size, ans.getDates());

    // set new colnames for the new timeseries
    ans.setColnames(getColnames());

    // for each column
    for(TSDIM c = 0; c < ncol(); c++) {
      // for every row starting at the original date record
      for(TSDIM r = n; r < nrow(); r++) {
        // if the data is NA at the original data point or the data point n records back, set the record to be NA
        if(numeric_traits<TDATA>::ISNA(data[r]) || numeric_traits<TDATA>::ISNA(data[r-n])) {
          ans_data[r-n] = numeric_traits<TDATA>::NA();
        } else {
          // set the record to be the difference between the current days value and the record n records back
          ans_data[r-n] = data[r] - data[r-n];
        }
      }
      ans_data += ans.nrow();
      data += nrow();
    }
    return ans;
  }

  // const selector using overloaded operator (row, col) to get individual value
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  const TDATA TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::operator() (const TSDIM row, const TSDIM col) const {
    // get the data array
    const TDATA* data = getData();
    // calculate the index and return the appropriate value
    return data[offset(row, col)];
  }
  // selector that can be modified to get individual value
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  TDATA& TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::operator() (const TSDIM row, const TSDIM col) {
    TDATA* data = getData();
    return data[offset(row, col)];
  }
  // get the dates array
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  inline
  TDATE* TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::getDates() const {
    return tsdata_.getDates();
  }
  // get the data array
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  inline
  TDATA* TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::getData() const {
    return tsdata_.getData();
  }

  // get the number of rows
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  inline
  const TSDIM TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::nrow() const {
    return tsdata_.nrow();
  }
  // get the number oc columns
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  inline
  const TSDIM TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::ncol() const {
    return tsdata_.ncol();
  }
  // get the column names
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  inline
  std::vector<std::string> TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::getColnames() const {
    return tsdata_.getColnames();
  }
  // set the column names
  // returns if setting the column names was successful
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  bool TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::setColnames(const std::vector<std::string>& cnames) {
    // if there are column names and the number of names is the same as the number of columns
    // set the column names
    if(cnames.size() > 0 && static_cast<TSDIM>(cnames.size()) == ncol()) {
      tsdata_.setColnames(cnames);
      return true; // success
    } else {
      return false; // failure
    }
  }

  // window function 
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  template<typename ReturnType, template<class> class F>
  const TSeries<TDATE,ReturnType,TSDIM,TSDATABACKEND,DatePolicy> TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::window(const size_t window) const {

    // allocate new time series which has the same number of columns, but the number of rows is the original number of rows - (window size - 1)
    TSeries<TDATE,ReturnType,TSDIM,TSDATABACKEND,DatePolicy> ans(nrow() - (window - 1), ncol());

    // copy over dates
    // copy the dates that are within this window
    std::copy(getDates() + (window - 1), getDates()+nrow(), ans.getDates());

    // set new colnames for the new time series
    ans.setColnames(getColnames());
    // get the data from the new time series and the current time series
    ReturnType* ans_data = ans.getData();
    TDATA* data = getData();
    // for every column 
    for(TSDIM col = 0; col < ncol(); col++) {
      // apply functions apply function on the column
      windowApply<ReturnType,F>::apply(ans_data, data, data + nrow(), window);
      // increment the iterators to the next column
      ans_data += ans.nrow();
      data += nrow();
    }
    return ans;
  }

  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  template<typename ReturnType, template<class> class F, template<class, template<typename> class> class PFUNC>
  const TSeries<TDATE,ReturnType,TSDIM,TSDATABACKEND,DatePolicy> TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::time_window(const int n) const {
    // pre-allocate vector for transformed dates
    typename std::vector<TDATE> partitions;
    partitions.resize(nrow());
    TDATE* dts = getDates();
    // transform dates
    for(TSDIM row = 0; row < nrow(); row++) {
      partitions[row] = PFUNC<TDATE, DatePolicy>()(dts[row],n);
    }
    //std::transform(getDates(), getDates() + nrow(), partitions.begin(), std::bind2nd(PFUNC<TDATE, DatePolicy>(), n));
    // vector for selected rows
    std::vector<TSDIM> ans_rows;
    breaks(partitions.begin(),partitions.end(),std::back_inserter(ans_rows));

    TSeries<TDATE,ReturnType,TSDIM,TSDATABACKEND,DatePolicy> ans(ans_rows.size(), ncol());
    ans.setColnames(getColnames());
    TDATE* dates = getDates();
    TDATE* ans_dates = ans.getDates();
    for(size_t i = 0; i < ans_rows.size(); i++) {
      ans_dates[i] = dates[ans_rows[i]];
    }

    ReturnType* ans_data = ans.getData();
    TDATA* data = getData();
    for(TSDIM ans_col = 0; ans_col < ans.ncol(); ans_col++, data+=nrow()) {
      size_t range_start = 0;
      for(size_t i = 0; i < ans_rows.size(); i++) {
        ans_data[ans.offset(i, ans_col)] = F<ReturnType>::apply(data + range_start, data + ans_rows[i] + 1);
        range_start = ans_rows[i] + 1;
      }
    }
    return ans;
  }

  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  template<typename ReturnType, template<class> class F>
  const TSeries<TDATE,ReturnType,TSDIM,TSDATABACKEND,DatePolicy> TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::transform() const {

    // allocate new answer
    TSeries<TDATE,ReturnType,TSDIM,TSDATABACKEND,DatePolicy> ans(nrow(), ncol());

    // copy over dates
    std::copy(getDates(),getDates()+nrow(),ans.getDates());

    // set new colnames
    ans.setColnames(getColnames());

    ReturnType* ans_data = ans.getData();
    TDATA* data = getData();

    for(TSDIM col = 0; col < ncol(); col++) {
      F<ReturnType>::apply(ans_data, data, data + nrow());
      ans_data += ans.nrow();
      data += nrow();
    }
    return ans;
  }
  // ReturnType corresponds to TData in time series
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  template<typename ReturnType, template<class> class F, typename T>
  const TSeries<TDATE,ReturnType,TSDIM,TSDATABACKEND,DatePolicy> TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::transform_1arg(T arg1) const {

    // allocate new answer
    TSeries<TDATE,ReturnType,TSDIM,TSDATABACKEND,DatePolicy> ans(nrow(), ncol());

    // copy over dates
    std::copy(getDates(),getDates()+nrow(),ans.getDates());

    // set new colnames
    ans.setColnames(getColnames());

    ReturnType* ans_data = ans.getData();
    TDATA* data = getData();

    for(TSDIM col = 0; col < ncol(); col++) {
      F<ReturnType>::apply(ans_data, data, data + nrow(), arg1);
      ans_data += ans.nrow();
      data += nrow();
    }
    return ans;
  }

  // this is for a positive row subset (positive and negative rowsets cannot mix)
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  template<typename T>
  const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::row_subset(T beg, T end) const {
    TSDIM new_nrow = static_cast<TSDIM>( std::distance(beg,end) );
    TSDIM new_ncol = ncol();

    // allocate new answer
    TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> ans(new_nrow, new_ncol);

    // copy over colnames
    ans.setColnames(getColnames());

    TDATE* dates = getDates();
    TDATA* data = getData();
    TDATE* ans_dates = ans.getDates();
    TDATA* ans_data = ans.getData();

    TSDIM ans_index = 0;

    while(beg!=end) {
      ans_dates[ans_index] = dates[*beg];
      for(TSDIM c = 0; c < ncol(); c++) {
        ans_data[ans.offset(ans_index,c)] = data[offset(*beg,c)];
      }
      ++beg;
      ++ans_index;
    }
    return ans;
  }

  // this is for a positive row subset (positive and negative rowsets cannot mix)
  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  template<typename T>
  const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::pad(T beg, T end) const {
    // construct set with existing dates
    std::set<TDATE> new_dts(getDates(),getDates() + nrow());

    // add new dates
    new_dts.insert(beg,end);

    // allocate new answer
    TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> ans(new_dts.size(), ncol());

    // copy colnames
    ans.setColnames(getColnames());

    // init dates
    std::copy(new_dts.begin(),new_dts.end(),ans.getDates());

    // init to NA
    std::fill(ans.getData(),ans.getData()+ans.nrow() * ans.ncol(),numeric_traits<TDATA>::NA());

    RangeSpecifier<TDATE,TSDIM> range(getDates(),ans.getDates(),nrow(),ans.nrow());
    const TSDIM* r1 = range.getArg1();
    const TSDIM* r2 = range.getArg2();
    TDATA* ans_data = ans.getData();
    TDATA* this_data = getData();
    for(TSDIM col = 0; col < ans.ncol(); col++) {
      for(TSDIM i = 0; i < range.getSize(); i++) {
        ans_data[ans.offset(r2[i],col)] = this_data[offset(r1[i],col)];
      }
    }
    return ans;
  }


  template<typename TDATE, typename TDATA, typename TSDIM, template<typename,typename,typename> class TSDATABACKEND, template<typename> class DatePolicy>
  template<template<class, template<typename> class> class PFUNC>
  const TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy> TSeries<TDATE,TDATA,TSDIM,TSDATABACKEND,DatePolicy>::freq(const int n) const {

    // pre-allocate vector for transformed dates
    typename std::vector<TDATE> partitions;
    partitions.resize(nrow());
    // transform dates
    TDATE* dts = getDates();
    for(TSDIM row = 0; row < nrow(); row++) {
      partitions[row] = PFUNC<TDATE, DatePolicy>()(dts[row],n);
    }
    //std::transform(getDates(), getDates() + nrow(), partitions.begin(), std::bind2nd(PFUNC<TDATE, DatePolicy>, n));
    // vector for selected rows
    std::vector<TSDIM> ans_rows;
    breaks(partitions.begin(),partitions.end(),std::back_inserter(ans_rows));
    return row_subset(ans_rows.begin(), ans_rows.end());
  }
}  // namespace tslib

#endif // TSERIES_HPP
