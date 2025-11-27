#include "shilin_n_counting_number_sentences_in_line/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>

#include "shilin_n_counting_number_sentences_in_line/common/include/common.hpp"

namespace shilin_n_counting_number_sentences_in_line {

ShilinNCountingNumberSentencesInLineMPI::ShilinNCountingNumberSentencesInLineMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    GetInput() = in;
  }
  GetOutput() = 0;
}

bool ShilinNCountingNumberSentencesInLineMPI::ValidationImpl() {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    return GetOutput() == 0;
  }
  return true;
}

bool ShilinNCountingNumberSentencesInLineMPI::PreProcessingImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (rank == 0) {
    GetOutput() = 0;
  }
  return true;
}

namespace {

bool IsPunctuation(char c) {
  return c == '.' || c == '!' || c == '?';
}
}

int ShilinNCountingNumberSentencesInLineMPI::SkipPunctuationSequence(const std::string &input_str, int start_pos,
                                                                     int end_pos) {
  int pos = start_pos;
  while (pos < end_pos && IsPunctuation(input_str[static_cast<size_t>(pos)])) {
    ++pos;
  }
  return pos;
}

int ShilinNCountingNumberSentencesInLineMPI::CountSentencesFromPosition(const std::string &input_str, int start_pos,
                                                                        int end_pos) {
  int count = 0;
  for (int i = start_pos; i < end_pos; ++i) {
    if (IsPunctuation(input_str[static_cast<size_t>(i)])) {
      count++;
      i = SkipPunctuationSequence(input_str, i + 1, end_pos) - 1;
    }
  }
  return count;
}

int ShilinNCountingNumberSentencesInLineMPI::CountSentencesInChunk(const std::string &input_str, int start_pos,
                                                                   int end_pos, char left_boundary_char) {
  const bool skip_start = start_pos > 0 && IsPunctuation(left_boundary_char) &&
                          std::cmp_less(start_pos, input_str.length()) &&
                          IsPunctuation(input_str[static_cast<size_t>(start_pos)]);

  if (skip_start) {
    const int new_start_pos = SkipPunctuationSequence(input_str, start_pos, end_pos);
    return CountSentencesFromPosition(input_str, new_start_pos, end_pos);
  }

  return CountSentencesFromPosition(input_str, start_pos, end_pos);
}

bool ShilinNCountingNumberSentencesInLineMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  std::string input_str;
  int input_length = 0;

  if (rank == 0) {
    input_str = GetInput();
    input_length = static_cast<int>(input_str.length());
  }

  MPI_Bcast(&input_length, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (input_length == 0) {
    if (rank == 0) {
      GetOutput() = 0;
    }
    return true;
  }

  if (rank != 0) {
    input_str.resize(input_length);
  }

  MPI_Bcast(input_str.data(), input_length, MPI_CHAR, 0, MPI_COMM_WORLD);

  int chunk_size = input_length / size;
  int remainder = input_length % size;

  int start_pos = (rank * chunk_size) + std::min(rank, remainder);
  int end_pos = start_pos + chunk_size + (rank < remainder ? 1 : 0);

  char left_boundary_char = '\0';
  if (start_pos > 0) {
    left_boundary_char = input_str[static_cast<size_t>(start_pos - 1)];
  }

  int local_count = CountSentencesInChunk(input_str, start_pos, end_pos, left_boundary_char);

  int global_count = 0;
  MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Bcast(&global_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

  GetOutput() = global_count;
  return true;
}

bool ShilinNCountingNumberSentencesInLineMPI::PostProcessingImpl() {
  return true;
}

}  // namespace shilin_n_counting_number_sentences_in_line
