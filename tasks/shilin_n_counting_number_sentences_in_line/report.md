
Рекомендуемая структура

    Введение (краткий контекст и цель)

    Постановка задачи (определение и ограничения)

    Описание алгоритма (базового/последовательного)

    Схема распараллеливания (для MPI — топология/обмены, для потоков — декомпозиция и планирование)

    Экспериментальные результаты — Окружение (CPU/OS/компилятор/тип сборки; числа потоков/процессов) — Таблицы/графики времени; метрики ускорения/эффективности — Описание проверки корректности

    Conclusions (what worked, what didn’t)

    Источники (статьи, документация; ссылки)

    Appendix (optional code snippets)

Критерии оценки

    Полнота — все пункты из рекомендуемой структуры присутствуют и осмысленны — есть подтверждение корректности и результатов производительности

    Качество текста — ясность, связность, корректная терминология; приемлемое качество языка

    Качество оформления — корректные Markdown‑заголовки, списки, таблицы, блоки кода — рисунки/графики с подписями и относительными путями; читаемое форматирование

Примечания

    Отчёты проверяются онлайн в Pull Request. Итоговая оценка ставится по версии, прошедшей ревью.

    If the task is disabled by the end of semester (e.g., fails in master; see scoreboard “blue zone”), report points for that task are nullified.

    Плагиат запрещён; совпадающие или почти совпадающие отчёты будут отклонены.

Шаблон Markdown (report.md)

Используйте следующий скелет для вашего report.md. Пути к файлам указывайте относительно папки задачи (для изображений/данных).

# <Task Full Name>

- Student: <Last First Middle>, group <Group>
- Technology: <SEQ | MPI | OMP | TBB | STL | ALL>
- Variant: <N>

## 1. Introduction
Brief motivation, problem context, expected outcome.

## 2. Problem Statement
Formal task definition, input/output format, constraints.

## 3. Baseline Algorithm (Sequential)
Describe the base algorithm with enough detail to reproduce.

## 4. Parallelization Scheme
- For MPI: data distribution, communication pattern/topology, rank roles.
- For threads: decomposition, scheduling, synchronization.
Diagrams or short pseudocode are welcome.

## 5. Implementation Details
- Code structure (files, key classes/functions)
- Important assumptions and corner cases
- Memory usage considerations

## 6. Experimental Setup
- Hardware/OS: CPU model, cores/threads, RAM, OS version
- Toolchain: compiler, version, build type (Release/RelWithDebInfo)
- Environment: PPC_NUM_THREADS / PPC_NUM_PROC, other relevant vars
- Data: how test data is generated or sourced (relative paths)

## 7. Results and Discussion

### 7.1 Correctness
Briefly explain how correctness was verified (reference results, invariants, unit tests).

### 7.2 Performance
Present time, speedup and efficiency. Example table:

| Mode        | Count | Time, s | Speedup | Efficiency |
|-------------|-------|---------|---------|------------|
| seq         | 1     | 1.234   | 1.00    | N/A        |
| omp         | 2     | 0.700   | 1.76    | 88.0%      |
| omp         | 4     | 0.390   | 3.16    | 79.0%      |

Optionally add plots (use relative paths), and discuss bottlenecks and scalability limits.

## 8. Conclusions
Summarize findings and limitations.

## 9. References
1. <Article/Book/Doc URL>
2. <Another source>

## Appendix (Optional)
```cpp
// Short, readable code excerpts if needed
```

RunINC код MPI версии приложить в приложения 