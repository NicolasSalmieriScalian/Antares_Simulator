set(SRC_INFEASIBLE_PROBLEM_ANALYSIS
  infeasible-problem-analysis/problem.cpp
  infeasible-problem-analysis/problem.h
  infeasible-problem-analysis/report.h
  infeasible-problem-analysis/report.cpp
  infeasible-problem-analysis/constraint.h
  infeasible-problem-analysis/constraint.cpp
  infeasible-problem-analysis/exceptions.h
  infeasible-problem-analysis/exceptions.cpp
  )

add_library(infeasible_problem_analysis ${SRC_INFEASIBLE_PROBLEM_ANALYSIS})
target_link_libraries(infeasible_problem_analysis
  PUBLIC ortools::ortools sirius_solver
  PRIVATE libantares-core # antares/study/OrtoolsSolver.cpp
  )