#pragma once

#include "shilin_n_gauss_band_horizontal_scheme/common/include/common.hpp"
#include "task/include/task.hpp"

namespace shilin_n_gauss_band_horizontal_scheme {

class ShilinNGaussBandHorizontalSchemeSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit ShilinNGaussBandHorizontalSchemeSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace shilin_n_gauss_band_horizontal_scheme

