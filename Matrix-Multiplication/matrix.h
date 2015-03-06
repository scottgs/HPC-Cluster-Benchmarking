#ifndef MATRIX_H
#define MATRIX_H

#include <boost/serialization/serialization.hpp>
#include <ostream>
#include <vector>

// TODO: Try to use template constructors for Index and Size.
// Currently can't do it since both take in two uint arguments
// and the compiler cannot do the reduction on it--shadowing of templates.

class Index {
public:
  Index(uint row, uint col) : row(row), col(col)
  {

  }

  Index operator=(const Index & rhs) {
    if (this != &rhs) {
      this->row = rhs.row;
      this->col = rhs.col;
    }
    return *this;
  }

  uint row, col;

  // Use Boost Access to allow serialization to access non-public data members.
  friend class serialize;

  template<class Archive>
  void serialize(Archive & a, const uint version) {
    // Simply serialize the data members
    a & row;
    a & col;
  }
};

class Size {
public:
  Size(uint rows, uint cols) : rows(rows), cols(cols) 
  {

  }

  Size operator=(const Size & rhs) {
    if (this != &rhs) {
      this->rows = rhs.rows;
      this->cols = rhs.cols;
    }
    return *this;
  }

  uint rows, cols;

  friend std::ostream& operator<< (std::ostream & os, const Size & size);

  // Use Boost Access to allow serialization to access data members.
  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive & a, const uint version) {
    // Simply serialize the data members
    a & rows;
    a & cols;
  }
};

class Matrix {
  Matrix();
  Matrix(const Size& size);
  ~Matrix();

  // Overload a few operators
  Matrix operator=(const Matrix & rhs);
  Matrix operator*(const Matrix & rhs);
  bool operator==(const Matrix & rhs);
  // To print the matrix
  friend std::ostream& operator << (std::ostream & os, const Matrix & mat);

  std::vector<int> get_row(int row) const;
  std::vector<int> get_col(int col) const;

  void insert_sub_mat(const Matrix & mat);
  void square_mult(Matrix & mat); // For squaring matrices

  Size size;
  std::vector<std::vector<int> > data;

  // Clever way of focusing in on sub matrices.
  Index top_left, lower_right;

  // Use Boost Access to allow serialization to access data members.
  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive & a, const uint version) {
    // Simply serialize the data members
    a & size;
    a & data;
    a & top_left;
    a & lower_right;
  }
};

class Matrix_Section {
public:
  Matrix_Section();
  ~Matrix_Section();

  Matrix_Section operator=(const Matrix_Section & rhs);

  int row_index, col_index;
  std::vector<std::vector<int> > row_data, col_data;
  
  // Use Boost Access to allow serialization to access data members.
  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive & a, const uint version) {
    // Simply serialize the data members
    a & row_index;
    a & col_index;
    a & row_data;
    a & col_data;
  }
};

// End MATRIX_H
#endif  
