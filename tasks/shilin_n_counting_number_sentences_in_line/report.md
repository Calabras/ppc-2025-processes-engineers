# Подсчет числа предложений в строке

- Student: Шилин Никита Дмитриевич, group 3823Б1ПР1
- Technology: MPI + SEQ
- Variant: 25

## 1. Introduction

Обработка текста является одной из фундаментальных задач в области компьютерных наук. Подсчет предложений в тексте - базовая операция, используемая в системах обработки естественного языка, анализаторах текста, системах машинного перевода и других приложениях.
Цель данной работы - реализовать эффективный алгоритм подсчета предложений в строке с использованием последовательного подхода (SEQ) и параллельного подхода на основе MPI (Message Passing Interface), а также провести сравнительный анализ производительности обоих подходов.

## 2. Problem Statement

**Задача:** Подсчитать количество предложений в заданной строке.

**Входные данные:**
- Строка произвольной длины (тип `std::string`)
- Может содержать текст на любом языке
- Может быть пустой строкой

**Выходные данные:**
- Целое число (тип `int`) - количество предложений в строке

**Определение предложения:**
Предложение считается завершенным, если оно заканчивается одним из следующих знаков препинания:
- Точка (`.`)
- Восклицательный знак (`!`)
- Вопросительный знак (`?`)

**Особенности:**
- Последовательные знаки препинания (например, `...` или `!!!`) считаются как одно окончание предложения
- Строка без завершающих знаков препинания содержит 0 предложений
- Пустая строка содержит 0 предложений

**Ограничения:**
- Длина строки ограничена доступной памятью
- Алгоритм должен корректно обрабатывать Unicode-символы (UTF-8)

## 3. Baseline Algorithm (Sequential)

Последовательный алгоритм выполняет линейный проход по строке:

**Псевдокод:**
```
function CountSentences(input_string):
    count = 0
    i = 0
    
    while i < length(input_string):
        ch = input_string[i]
        
        if ch in {'.', '!', '?'}:
            count++
            
            // Skip consecutive punctuation marks
            while (i+1 < length(input_string)) and 
                  (input_string[i+1] in {'.', '!', '?'}):
                i++
        
        i++
    
    return count
```

**Сложность:**
- Временная: O(n), где n - длина строки
- Пространственная: O(1) - используется только счетчик

## 4. Parallelization Scheme

### MPI Подход

Параллелизация основана на декомпозиции данных - строка разделяется между процессами.

**Схема распределения данных:**
1. **Rank 0** (главный процесс):
   - Получает входную строку
   - Вычисляет её длину
   - Распространяет длину на все процессы (`MPI_Bcast`)
   - Распространяет саму строку на все процессы (`MPI_Bcast`)

2. **Все процессы (0 to size-1)**:
   - Вычисляют границы своего фрагмента строки:
     - `chunk_size = string_length / num_processes`
     - `remainder = string_length % num_processes`
     - Процессы с `rank < remainder` получают на 1 символ больше
   - Обрабатывают свой фрагмент независимо
   - Подсчитывают предложения в своем фрагменте

3. **Сбор результатов**:
   - Используется `MPI_Reduce` с операцией `MPI_SUM` для суммирования локальных счетчиков
   - Результат собирается в rank 0
   - Результат распространяется на все процессы через `MPI_Bcast` для валидации

**Коммуникационный паттерн:**
```
Rank 0:  [Input String] --Bcast--> All ranks
All ranks: Count locally
All ranks: --Reduce(SUM)--> Rank 0
Rank 0:  [Result] --Bcast--> All ranks
```

**Топология:** Стандартная MPI_COMM_WORLD (все процессы связаны)

**Особенности:**
- Используется равномерное распределение данных
- При нечетном делении первые процессы получают на 1 символ больше
- Каждый процесс работает независимо, минимизируя коммуникацию

## 5. Implementation Details

### Структура кода

**Файлы:**
- `common/include/common.hpp` - определение типов данных
- `seq/include/ops_seq.hpp`, `seq/src/ops_seq.cpp` - последовательная реализация
- `mpi/include/ops_mpi.hpp`, `mpi/src/ops_mpi.cpp` - параллельная реализация MPI
- `tests/functional/main.cpp` - функциональные тесты (15 тестовых случаев)
- `tests/performance/main.cpp` - тесты производительности

**Ключевые классы:**
- `ShilinNCountingNumberSentencesInLineSEQ` - последовательная версия
- `ShilinNCountingNumberSentencesInLineMPI` - параллельная версия MPI

**Методы (для обеих версий):**
- `ValidationImpl()` - проверка корректности входных/выходных данных
- `PreProcessingImpl()` - инициализация выходных данных
- `RunImpl()` - основной алгоритм подсчета
- `PostProcessingImpl()` - финализация (не требуется для данной задачи)

### Важные предположения и граничные случаи

1. **Пустая строка** - возвращает 0
2. **Строка без знаков препинания** - возвращает 0
3. **Множественная пунктуация** (`...`, `!!!`, `???`) - считается как одно предложение
4. **Смешанная пунктуация** (`.!?`) - считается как одно предложение
5. **Короткие строки** - эффективно обрабатываются в SEQ режиме

### Использование памяти

- **SEQ версия:** O(1) дополнительной памяти (только счетчик)
- **MPI версия:** O(n) на каждом процессе, где n - длина исходной строки
  - Вся строка копируется на каждый процесс для упрощения логики
  - Альтернатива с распределенной памятью возможна, но усложняет обработку границ

## 6. Experimental Setup

### Аппаратное и программное окружение

**Аппаратное обеспечение:**
- CPU: Apple M1/M2 (ARM64 архитектура)
- Cores: 8 cores (4 performance + 4 efficiency)
- RAM: 16 GB
- OS: macOS Sequoia 15.4

**Программное обеспечение:**
- Compiler: AppleClang 17.0.0.17000013
- MPI Implementation: Open MPI 5.0.8
- Build Type: Release
- CMake: 3.27.3

**Переменные окружения:**
```bash
export PPC_NUM_PROC=4
export OMPI_MCA_pmix_tool_enable=0
```

**Тестовые данные:**
- Функциональные тесты: 15 различных строк с известными результатами
- Тесты производительности: строка из 10,000 повторяющихся предложений (~250,000 символов)

## 7. Results and Discussion

### 7.1 Correctness

Корректность реализации проверена следующими способами:

1. **Функциональные тесты (30 тестов):**
   - 15 различных входных строк
   - Каждая строка тестируется в SEQ и MPI режимах
   - Все тесты пройдены успешно (100%)

2. **Тестовые случаи включают:**
   - Пустые строки
   - Строки без предложений
   - Одно предложение
   - Множественные предложения (до 10)
   - Различные знаки препинания
   - Множественная пунктуация
   - Граничные случаи

3. **Сравнение результатов:**
   - Результаты SEQ и MPI версий идентичны для всех тестовых случаев
   - MPI версия с разным количеством процессов (1, 2, 4) дает одинаковые результаты

**Пример верификации:**
```
Input: "Hello! How are you?"
Expected: 2
SEQ result: 2 ✓
MPI result (4 processes): 2 ✓
```

### 7.2 Performance

Результаты тестов производительности на строке из 10,000 предложений (~250,000 символов):

| Mode        | Processes | Time (s)  | Speedup | Efficiency |
|-------------|-----------|-----------|---------|------------|
| SEQ         | 1         | 0.000154  | 1.00    | N/A        |
| MPI pipeline| 4         | 0.003924  | 0.04    | 1.0%       |
| MPI task_run| 4         | 0.000307  | 0.50    | 12.5%      |

**Наблюдения:**

1. **SEQ версия быстрее для данного размера данных:**
   - Overhead MPI (коммуникация) превышает выгоду от параллелизма
   - Время на `MPI_Bcast` строки значительное (~0.003 сек в pipeline режиме)

2. **MPI task_run vs pipeline:**
   - `task_run` в 13x быстрее `pipeline` за счет меньших накладных расходов
   - Pipeline режим включает дополнительные операции инициализации

3. **Масштабируемость:**
   - Для коротких строк (< 1 млн символов) SEQ версия оптимальна
   - MPI версия будет эффективна для очень больших текстов (> 10 млн символов)
   - Соотношение вычислений к коммуникации низкое для данной задачи

**Узкие места:**
- Коммуникационные накладные расходы (broadcast строки)
- Последовательное чтение результата не оптимизировано
- Отсутствие кэш-локальности в MPI версии

## 8. Conclusions

В данной работе реализованы последовательная (SEQ) и параллельная (MPI) версии алгоритма подсчета предложений в строке.

**Основные результаты:**
1. Обе версии работают корректно на всех тестовых случаях (30/30 тестов пройдено)
2. SEQ версия показывает лучшую производительность на строках умеренного размера
3. MPI версия имеет высокие коммуникационные накладные расходы

**Что сработало:**
- Простой и эффективный алгоритм последовательной обработки
- Корректная обработка множественной пунктуации
- Равномерное распределение данных в MPI версии

**Что не сработало / требует улучшения:**
- MPI версия не показала ускорения из-за высоких коммуникационных издержек
- Для эффективного использования MPI нужны значительно большие объемы данных
- Возможна оптимизация: обработка потоком данных без полной репликации строки

**Рекомендации:**
- Для строк < 1 млн символов использовать SEQ версию
- Для очень больших текстов или множественных строк - использовать MPI
- Рассмотреть гибридный подход (MPI + OpenMP) для лучшей масштабируемости

## 9. References

1. [MPI Standard Documentation](https://www.mpi-forum.org/docs/) - официальная документация MPI
2. [Open MPI Documentation](https://www.open-mpi.org/doc/) - документация Open MPI 5.0
3. [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) - руководство по стилю кода
4. Гергель В.П. "Высокопроизводительные вычисления для многоядерных многопроцессорных систем" - учебное пособие
5. [PPC Course Repository](https://github.com/learning-process/ppc-2025-processes-engineers) - репозиторий курса

## Appendix: MPI Implementation

### RunImpl() - основной алгоритм MPI версии

```cpp
bool ShilinNCountingNumberSentencesInLineMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  std::string input_str;
  int input_length = 0;

  // Rank 0 broadcasts the length of the input string
  if (rank == 0) {
    input_str = GetInput();
    input_length = static_cast<int>(input_str.length());
  }

  MPI_Bcast(&input_length, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // If empty string, result is 0
  if (input_length == 0) {
    if (rank == 0) {
      GetOutput() = 0;
    }
    return true;
  }

  // Prepare buffer for all ranks
  if (rank != 0) {
    input_str.resize(input_length);
  }

  // Broadcast the entire string to all processes
  MPI_Bcast(input_str.data(), input_length, MPI_CHAR, 0, MPI_COMM_WORLD);

  // Calculate chunk size for each process
  int chunk_size = input_length / size;
  int remainder = input_length % size;

  // Determine start and end for this rank
  int start_pos = rank * chunk_size + std::min(rank, remainder);
  int end_pos = start_pos + chunk_size + (rank < remainder ? 1 : 0);

  // Count sentences in local chunk
  int local_count = 0;
  for (int i = start_pos; i < end_pos; ++i) {
    char ch = input_str[i];
    if (ch == '.' || ch == '!' || ch == '?') {
      local_count++;
      // Skip consecutive punctuation marks
      while (i + 1 < end_pos && 
             (input_str[i + 1] == '.' || 
              input_str[i + 1] == '!' || 
              input_str[i + 1] == '?')) {
        ++i;
      }
    }
  }

  // Gather results to rank 0
  int global_count = 0;
  MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  // Broadcast result to all processes for validation
  if (rank == 0) {
    GetOutput() = global_count;
  }
  MPI_Bcast(&global_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
  
  if (rank != 0) {
    GetOutput() = global_count;
  }

  return true;
}
```

### Ключевые MPI операции

1. **MPI_Bcast** (broadcast):
   - Распространение длины строки: `MPI_Bcast(&input_length, 1, MPI_INT, 0, MPI_COMM_WORLD)`
   - Распространение строки: `MPI_Bcast(input_str.data(), input_length, MPI_CHAR, 0, MPI_COMM_WORLD)`
   - Распространение результата: `MPI_Bcast(&global_count, 1, MPI_INT, 0, MPI_COMM_WORLD)`

2. **MPI_Reduce** (reduction):
   - Суммирование локальных счетчиков: `MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD)`

### Распределение данных (пример для 4 процессов, строка длины 10)

```
String: "ABCDEFGHIJ" (length = 10)
Processes: 4

chunk_size = 10 / 4 = 2
remainder = 10 % 4 = 2

Rank 0: positions [0, 3)  -> "ABC" (chunk_size + 1)
Rank 1: positions [3, 6)  -> "DEF" (chunk_size + 1)
Rank 2: positions [6, 8)  -> "GH"  (chunk_size)
Rank 3: positions [8, 10) -> "IJ"  (chunk_size)
```

### Полная реализация ValidationImpl

```cpp
bool ShilinNCountingNumberSentencesInLineMPI::ValidationImpl() {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    // Input can be any string (including empty)
    // Output should be initialized to 0
    return GetOutput() == 0;
  }
  return true;
}
```

### Полная реализация PreProcessingImpl

```cpp
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
```

### Тестовые случаи (примеры из functional tests)

```cpp
const std::array<TestType, 15> kTestParam = {
    std::make_tuple("Hello world.", "1"),
    std::make_tuple("Hello! How are you?", "2"),
    std::make_tuple("This is a test. Another sentence! And one more?", "3"),
    std::make_tuple("", "0"),
    std::make_tuple("No sentences here", "0"),
    std::make_tuple("One. Two. Three.", "3"),
    std::make_tuple("Multiple punctuation...!!!", "1"),
    std::make_tuple("Mix. Of! Different? Endings.", "4"),
    std::make_tuple("Only dots...", "1"),
    std::make_tuple("Single! Exclamation!", "2"),
    std::make_tuple("Question? Answer! Statement.", "3"),
    std::make_tuple("Wow!!! Amazing!!! Great!!!", "3"),
    std::make_tuple("A.B.C.D.E.F.G.H.I.J.", "10"),
    std::make_tuple("Just one very long sentence without any ending", "0"),
    std::make_tuple("Start. Middle! End?", "3")
};
```
