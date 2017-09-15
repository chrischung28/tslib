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
#pragma once

#include <iterator>
#include <string>
#include <vector>

namespace tslib {

// template for vector backend
template <typename IDX, typename T, typename DIM> class VectorBackend {
private:
  // DIM is the type of the ncol value
  DIM ncol_;
  // IDX will represent the type of the index vector 
  // index is number of rows
  // i believe the index refers to the dates
  std::vector<IDX> index_;
  // T will represent the type of data
  // will contain all the data
  std::vector<T> data_;
  // colnames will be represented by strings
  std::vector<std::string> colnames_;

  // Gets you the value in the ith vector to the right of the matrix
  // col major offset 
  size_t column_offset(DIM i) const { return static_cast<size_t>(i) * index_.size(); }

public:
  // define iterators for the index and the data vectors
  typedef typename std::vector<IDX>::iterator index_iterator;
  typedef typename std::vector<IDX>::const_iterator const_index_iterator;

  typedef typename std::vector<T>::const_iterator const_data_iterator;
  typedef typename std::vector<T>::iterator data_iterator;

  // no default constructor
  VectorBackend() = delete;
  // copy constructor copies the ncol, index vector, data vector, and column names
  VectorBackend(const VectorBackend &t) : ncol_{t.ncol_}, index_{t.index_}, data_{t.data_}, colnames_{t.colnames_} {}
  // if dimensions given for the nrow and col, set the ncol, set index as having the number of elements in the index, and the data vector
  // having the same amount of elements as nrow*ncol (or number of elements in the matrix), set colnames to nothing
  VectorBackend(DIM nrow, DIM ncol) : ncol_{ncol}, index_(nrow), data_(nrow * ncol), colnames_{} {}
  // no assignment constructor
  VectorBackend &operator=(const VectorBackend &rhs) = delete;
  // default move constructor
  VectorBackend(VectorBackend &&)                    = default;

  // calling nrow will get you the size of the index vector
  DIM nrow() const { return static_cast<DIM>(index_.size()); }
  // get the number of columns in the vector
  DIM ncol() const { return ncol_; }

  // get the beginning and ending iterators of the index and the 
  const_index_iterator index_begin() const { return index_.begin(); }
  index_iterator index_begin() { return index_.begin(); }
  const_index_iterator index_end() const { return index_.end(); }
  index_iterator index_end() { return index_.end(); }

  // return the iterators for each colum as if it was its own vector
  const_data_iterator col_begin(DIM i) const { return data_.begin() + column_offset(i); } // head of column
  data_iterator col_begin(DIM i) { return data_.begin() + column_offset(i); }
  const_data_iterator col_end(DIM i) const { return data_.begin() + column_offset(i + 1L); } // head of next column
  data_iterator col_end(DIM i) { return data_.begin() + column_offset(i + 1L); }
 
  // get the column names
  const std::vector<std::string> getColnames() const { return colnames_; }
  // the the number of column names
  const DIM getColnamesSize() const { return static_cast<DIM>(colnames_.size()); }
  // takes in a string vector, makes sure it is equal to the number of columns, and sets the column names
  // returns true if set, false if not
  const bool setColnames(const std::vector<std::string> &names) {
    if (static_cast<DIM>(names.size()) == ncol_) {
      colnames_ = names;
      return true;
    }
    return false;
  }
};

} // namespace tslib
