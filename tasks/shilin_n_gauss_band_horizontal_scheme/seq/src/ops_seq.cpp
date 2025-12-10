#include "shilin_n_gauss_band_horizontal_scheme/seq/include/ops_seq.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

#include "shilin_n_gauss_band_horizontal_scheme/common/include/common.hpp"

namespace shilin_n_gauss_band_horizontal_scheme {

ShilinNGaussBandHorizontalSchemeSEQ::ShilinNGaussBandHorizontalSchemeSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<double>();
}

bool ShilinNGaussBandHorizontalSchemeSEQ::ValidationImpl() {
  const InType &input = GetInput();
  if (input.empty()) {
    return false;
  }

  size_t n = input.size();
  if (n == 0) {
    return false;
  }

  size_t cols = input[0].size();
  if (cols < n + 1) {
    return false;
  }
  for (size_t i = 1; i < n; ++i) {
    if (input[i].size() != cols) {
      return false;
    }
  }

  return true;
}

bool ShilinNGaussBandHorizontalSchemeSEQ::PreProcessingImpl() {
  GetOutput() = std::vector<double>();
  return true;
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
bool ShilinNGaussBandHorizontalSchemeSEQ::RunImpl() {
  InType augmented_matrix = GetInput();
  size_t n = augmented_matrix.size();
  if (n == 0) {
    return false;
  }

  size_t cols = augmented_matrix[0].size();
  if (cols < n + 1) {
    return false;
  }

  for (size_t k = 0; k < n; ++k) {
    size_t max_row = k;
    double max_val = std::abs(augmented_matrix[k][k]);

    for (size_t i = k + 1; i < n; ++i) {
      if (std::abs(augmented_matrix[i][k]) > max_val) {
        max_val = std::abs(augmented_matrix[i][k]);
        max_row = i;
      }
    }

    if (max_row != k) {
      std::swap(augmented_matrix[k], augmented_matrix[max_row]);
    }

    if (std::abs(augmented_matrix[k][k]) < 1e-10) {
      return false;
    }
    for (size_t i = k + 1; i < n; ++i) {
      if (std::abs(augmented_matrix[i][k]) > 1e-10) {
        double factor = augmented_matrix[i][k] / augmented_matrix[k][k];

        for (size_t j = k; j < cols; ++j) {
          augmented_matrix[i][j] -= factor * augmented_matrix[k][j];
        }
      }
    }
  }

  std::vector<double> x(n, 0.0);
  for (int i = static_cast<int>(n) - 1; i >= 0; --i) {
    double sum = 0.0;
    for (size_t j = static_cast<size_t>(i) + 1; j < n; ++j) {
      sum += augmented_matrix[static_cast<size_t>(i)][j] * x[j];
    }

    x[static_cast<size_t>(i)] =
        (augmented_matrix[static_cast<size_t>(i)][cols - 1] - sum) / augmented_matrix[static_cast<size_t>(i)][static_cast<size_t>(i)];
  }

  GetOutput() = x;
  return true;
}

bool ShilinNGaussBandHorizontalSchemeSEQ::PostProcessingImpl() {
  return !GetOutput().empty();
}

}  // namespace shilin_n_gauss_band_horizontal_scheme

