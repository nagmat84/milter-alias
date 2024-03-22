include_directories(${PROJECT_SOURCE_DIR}/../src)

add_executable(
	milter-alias-test
	../src/extstring.c
	../src/string_array.c
	main.c
	test_extstring.c
	test_string_array.c
)

target_compile_options(milter-alias-test PRIVATE -Wall -Wextra -fprofile-arcs -ftest-coverage)
target_link_libraries(milter-alias-test check gcov)

enable_testing()
add_test(NAME milter-alias-test COMMAND milter-alias-test)
