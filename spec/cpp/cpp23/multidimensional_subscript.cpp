// Multidimensional subscript operator — C++23
// What: operator[] can now take more than one argument, enabling matrix[i, j].
// Why:  previously you needed operator() for the same job, which broke the natural
//       indexing notation expected from any matrix or grid type.
// Pre:  matrix(i, j) — workable, but every reader had to learn the convention.

#include <iostream>
#include <vector>

class Matrix {
public:
    Matrix(std::size_t rows, std::size_t cols)
        : rows_(rows), cols_(cols), data_(rows * cols, 0) {}

    // C++23: multi-arg subscript
    int&       operator[](std::size_t i, std::size_t j)       { return data_[i * cols_ + j]; }
    const int& operator[](std::size_t i, std::size_t j) const { return data_[i * cols_ + j]; }

    std::size_t rows() const { return rows_; }
    std::size_t cols() const { return cols_; }

private:
    std::size_t      rows_;
    std::size_t      cols_;
    std::vector<int> data_;
};

int main() {
    Matrix m(3, 4);

    // Fill with i*10+j
    for (std::size_t i = 0; i < m.rows(); ++i) {
        for (std::size_t j = 0; j < m.cols(); ++j) {
            m[i, j] = static_cast<int>(i * 10 + j);
        }
    }

    // Print using the same notation
    for (std::size_t i = 0; i < m.rows(); ++i) {
        for (std::size_t j = 0; j < m.cols(); ++j) {
            std::cout << m[i, j] << '\t';
        }
        std::cout << "\n";
    }

    return 0;
}
