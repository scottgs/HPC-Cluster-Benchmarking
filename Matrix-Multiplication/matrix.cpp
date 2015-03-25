#include <assert.h>
#include <iostream>

#include "matrix.h"

Matrix::Matrix() : 
	size(Size(0,0)), 
	top_left(0,0), 
	lower_right(0,0)
{

}

Matrix::Matrix(const Size & size) : 
	size(Size(0,0)																																														), 
	top_left(0,0), 
	lower_right(0,0)
{
	// Make sure it's a square matrix.
	assert(size.rows == size.cols);

	data.resize(size.rows);
	
	const uint data_size = data.size();
	const uint num_cols = size.cols;
	for (uint i = 0; i < data_size; ++i)
		data[i].resize(num_cols);
}

Matrix Matrix::operator=(const Matrix & rhs) {
	// Same object? If so, don't do the assignment and just return *this.
	if (&rhs == this) return *this;

	// Otherwise, reallocate and copy the data.
	this->size = rhs.size;
	this->top_left = rhs.top_left;
	this->lower_right = rhs.lower_right;

	// Clear the old data and resize again.
	data.clear();
	data.resize(size.rows);
	const uint num_rows = size.rows;
	const uint num_cols = size.cols;

	for (uint i = 0; i < num_rows; ++i) {
		data[i].clear();
		data[i].resize(num_cols);
	}

	for (uint row = 0; row < num_rows; ++row)
		for (uint col = 0; col < num_cols; ++col)
			data[row][col] = rhs.data[row][col];

	return *this;
}

// Square matrices only for now
// TODO: Transpose the matrix and use tiling with block sizes.
Matrix Matrix::operator*(const Matrix& rhs) {
  // Make sure that both matrices are square and the same size.
  assert(rhs.size.rows == rhs.size.cols);
  assert(this->size.rows == rhs.size.rows);
  assert(this->size.cols == rhs.size.cols);

  Matrix result(this->size);
  uint temp_sum = 0; // To get the reduction

  const uint num_rows = this->size.rows;
  const uint num_cols = this->size.cols;

  for (uint row = 0; row < num_rows; ++row) {
  	for (uint col = 0; col < num_cols; ++col) {
  		temp_sum = 0;
  		for (uint k = 0; k < num_rows; ++k) {
  			temp_sum += this->data[row][k] * rhs.data[k][col];
  		}
  		result.data[row][col] = temp_sum;
  	}
  }

  return result;
}

std::vector<int> Matrix::get_col(uint col) const {
	std::vector<int> col_data;
	const uint num_rows = size.rows;
	for (uint row = 0; row < num_rows; ++row) 
		col_data.push_back(data[row][col]);

	return col_data;
}

void Matrix::insert_sub_mat(const Matrix & mat) {
	const uint num_rows = mat.size.rows;
	const uint num_cols = mat.size.cols;
	const uint row_offset = mat.top_left.row;
	const uint col_offset = mat.top_left.col;

	for (uint row = 0; row < num_rows; ++row)
		for (uint col = 0; col < num_cols; ++col)
			data[row + row_offset][col + col_offset] = mat.data[row][col];
}

inline std::ostream& operator <<(std::ostream & os, const Matrix & matrix) {
	const uint num_rows = matrix.size.rows;
	const uint num_cols = matrix.size.cols;

	for (uint row = 0; row < num_rows; ++row) {
		for (uint col = 0; col < num_cols; ++col) {
			os << matrix.data[row][col] << " ";
		}
		os << std::endl;
	}
	return os;
}

inline std::ostream& operator << (std::ostream & os, const Size& size) {
	os << size.rows << "," << size.cols;
	return os;
}

Matrix_Section::Matrix_Section() :
	row_index(0),
	col_index(0)
{

}

inline Matrix_Section Matrix_Section::operator=(const Matrix_Section & rhs) {
	// Same object? If so, don't do the assignment and just return *this.
	if (&rhs == this) return *this;

	this->row_index = rhs.row_index;
	this->col_index = rhs.col_index;

	// Make sure it's a square matrix
	assert(rhs.row_data.size() == rhs.col_data.size());

	this->row_data.resize(rhs.row_data.size());
	this->col_data.resize(rhs.col_data.size());

	for(uint i = 0; i < rhs.row_data.size(); ++i){
    	std::copy(rhs.row_data[i].begin(), rhs.row_data[i].end(), std::back_inserter(row_data[i]));
    	std::copy(rhs.col_data[i].begin(), rhs.col_data[i].end(), std::back_inserter(col_data[i]));
  	}

	return *this;
}

/**
 * TODO: Can do more clever things to speed up the matrix product
 */
void Matrix_Section::calculate_vector_prod(Matrix & mat) {
  assert(row_data.size() == col_data.size());

  const uint size = row_data.size();

  mat.top_left = Index(row_index, col_index);
  mat.lower_right = Index(row_index + size, col_index + size);

  uint temp_sum = 0; // To get the reduction
  for (uint row = 0; row < size; ++row) {
  	for (uint col = 0; col < size; ++col) {
  		temp_sum = 0;
  		for (uint k = 0; k < row_data[0].size(); ++k) {
  			temp_sum += row_data[row][k] * col_data[col][k];
  		}
  		mat.data[row][col] = temp_sum;
  	}
  }
}