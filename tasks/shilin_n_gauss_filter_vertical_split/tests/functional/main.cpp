#include <gtest/gtest.h>
#include <stb/stb_image.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

#include "shilin_n_gauss_filter_vertical_split/common/include/common.hpp"
#include "shilin_n_gauss_filter_vertical_split/mpi/include/ops_mpi.hpp"
#include "shilin_n_gauss_filter_vertical_split/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace shilin_n_gauss_filter_vertical_split {

class ShilinNGaussFilterVerticalSplitRunFuncTestsProcesses
    : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::to_string(std::get<0>(test_param)) + "x" + std::to_string(std::get<1>(test_param));
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    int width = std::get<0>(params);
    int height = std::get<1>(params);

    InType input_data;
    input_data.width = width;
    input_data.height = height;
    input_data.channels = 3;

    size_t pixel_count =
        static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(input_data.channels);
    input_data.pixels = std::vector<uint8_t>(pixel_count);

    // создаем тестовое изображение с градиентом
    for (int row = 0; row < height; ++row) {
      for (int col = 0; col < width; ++col) {
        for (int ch = 0; ch < input_data.channels; ++ch) {
          size_t idx =
              (static_cast<size_t>(row) * static_cast<size_t>(width) * static_cast<size_t>(input_data.channels)) +
              (static_cast<size_t>(col) * static_cast<size_t>(input_data.channels)) + static_cast<size_t>(ch);
          input_data.pixels[idx] = static_cast<uint8_t>((col + row + ch * 10) % 256);
        }
      }
    }

    // вычисляем ожидаемый результат применяя фильтр гаусса
    expected_output_ = std::vector<uint8_t>(pixel_count);
    constexpr std::array<std::array<double, 3>, 3> kKernel = {{{{1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0}},
                                                               {{2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0}},
                                                               {{1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0}}}};

    for (int row = 0; row < height; ++row) {
      for (int col = 0; col < width; ++col) {
        for (int ch = 0; ch < input_data.channels; ++ch) {
          double sum = 0.0;
          for (int ky = -1; ky <= 1; ++ky) {
            for (int kx = -1; kx <= 1; ++kx) {
              int px = col + kx;
              int py = row + ky;
              if (px >= 0 && px < width && py >= 0 && py < height) {
                size_t idx =
                    (static_cast<size_t>(py) * static_cast<size_t>(width) * static_cast<size_t>(input_data.channels)) +
                    (static_cast<size_t>(px) * static_cast<size_t>(input_data.channels)) + static_cast<size_t>(ch);
                const auto kernel_row = static_cast<size_t>(ky + 1);
                const auto kernel_col = static_cast<size_t>(kx + 1);
                sum += static_cast<double>(input_data.pixels[idx]) * kKernel.at(kernel_row).at(kernel_col);
              }
            }
          }
          size_t out_idx =
              (static_cast<size_t>(row) * static_cast<size_t>(width) * static_cast<size_t>(input_data.channels)) +
              (static_cast<size_t>(col) * static_cast<size_t>(input_data.channels)) + static_cast<size_t>(ch);
          expected_output_[out_idx] = static_cast<uint8_t>(std::clamp(sum, 0.0, 255.0));
        }
      }
    }

    input_data_ = input_data;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    if (output_data.size() != expected_output_.size()) {
      return false;
    }

    const double tolerance = 2.0;
    for (size_t i = 0; i < expected_output_.size(); ++i) {
      if (std::abs(static_cast<int>(output_data[i]) - static_cast<int>(expected_output_[i])) > tolerance) {
        return false;
      }
    }
    return true;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
  OutType expected_output_;
};

namespace {

TEST_P(ShilinNGaussFilterVerticalSplitRunFuncTestsProcesses, ApplyGaussianFilter) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 6> kTestParam = {std::make_tuple(5, 5),   std::make_tuple(10, 10),
                                            std::make_tuple(20, 20), std::make_tuple(30, 30),
                                            std::make_tuple(50, 50), std::make_tuple(100, 100)};

const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<ShilinNGaussFilterVerticalSplitMPI, InType>(
                                               kTestParam, PPC_SETTINGS_shilin_n_gauss_filter_vertical_split),
                                           ppc::util::AddFuncTask<ShilinNGaussFilterVerticalSplitSEQ, InType>(
                                               kTestParam, PPC_SETTINGS_shilin_n_gauss_filter_vertical_split));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = ShilinNGaussFilterVerticalSplitRunFuncTestsProcesses::PrintFuncTestName<
    ShilinNGaussFilterVerticalSplitRunFuncTestsProcesses>;

INSTANTIATE_TEST_SUITE_P(GaussianFilterTests, ShilinNGaussFilterVerticalSplitRunFuncTestsProcesses, kGtestValues,
                         kPerfTestName);

}  // namespace

}  // namespace shilin_n_gauss_filter_vertical_split
