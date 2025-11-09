#pragma once

#include "example_processes/common/include/common.hpp"
#include "task/include/task.hpp"

namespace shilin_n_counting_number_sentences_in_line {

class NesterovATestTaskMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit NesterovATestTaskMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace shilin_n_counting_number_sentences_in_line
