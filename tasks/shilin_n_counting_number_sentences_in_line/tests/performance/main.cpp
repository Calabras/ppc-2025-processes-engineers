#include <gtest/gtest.h>

#include <string>

#include "shilin_n_counting_number_sentences_in_line/common/include/common.hpp"
#include "shilin_n_counting_number_sentences_in_line/mpi/include/ops_mpi.hpp"
#include "shilin_n_counting_number_sentences_in_line/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace shilin_n_counting_number_sentences_in_line {

class ShilinNCountingNumberSentencesInLinePerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
  InType input_data_;
  OutType expected_output_ = 0;

  void SetUp() override {
    const int num_sentences = 10000;
    std::string sentence = "This is a test sentence. ";
    input_data_.clear();
    input_data_.reserve(sentence.length() * num_sentences);

    for (int i = 0; i < num_sentences; ++i) {
      input_data_ += sentence;
    }

    expected_output_ = num_sentences;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return expected_output_ == output_data;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ShilinNCountingNumberSentencesInLinePerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, ShilinNCountingNumberSentencesInLineMPI,
                                                       ShilinNCountingNumberSentencesInLineSEQ>(
    PPC_SETTINGS_shilin_n_counting_number_sentences_in_line);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ShilinNCountingNumberSentencesInLinePerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ShilinNCountingNumberSentencesInLinePerfTests, kGtestValues, kPerfTestName);

}  // namespace shilin_n_counting_number_sentences_in_line
