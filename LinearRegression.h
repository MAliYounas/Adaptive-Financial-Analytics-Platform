#ifndef LINEAR_REGRESSION_H
#define LINEAR_REGRESSION_H

#include "Matrix.h"
#include "Vector.h"

using namespace std;

template<typename T>
class LinearRegression {
private:
    Vector<T> coefficients_;
    T intercept_;
    int num_features_;
    bool is_fitted_;

    Matrix<T> add_intercept_column(Matrix<T>& X) {
        int rows = X.rows();
        int cols = X.cols();
        Matrix<T> X_with_intercept(rows, cols + 1);
        for (int i = 0; i < rows; ++i) {
            X_with_intercept[i][0] = T(1);
            for (int j = 0; j < cols; ++j) {
                X_with_intercept[i][j + 1] = X[i][j];
            }
        }
        return X_with_intercept;
    }

    Matrix<T> compute_normal_equation(Matrix<T>& X, Vector<T>& y) {
        Matrix<T> X_transpose = X.transpose();
        Matrix<T> XTX = X_transpose * X;

        Matrix<T> XT = X_transpose;
        Vector<T> XTy(X.cols());
        for (int i = 0; i < XT.rows(); ++i) {
            T sum = T();
            for (int j = 0; j < y.size(); ++j) {
                sum = sum + (XT[i][j] * y[j]);
            }
            XTy[i] = sum;
        }

        Matrix<T> XTX_inv = compute_pseudo_inverse(XTX);

        Vector<T> theta(XTX_inv.rows());
        for (int i = 0; i < XTX_inv.rows(); ++i) {
            T sum = T();
            for (int j = 0; j < XTy.size(); ++j) {
                sum = sum + (XTX_inv[i][j] * XTy[j]);
            }
            theta[i] = sum;
        }

        return create_matrix_from_vector(theta);
    }

    Matrix<T> compute_pseudo_inverse(Matrix<T>& A) {
        Matrix<T> A_transpose = A.transpose();
        Matrix<T> ATA = A_transpose * A;

        T det = ATA.determinant();
        if (det == T(0)) {
            return compute_regularized_inverse(ATA);
        }

        return compute_inverse_2x2_or_general(ATA);
    }

    Matrix<T> compute_regularized_inverse(Matrix<T>& A) {
        T lambda = T(0.01);
        Matrix<T> identity = A.identity(A.rows());
        Matrix<T> scaled_identity = identity.scalar_multiply(lambda);
        Matrix<T> regularized = A + scaled_identity;
        return compute_inverse_2x2_or_general(regularized);
    }

    Matrix<T> compute_inverse_2x2_or_general(Matrix<T>& A) {
        if (A.rows() == 2 && A.cols() == 2) {
            return inverse_2x2(A);
        } else {
            return gauss_jordan_inverse(A);
        }
    }

    Matrix<T> inverse_2x2(Matrix<T>& A) {
        T det = A.determinant();
        if (det == T(0)) {
            throw "Matrix is singular, cannot compute inverse";
        }
        Matrix<T> result(2, 2);
        result[0][0] = A[1][1] / det;
        result[0][1] = -A[0][1] / det;
        result[1][0] = -A[1][0] / det;
        result[1][1] = A[0][0] / det;
        return result;
    }

    Matrix<T> gauss_jordan_inverse(Matrix<T>& A) {
        int n = A.rows();
        Matrix<T> augmented(n, 2 * n);
        Matrix<T> identity = A.identity(n);

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                augmented[i][j] = A[i][j];
                augmented[i][j + n] = identity[i][j];
            }
        }

        for (int i = 0; i < n; ++i) {
            int max_row = i;
            T max_val = augmented[i][i];
            if (max_val < T(0)) {
                max_val = -max_val;
            }

            for (int k = i + 1; k < n; ++k) {
                T val = augmented[k][i];
                if (val < T(0)) {
                    val = -val;
                }
                if (val > max_val) {
                    max_val = val;
                    max_row = k;
                }
            }

            if (max_row != i) {
                for (int j = 0; j < 2 * n; ++j) {
                    T temp = augmented[i][j];
                    augmented[i][j] = augmented[max_row][j];
                    augmented[max_row][j] = temp;
                }
            }

            T pivot = augmented[i][i];
            if (pivot == T(0)) {
                throw "Matrix is singular, cannot compute inverse";
            }

            for (int j = 0; j < 2 * n; ++j) {
                augmented[i][j] = augmented[i][j] / pivot;
            }

            for (int k = 0; k < n; ++k) {
                if (k != i) {
                    T factor = augmented[k][i];
                    for (int j = 0; j < 2 * n; ++j) {
                        augmented[k][j] = augmented[k][j] - (factor * augmented[i][j]);
                    }
                }
            }
        }

        Matrix<T> result(n, n);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                result[i][j] = augmented[i][j + n];
            }
        }
        return result;
    }

    Matrix<T> create_matrix_from_vector(Vector<T>& v) {
        Matrix<T> result(v.size(), 1);
        for (int i = 0; i < v.size(); ++i) {
            result[i][0] = v[i];
        }
        return result;
    }

    T mean(Vector<T>& values) {
        if (values.size() == 0) {
            return T(0);
        }
        T sum = T();
        for (int i = 0; i < values.size(); ++i) {
            sum = sum + values[i];
        }
        return sum / T(values.size());
    }

    T variance(Vector<T>& values) {
        if (values.size() == 0) {
            return T(0);
        }
        T mean_val = mean(values);
        T sum_sq_diff = T();
        for (int i = 0; i < values.size(); ++i) {
            T diff = values[i] - mean_val;
            sum_sq_diff = sum_sq_diff + (diff * diff);
        }
        return sum_sq_diff / T(values.size());
    }

public:
    explicit LinearRegression() {
        intercept_ = T(0);
        num_features_ = 0;
        is_fitted_ = false;
    }

    LinearRegression(LinearRegression& other)
        : coefficients_(other.coefficients_),
          intercept_(other.intercept_),
          num_features_(other.num_features_),
          is_fitted_(other.is_fitted_) {
    }

    LinearRegression(LinearRegression&& other)
        : coefficients_(other.coefficients_),
          intercept_(other.intercept_),
          num_features_(other.num_features_),
          is_fitted_(other.is_fitted_) {
        other.coefficients_ = Vector<T>();
        other.intercept_ = T(0);
        other.num_features_ = 0;
        other.is_fitted_ = false;
    }

    LinearRegression& operator=(LinearRegression& other) {
        if (this != &other) {
            coefficients_ = other.coefficients_;
            intercept_ = other.intercept_;
            num_features_ = other.num_features_;
            is_fitted_ = other.is_fitted_;
        }
        return *this;
    }

    LinearRegression& operator=(LinearRegression&& other) {
        if (this != &other) {
            coefficients_ = other.coefficients_;
            intercept_ = other.intercept_;
            num_features_ = other.num_features_;
            is_fitted_ = other.is_fitted_;
            other.coefficients_ = Vector<T>();
            other.intercept_ = T(0);
            other.num_features_ = 0;
            other.is_fitted_ = false;
        }
        return *this;
    }

    ~LinearRegression() {
    }

    void fit(Matrix<T>& X, Vector<T>& y) {
        int n = X.rows();
        if (n != y.size()) {
            throw "Number of samples in X must match size of y";
        }
        if (n == 0) {
            throw "Cannot fit with empty dataset";
        }

        int m = X.cols();
        num_features_ = m;

        coefficients_ = Vector<T>();
        for (int j = 0; j < m; ++j) {
            coefficients_.push_back(T(0));
        }
        intercept_ = T(0);

        T learning_rate = T(0.05);
        int iterations = 200;

        for (int iter = 0; iter < iterations; ++iter) {
            T grad_b = T(0);
            Vector<T> grad_w;
            for (int j = 0; j < m; ++j) {
                grad_w.push_back(T(0));
            }

            for (int i = 0; i < n; ++i) {
                T pred = intercept_;
                for (int j = 0; j < m; ++j) {
                    pred = pred + (coefficients_[j] * X[i][j]);
                }
                T error = pred - y[i];
                grad_b = grad_b + error;
                for (int j = 0; j < m; ++j) {
                    grad_w[j] = grad_w[j] + (error * X[i][j]);
                }
            }

            T scale = learning_rate / T(n);
            intercept_ = intercept_ - (scale * grad_b);
            for (int j = 0; j < m; ++j) {
                coefficients_[j] = coefficients_[j] - (scale * grad_w[j]);
            }
        }

        is_fitted_ = true;
    }

    void fit(Vector<Vector<T>>& X, Vector<T>& y) {
        if (X.size() == 0) {
            throw "Cannot fit with empty dataset";
        }
        int num_features = X[0].size();
        Matrix<T> X_matrix(X.size(), num_features);
        for (int i = 0; i < X.size(); ++i) {
            if (X[i].size() != num_features) {
                throw "All feature vectors must have the same size";
            }
            for (int j = 0; j < num_features; ++j) {
                X_matrix[i][j] = X[i][j];
            }
        }
        fit(X_matrix, y);
    }

    T predict(Vector<T>& features) {
        if (!is_fitted_) {
            throw "Model must be fitted before prediction";
        }
        if (features.size() != num_features_) {
            throw "Number of features must match training data";
        }
        T prediction = intercept_;
        for (int i = 0; i < coefficients_.size(); ++i) {
            prediction = prediction + (coefficients_[i] * features[i]);
        }
        return prediction;
    }

    Vector<T> predict(Matrix<T>& X) {
        if (!is_fitted_) {
            throw "Model must be fitted before prediction";
        }
        if (X.cols() != num_features_) {
            throw "Number of features must match training data";
        }
        Vector<T> predictions;
        for (int i = 0; i < X.rows(); ++i) {
            Vector<T> row;
            for (int j = 0; j < X.cols(); ++j) {
                row.push_back(X[i][j]);
            }
            T pred = predict(row);
            predictions.push_back(pred);
        }
        return predictions;
    }

    T score(Matrix<T>& X, Vector<T>& y) {
        if (!is_fitted_) {
            throw "Model must be fitted before scoring";
        }
        Vector<T> predictions = predict(X);
        return r_squared(y, predictions);
    }

    T r_squared(Vector<T>& y_true, Vector<T>& y_pred) {
        if (y_true.size() != y_pred.size()) {
            throw "True and predicted values must have same size";
        }
        if (y_true.size() == 0) {
            return T(0);
        }

        T y_mean = mean(y_true);
        T ss_res = T();
        T ss_tot = T();

        for (int i = 0; i < y_true.size(); ++i) {
            T diff_res = y_true[i] - y_pred[i];
            ss_res = ss_res + (diff_res * diff_res);

            T diff_tot = y_true[i] - y_mean;
            ss_tot = ss_tot + (diff_tot * diff_tot);
        }

        if (ss_tot == T(0)) {
            return T(1);
        }

        return T(1) - (ss_res / ss_tot);
    }

    T mean_squared_error(Vector<T>& y_true, Vector<T>& y_pred) {
        if (y_true.size() != y_pred.size()) {
            throw "True and predicted values must have same size";
        }
        if (y_true.size() == 0) {
            return T(0);
        }

        T sum_sq_error = T();
        for (int i = 0; i < y_true.size(); ++i) {
            T error = y_true[i] - y_pred[i];
            sum_sq_error = sum_sq_error + (error * error);
        }
        return sum_sq_error / T(y_true.size());
    }

    T root_mean_squared_error(Vector<T>& y_true, Vector<T>& y_pred) {
        T mse = mean_squared_error(y_true, y_pred);
        T rmse = T(1);
        T temp = mse;
        T epsilon = T(0.0001);

        if (temp < T(0)) {
            temp = -temp;
        }

        while ((temp - (rmse * rmse)) > epsilon) {
            rmse = (rmse + (temp / rmse)) / T(2);
        }

        if (mse < T(0)) {
            return -rmse;
        }
        return rmse;
    }

    T mean_absolute_error(Vector<T>& y_true, Vector<T>& y_pred) {
        if (y_true.size() != y_pred.size()) {
            throw "True and predicted values must have same size";
        }
        if (y_true.size() == 0) {
            return T(0);
        }

        T sum_abs_error = T();
        for (int i = 0; i < y_true.size(); ++i) {
            T error = y_true[i] - y_pred[i];
            if (error < T(0)) {
                error = -error;
            }
            sum_abs_error = sum_abs_error + error;
        }
        return sum_abs_error / T(y_true.size());
    }

    Vector<T> get_coefficients() {
        if (!is_fitted_) {
            throw "Model must be fitted before getting coefficients";
        }
        return coefficients_;
    }

    T get_intercept() {
        if (!is_fitted_) {
            throw "Model must be fitted before getting intercept";
        }
        return intercept_;
    }

    int get_num_features() {
        return num_features_;
    }

    bool is_fitted() {
        return is_fitted_;
    }

    void reset() {
        coefficients_.clear();
        intercept_ = T(0);
        num_features_ = 0;
        is_fitted_ = false;
    }

    T correlation_coefficient(Vector<T>& x, Vector<T>& y) {
        if (x.size() != y.size()) {
            throw "Vectors must have same size for correlation";
        }
        if (x.size() == 0) {
            return T(0);
        }

        T x_mean = mean(x);
        T y_mean = mean(y);

        T numerator = T();
        T x_variance_sum = T();
        T y_variance_sum = T();

        for (int i = 0; i < x.size(); ++i) {
            T x_diff = x[i] - x_mean;
            T y_diff = y[i] - y_mean;
            numerator = numerator + (x_diff * y_diff);
            x_variance_sum = x_variance_sum + (x_diff * x_diff);
            y_variance_sum = y_variance_sum + (y_diff * y_diff);
        }

        T denominator_sq = x_variance_sum * y_variance_sum;
        if (denominator_sq == T(0)) {
            return T(0);
        }

        T denominator = T(1);
        T temp = denominator_sq;
        T epsilon = T(0.0001);

        if (temp < T(0)) {
            temp = -temp;
        }

        while ((temp - (denominator * denominator)) > epsilon) {
            denominator = (denominator + (temp / denominator)) / T(2);
        }

        if (denominator_sq < T(0)) {
            denominator = -denominator;
        }

        return numerator / denominator;
    }

    Vector<T> residuals(Vector<T>& y_true, Vector<T>& y_pred) {
        if (y_true.size() != y_pred.size()) {
            throw "True and predicted values must have same size";
        }
        Vector<T> res;
        for (int i = 0; i < y_true.size(); ++i) {
            res.push_back(y_true[i] - y_pred[i]);
        }
        return res;
    }

    T explained_variance_score(Vector<T>& y_true, Vector<T>& y_pred) {
        if (y_true.size() != y_pred.size()) {
            throw "True and predicted values must have same size";
        }
        if (y_true.size() == 0) {
            return T(0);
        }

        T y_var = variance(y_true);
        if (y_var == T(0)) {
            return T(1);
        }

        T sum_sq_diff = T();
        T y_mean = mean(y_true);
        for (int i = 0; i < y_true.size(); ++i) {
            T diff = y_pred[i] - y_mean;
            sum_sq_diff = sum_sq_diff + (diff * diff);
        }

        T explained_var = sum_sq_diff / T(y_true.size());
        return T(1) - (explained_var / y_var);
    }
};

#endif

