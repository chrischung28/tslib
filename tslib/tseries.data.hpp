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

#ifndef TSERIES_DATA_HPP
#define TSERIES_DATA_HPP

#include <vector>
#include <string>

namespace tslib {
  // templated backend class that takes in the type of the Date, Data, and the dimensions
  template <typename TDATE,typename TDATA, typename TSDIM = long>
  class TSdataSingleThreaded {
  private:
    // bool for release_data - for data created using new
    bool release_data_;
    // vector of strings for the column names
    std::vector<std::string> colnames_;
    // nrow and ncol
    TSDIM rows_;
    TSDIM cols_;
    // seems to use array pointers
    // pointer to dates
    TDATE* dates_;
    // pointer to data
    TDATA* data_;
  public:
    // destructor for class
    ~TSdataSingleThreaded();
    // default construct
    TSdataSingleThreaded();
    // copy constructor
    TSdataSingleThreaded(const TSdataSingleThreaded& t);
    // constructor that takes in dimensions
    TSdataSingleThreaded(const TSDIM rows, const TSDIM cols);
    // constructor that takes in a vector of data, dates, rows, columns
    // bool release defualt set to false
    TSdataSingleThreaded(TDATA* external_data,
                         TDATE* external_dates,
                         const TSDIM rows,
                         const TSDIM cols,
                         const bool release = false);
    // assignment operator
    TSdataSingleThreaded& operator=(const TSdataSingleThreaded& rhs);

    // functions to get private data
    TSDIM nrow() const;
    TSDIM ncol() const;
    TDATA* getData() const;
    TDATE* getDates() const;

    // set column names
    void setColnames(const std::vector<std::string>& cnames);
    // get column names
    std::vector<std::string> getColnames() const;
    // get column name size
    const size_t getColnamesSize() const;
  };

  // destructor
  // delete memory off heap if data was created from it
  template <typename TDATE,typename TDATA, typename TSDIM>
  TSdataSingleThreaded<TDATE,TDATA,TSDIM>::~TSdataSingleThreaded() {
    if(release_data_) {
      delete []dates_;
      delete []data_;
    }
  }

  // default constructor assumes release_data is true, 0 rows, 0 cols
  // no dates and no data
  template <typename TDATE,typename TDATA, typename TSDIM>
  TSdataSingleThreaded<TDATE,TDATA,TSDIM>::TSdataSingleThreaded():
    release_data_(true), rows_(0), cols_(0), dates_(NULL), data_(NULL) {}

  // copy everything from the other thread
  // create new arrays for dates and data and copy all the data through
  template <typename TDATE,typename TDATA, typename TSDIM>
  TSdataSingleThreaded<TDATE,TDATA,TSDIM>::TSdataSingleThreaded(const TSdataSingleThreaded& t):
  release_data_(true), colnames_(t.colnames_), rows_(t.rows_), cols_(t.cols_), dates_(new TDATE[t.rows_]), data_(new TDATA[t.rows_*t.cols_]) {
    std::copy(t.dates_, t.dates_ + rows_, dates_);
    std::copy(t.data_, t.data_ + t.rows_*t.cols_, data_);
  }

  // set release_data to True, set rows and cols, set the dates and data arrays without inputting any values
  template <typename TDATE,typename TDATA, typename TSDIM>
  TSdataSingleThreaded<TDATE,TDATA,TSDIM>::TSdataSingleThreaded(const TSDIM rows, const TSDIM cols):
    release_data_(true), rows_(rows), cols_(cols), dates_(new TDATE[rows]), data_(new TDATA[rows*cols]) {}

  // provide own release, provide external data array, external_data array, and the nrows and ncols
  template <typename TDATE,typename TDATA, typename TSDIM>
  TSdataSingleThreaded<TDATE,TDATA,TSDIM>::TSdataSingleThreaded(TDATA* external_data, TDATE* external_dates, const TSDIM nrows, const TSDIM ncols, const bool release):
    release_data_(release), rows_(nrows), cols_(ncols), dates_(external_dates), data_(external_data) {}
  
  // assignment operator
  template <typename TDATE,typename TDATA, typename TSDIM>
  TSdataSingleThreaded<TDATE,TDATA,TSDIM>& TSdataSingleThreaded<TDATE,TDATA,TSDIM>::operator=(const TSdataSingleThreaded& rhs) {
    // release old data
    // delete the old data
    if(release_data_) {
      delete []dates_;
      delete []data_;
    }
    // set release_data to true to be sure
    release_data_ = true;
    // copy the rest from rhs
    colnames_ = rhs.colnames_;
    rows_ = rhs.rows_;
    cols_ = rhs.cols_;
    dates_ = new TDATE[rhs.rows_];
    data_ = new TDATA[rhs.rows_*rhs.cols_];
    std::copy(rhs.dates_, rhs.dates_ + rhs.rows_, dates_);
    std::copy(rhs.data_, rhs.data_ + rhs.rows_*rhs.cols_, data_);
    return *this;
  }

  // set the colnames
  template <typename TDATE,typename TDATA, typename TSDIM>
  void TSdataSingleThreaded<TDATE,TDATA,TSDIM>::setColnames(const std::vector<std::string>& cnames) {
    colnames_ = cnames;
  }

  // get the column names
  template <typename TDATE,typename TDATA, typename TSDIM>
  inline
  std::vector<std::string> TSdataSingleThreaded<TDATE,TDATA,TSDIM>::getColnames() const {
    return colnames_;
  }

  // get the colum names size
  template <typename TDATE,typename TDATA, typename TSDIM>
  inline
  const size_t TSdataSingleThreaded<TDATE,TDATA,TSDIM>::getColnamesSize() const {
    return colnames_.size();
  }

  // get the pointer to the data array
  template <typename TDATE,typename TDATA, typename TSDIM>
  inline
  TDATA* TSdataSingleThreaded<TDATE,TDATA,TSDIM>::getData() const {
    return data_;
  }
  // get the pointer to the dates array
  template <typename TDATE,typename TDATA, typename TSDIM>
  inline
  TDATE* TSdataSingleThreaded<TDATE,TDATA,TSDIM>::getDates() const {
    return dates_;
  }
  // get the number of rows
  template <typename TDATE,typename TDATA, typename TSDIM>
  inline
  TSDIM TSdataSingleThreaded<TDATE,TDATA,TSDIM>::nrow() const {
    return rows_;
  }
  // get the number of columns
  template <typename TDATE,typename TDATA, typename TSDIM>
  inline
  TSDIM TSdataSingleThreaded<TDATE,TDATA,TSDIM>::ncol() const {
    return cols_;
  }

} // namespace tslib

#endif // TSERIES_DATA_HPP
