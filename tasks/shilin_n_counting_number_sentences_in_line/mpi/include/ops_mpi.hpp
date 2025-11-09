#pragma once

#include "shilin_n_counting_number_sentences_in_line/common/include/common.hpp"
#include "task/include/task.hpp"

namespace shilin_n_counting_number_sentences_in_line {

class ShilinNCountingNumberSentencesInLineMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit ShilinNCountingNumberSentencesInLineMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace shilin_n_counting_number_sentences_in_line
