#include <gtest/gtest.h>

#include "shilin_n_counting_number_sentences_in_line/common/include/common.hpp"
#include "shilin_n_counting_number_sentences_in_line/mpi/include/ops_mpi.hpp"
#include "shilin_n_counting_number_sentences_in_line/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace shilin_n_counting_number_sentences_in_line {

class ShilinNCountingNumberSentencesInLinePerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 100;
  InType input_data_{};

  void SetUp() override {
    input_data_ = kCount_;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return input_data_ == output_data;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ShilinNCountingNumberSentencesInLinePerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, ShilinNCountingNumberSentencesInLineMPI, ShilinNCountingNumberSentencesInLineSEQ>(PPC_SETTINGS_shilin_n_counting_number_sentences_in_line);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ShilinNCountingNumberSentencesInLinePerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ShilinNCountingNumberSentencesInLinePerfTests, kGtestValues, kPerfTestName);

}  // namespace shilin_n_counting_number_sentences_in_line
