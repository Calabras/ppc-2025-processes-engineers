#include "shilin_n_counting_number_sentences_in_line/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <string>

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

  // Получаем граничный символ слева от текущего чанка
  // (последний символ предыдущего чанка)
  char left_boundary_char = '\0';
  if (start_pos > 0) {
    left_boundary_char = input_str[start_pos - 1];
  }

  // Проверяем, является ли символ знаком препинания
  auto is_punctuation = [](char c) { return c == '.' || c == '!' || c == '?'; };

  int local_count = 0;

  // Если первый символ чанка - знак препинания и левый граничный символ тоже
  // знак препинания, то это продолжение последовательности из предыдущего чанка
  // Нужно пропустить все последовательные знаки препинания в начале чанка
  if (start_pos > 0 && is_punctuation(left_boundary_char) && start_pos < input_length &&
      is_punctuation(input_str[start_pos])) {
    // Пропускаем все последовательные знаки препинания в начале чанка
    int i = start_pos;
    while (i < end_pos && is_punctuation(input_str[i])) {
      ++i;
    }
    // Начинаем подсчет с позиции после пропущенных знаков препинания
    for (; i < end_pos; ++i) {
      char ch = input_str[i];
      if (is_punctuation(ch)) {
        local_count++;
        // Пропускаем все последующие последовательные знаки препинания
        while (i + 1 < end_pos && is_punctuation(input_str[i + 1])) {
          ++i;
        }
      }
    }
  } else {
    // Обычный подсчет, начинаем с начала чанка
    for (int i = start_pos; i < end_pos; ++i) {
      char ch = input_str[i];
      if (is_punctuation(ch)) {
        local_count++;
        // Пропускаем все последующие последовательные знаки препинания
        while (i + 1 < end_pos && is_punctuation(input_str[i + 1])) {
          ++i;
        }
      }
    }
  }

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
