if(MSVC AND NOT (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
  set(OpenMP_C_FLAGS
      "/openmp:llvm"
      CACHE STRING "OpenMP C flags" FORCE)
  set(OpenMP_CXX_FLAGS
      "/openmp:llvm"
      CACHE STRING "OpenMP CXX flags" FORCE)
  # Pretend Clang-style OpenMP 5.0 support so CMake reports a newer version
  set(OpenMP_C_SPEC_DATE
      201811
      CACHE STRING "OpenMP C specification date" FORCE)
  set(OpenMP_CXX_SPEC_DATE
      201811
      CACHE STRING "OpenMP CXX specification date" FORCE)
endif()

# macOS with Apple Clang: find libomp from Homebrew
if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  find_program(HOMEBREW_BREW brew)
  if(HOMEBREW_BREW)
    execute_process(
      COMMAND ${HOMEBREW_BREW} --prefix libomp
      OUTPUT_VARIABLE LIBOMP_PREFIX
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )
    if(LIBOMP_PREFIX AND EXISTS "${LIBOMP_PREFIX}/lib/libomp.dylib")
      set(OpenMP_C_FLAGS
          "-Xpreprocessor -fopenmp -I${LIBOMP_PREFIX}/include"
          CACHE STRING "OpenMP C flags" FORCE)
      set(OpenMP_CXX_FLAGS
          "-Xpreprocessor -fopenmp -I${LIBOMP_PREFIX}/include"
          CACHE STRING "OpenMP CXX flags" FORCE)
      set(OpenMP_C_LIB_NAMES
          "omp"
          CACHE STRING "OpenMP C library names" FORCE)
      set(OpenMP_CXX_LIB_NAMES
          "omp"
          CACHE STRING "OpenMP CXX library names" FORCE)
      set(OpenMP_omp_LIBRARY
          "${LIBOMP_PREFIX}/lib/libomp.dylib"
          CACHE FILEPATH "OpenMP omp library" FORCE)
      set(OpenMP_libomp_LIBRARY
          "${LIBOMP_PREFIX}/lib/libomp.dylib"
          CACHE FILEPATH "OpenMP libomp library" FORCE)
    endif()
  endif()
endif()

function(ppc_link_threads exec_func_lib)
  target_link_libraries(${exec_func_lib} PUBLIC Threads::Threads)
endfunction()

function(ppc_link_openmp exec_func_lib)
  find_package(OpenMP REQUIRED)
  target_link_libraries(${exec_func_lib} PUBLIC ${OpenMP_libomp_LIBRARY}
                                                OpenMP::OpenMP_CXX)
endfunction()
