#pragma once

#include "shilin_n_gauss_filter_vertical_split/common/include/common.hpp"
#include "task/include/task.hpp"

namespace shilin_n_gauss_filter_vertical_split {

class ShilinNGaussFilterVerticalSplitMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit ShilinNGaussFilterVerticalSplitMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

  static void DistributeVerticalStripes(const std::vector<uint8_t> &input, std::vector<uint8_t> &local_data, int width,
                                        int height, int channels, int rank, int size, int &local_width,
                                        int &local_start_col);
  static void SendDataToProcess(const std::vector<uint8_t> &input, int dest, int width, int height, int channels,
                                int base_cols_per_proc, int remainder);
  static void CopyLocalData(const std::vector<uint8_t> &input, std::vector<uint8_t> &local_data, int local_start_col,
                            int local_width, int width, int height, int channels);
  static void ApplyGaussianKernelMPI(const std::vector<uint8_t> &local_input, std::vector<uint8_t> &local_output,
                                     int local_width, int local_start_col, int width, int height, int channels);
  static void GatherVerticalStripes(const std::vector<uint8_t> &local_data, std::vector<uint8_t> &output, int width,
                                    int height, int channels, int rank, int size, int local_width, int local_start_col);
};

}  // namespace shilin_n_gauss_filter_vertical_split
