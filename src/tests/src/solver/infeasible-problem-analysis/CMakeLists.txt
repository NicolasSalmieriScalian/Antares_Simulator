add_executable(test-unfeasible-problem-analyzer)
target_sources(test-unfeasible-problem-analyzer
		PRIVATE
		test-unfeasible-problem-analyzer.cpp)
target_link_libraries(test-unfeasible-problem-analyzer
		PRIVATE
		Boost::unit_test_framework
		infeasible_problem_analysis
)

# TODO: this is necessary so that windows can find the DLL without running "cmake --install"
#       Is there a better way to achieve this ?
copy_dependency(sirius_solver test-unfeasible-problem-analyzer)

add_test(NAME test-unfeasible-problem-analyzer COMMAND test-unfeasible-problem-analyzer)

# Storing the executable under the folder Unit-tests in Visual Studio
set_target_properties(test-unfeasible-problem-analyzer PROPERTIES FOLDER Unit-tests)
set_property(TEST test-unfeasible-problem-analyzer PROPERTY LABELS unit)
