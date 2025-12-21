#ifndef MATRIX_H
#define MATRIX_H

#include "Vector.h"

using namespace std;

template<typename T>
class Matrix {
private:
    Vector<Vector<T>> data;
    int rows_;
    int cols_;

    void initialize_matrix(int rows, int cols, const T& value = T()) {
        rows_ = rows;
        cols_ = cols;
        data.clear();
        for (int i = 0; i < rows_; ++i) {
            Vector<T> row;
            for (int j = 0; j < cols_; ++j) {
                row.push_back(value);
            }
            data.push_back(row);
        }
    }

    void copy_matrix(Matrix& other) {
        rows_ = other.rows_;
        cols_ = other.cols_;
        data.clear();
        for (int i = 0; i < rows_; ++i) {
            Vector<T> row;
            for (int j = 0; j < cols_; ++j) {
                row.push_back(other.data[i][j]);
            }
            data.push_back(row);
        }
    }

public:
    explicit Matrix(int rows = 0, int cols = 0, const T& value = T()) {
        if (rows > 0 && cols > 0) {
            initialize_matrix(rows, cols, value);
        } else {
            rows_ = 0;
            cols_ = 0;
        }
    }

    Matrix(Matrix& other) {
        copy_matrix(other);
    }

    Matrix(Matrix&& other)
        : data(other.data), rows_(other.rows_), cols_(other.cols_) {
        other.data = Vector<Vector<T>>();
        other.rows_ = 0;
        other.cols_ = 0;
    }

    Matrix& operator=(Matrix& other) {
        if (this != &other) {
            copy_matrix(other);
        }
        return *this;
    }

    Matrix& operator=(Matrix&& other) {
        if (this != &other) {
            data = other.data;
            rows_ = other.rows_;
            cols_ = other.cols_;
            other.data = Vector<Vector<T>>();
            other.rows_ = 0;
            other.cols_ = 0;
        }
        return *this;
    }

    ~Matrix() {
        data.clear();
    }

    void set_element(int row, int col, const T& value) {
        if (row < 0 || row >= rows_ || col < 0 || col >= cols_) {
            throw "Matrix index out of range";
        }
        data[row][col] = value;
    }

    T& get_element(int row, int col) {
        if (row < 0 || row >= rows_ || col < 0 || col >= cols_) {
            throw "Matrix index out of range";
        }
        return data[row][col];
    }


    Vector<T>& operator[](int row) {
        if (row < 0 || row >= rows_) {
            throw "Matrix row index out of range";
        }
        return data[row];
    }

    int rows() {
        return rows_;
    }

    int cols() {
        return cols_;
    }

    bool is_square() {
        return (rows_ == cols_);
    }

    Matrix transpose() {
        Matrix result(cols_, rows_);
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                result.data[j][i] = data[i][j];
            }
        }
        return result;
    }

    Matrix operator+(Matrix& other) {
        if (rows_ != other.rows_ || cols_ != other.cols_) {
            throw "Matrix dimensions must match for addition";
        }
        Matrix result(rows_, cols_);
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                result.data[i][j] = data[i][j] + other.data[i][j];
            }
        }
        return result;
    }

    Matrix operator-(Matrix& other) {
        if (rows_ != other.rows_ || cols_ != other.cols_) {
            throw "Matrix dimensions must match for subtraction";
        }
        Matrix result(rows_, cols_);
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                result.data[i][j] = data[i][j] - other.data[i][j];
            }
        }
        return result;
    }

    Matrix operator*(Matrix& other) {
        if (cols_ != other.rows_) {
            throw "Matrix dimensions incompatible for multiplication";
        }
        Matrix result(rows_, other.cols_);
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < other.cols_; ++j) {
                T sum = T();
                for (int k = 0; k < cols_; ++k) {
                    sum = sum + (data[i][k] * other.data[k][j]);
                }
                result.data[i][j] = sum;
            }
        }
        return result;
    }

    Matrix scalar_multiply(const T& scalar) {
        Matrix result(rows_, cols_);
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                result.data[i][j] = data[i][j] * scalar;
            }
        }
        return result;
    }

    Matrix scalar_add(const T& scalar) {
        Matrix result(rows_, cols_);
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                result.data[i][j] = data[i][j] + scalar;
            }
        }
        return result;
    }

    Matrix scalar_subtract(const T& scalar) {
        Matrix result(rows_, cols_);
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                result.data[i][j] = data[i][j] - scalar;
            }
        }
        return result;
    }

    Matrix operator*(const T& scalar) {
        return scalar_multiply(scalar);
    }

    Matrix operator+(const T& scalar) {
        return scalar_add(scalar);
    }

    Matrix operator-(const T& scalar) {
        return scalar_subtract(scalar);
    }

    bool operator==(Matrix& other) {
        if (rows_ != other.rows_ || cols_ != other.cols_) {
            return false;
        }
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                if (data[i][j] != other.data[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(Matrix& other) {
        return !(*this == other);
    }

    Matrix get_submatrix(int exclude_row, int exclude_col) {
        if (!is_square()) {
            throw "Submatrix can only be created from square matrices";
        }
        if (rows_ <= 1) {
            throw "Matrix too small for submatrix operation";
        }
        Matrix result(rows_ - 1, cols_ - 1);
        int result_row = 0;
        for (int i = 0; i < rows_; ++i) {
            if (i == exclude_row) {
                continue;
            }
            int result_col = 0;
            for (int j = 0; j < cols_; ++j) {
                if (j == exclude_col) {
                    continue;
                }
                result.data[result_row][result_col] = data[i][j];
                ++result_col;
            }
            ++result_row;
        }
        return result;
    }

    T determinant() {
        if (!is_square()) {
            throw "Determinant can only be calculated for square matrices";
        }
        if (rows_ == 1) {
            return data[0][0];
        }
        if (rows_ == 2) {
            return (data[0][0] * data[1][1]) - (data[0][1] * data[1][0]);
        }
        T det = T();
        int sign = 1;
        for (int j = 0; j < cols_; ++j) {
            Matrix sub = get_submatrix(0, j);
            T sub_det = sub.determinant();
            det = det + (sign * data[0][j] * sub_det);
            sign = -sign;
        }
        return det;
    }

    Matrix identity(int size) {
        Matrix result(size, size);
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (i == j) {
                    result.data[i][j] = T(1);
                } else {
                    result.data[i][j] = T(0);
                }
            }
        }
        return result;
    }

    Matrix create_identity() {
        if (!is_square()) {
            throw "Identity matrix can only be created for square matrices";
        }
        return identity(rows_);
    }

    Matrix power(int exponent) {
        if (!is_square()) {
            throw "Matrix power can only be calculated for square matrices";
        }
        if (exponent < 0) {
            throw "Negative exponent not supported";
        }
        if (exponent == 0) {
            return identity(rows_);
        }
        if (exponent == 1) {
            return *this;
        }
        Matrix result = *this;
        for (int i = 1; i < exponent; ++i) {
            result = result * (*this);
        }
        return result;
    }

    Matrix element_wise_multiply(Matrix& other) {
        if (rows_ != other.rows_ || cols_ != other.cols_) {
            throw "Matrix dimensions must match for element-wise multiplication";
        }
        Matrix result(rows_, cols_);
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                result.data[i][j] = data[i][j] * other.data[i][j];
            }
        }
        return result;
    }

    Matrix element_wise_divide(Matrix& other) {
        if (rows_ != other.rows_ || cols_ != other.cols_) {
            throw "Matrix dimensions must match for element-wise division";
        }
        Matrix result(rows_, cols_);
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                if (other.data[i][j] == T(0)) {
                    throw "Division by zero in element-wise division";
                }
                result.data[i][j] = data[i][j] / other.data[i][j];
            }
        }
        return result;
    }

    T trace() {
        if (!is_square()) {
            throw "Trace can only be calculated for square matrices";
        }
        T sum = T();
        for (int i = 0; i < rows_; ++i) {
            sum = sum + data[i][i];
        }
        return sum;
    }

    T sum_all_elements() {
        T sum = T();
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                sum = sum + data[i][j];
            }
        }
        return sum;
    }

    T max_element() {
        if (rows_ == 0 || cols_ == 0) {
            throw "Cannot find max element in empty matrix";
        }
        T max_val = data[0][0];
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                if (data[i][j] > max_val) {
                    max_val = data[i][j];
                }
            }
        }
        return max_val;
    }

    T min_element() {
        if (rows_ == 0 || cols_ == 0) {
            throw "Cannot find min element in empty matrix";
        }
        T min_val = data[0][0];
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                if (data[i][j] < min_val) {
                    min_val = data[i][j];
                }
            }
        }
        return min_val;
    }

    Vector<T> get_row(int row_index) {
        if (row_index < 0 || row_index >= rows_) {
            throw "Row index out of range";
        }
        Vector<T> row;
        for (int j = 0; j < cols_; ++j) {
            row.push_back(data[row_index][j]);
        }
        return row;
    }

    Vector<T> get_column(int col_index) {
        if (col_index < 0 || col_index >= cols_) {
            throw "Column index out of range";
        }
        Vector<T> col;
        for (int i = 0; i < rows_; ++i) {
            col.push_back(data[i][col_index]);
        }
        return col;
    }

    void set_row(int row_index, Vector<T>& row) {
        if (row_index < 0 || row_index >= rows_) {
            throw "Row index out of range";
        }
        if (row.size() != cols_) {
            throw "Row size must match matrix column size";
        }
        for (int j = 0; j < cols_; ++j) {
            data[row_index][j] = row[j];
        }
    }

    void set_column(int col_index, Vector<T>& col) {
        if (col_index < 0 || col_index >= cols_) {
            throw "Column index out of range";
        }
        if (col.size() != rows_) {
            throw "Column size must match matrix row size";
        }
        for (int i = 0; i < rows_; ++i) {
            data[i][col_index] = col[i];
        }
    }

    Matrix horizontal_concatenate(Matrix& other) {
        if (rows_ != other.rows_) {
            throw "Row count must match for horizontal concatenation";
        }
        Matrix result(rows_, cols_ + other.cols_);
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                result.data[i][j] = data[i][j];
            }
            for (int j = 0; j < other.cols_; ++j) {
                result.data[i][cols_ + j] = other.data[i][j];
            }
        }
        return result;
    }

    Matrix vertical_concatenate(Matrix& other) {
        if (cols_ != other.cols_) {
            throw "Column count must match for vertical concatenation";
        }
        Matrix result(rows_ + other.rows_, cols_);
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                result.data[i][j] = data[i][j];
            }
        }
        for (int i = 0; i < other.rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                result.data[rows_ + i][j] = other.data[i][j];
            }
        }
        return result;
    }

    void fill(const T& value) {
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                data[i][j] = value;
            }
        }
    }

    void fill_diagonal(const T& value) {
        if (!is_square()) {
            throw "Diagonal fill can only be done for square matrices";
        }
        for (int i = 0; i < rows_; ++i) {
            data[i][i] = value;
        }
    }

    bool is_symmetric() {
        if (!is_square()) {
            return false;
        }
        Matrix transposed = transpose();
        return (*this == transposed);
    }

    bool is_diagonal() {
        if (!is_square()) {
            return false;
        }
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                if (i != j && data[i][j] != T(0)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool is_upper_triangular() {
        if (!is_square()) {
            return false;
        }
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < i; ++j) {
                if (data[i][j] != T(0)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool is_lower_triangular() {
        if (!is_square()) {
            return false;
        }
        for (int i = 0; i < rows_; ++i) {
            for (int j = i + 1; j < cols_; ++j) {
                if (data[i][j] != T(0)) {
                    return false;
                }
            }
        }
        return true;
    }

    void swap_rows(int row1, int row2) {
        if (row1 < 0 || row1 >= rows_ || row2 < 0 || row2 >= rows_) {
            throw "Row indices out of range";
        }
        if (row1 == row2) {
            return;
        }
        for (int j = 0; j < cols_; ++j) {
            T temp = data[row1][j];
            data[row1][j] = data[row2][j];
            data[row2][j] = temp;
        }
    }

    void swap_columns(int col1, int col2) {
        if (col1 < 0 || col1 >= cols_ || col2 < 0 || col2 >= cols_) {
            throw "Column indices out of range";
        }
        if (col1 == col2) {
            return;
        }
        for (int i = 0; i < rows_; ++i) {
            T temp = data[i][col1];
            data[i][col1] = data[i][col2];
            data[i][col2] = temp;
        }
    }
};

#endif

