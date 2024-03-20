include_directories(${PROJECT_SOURCE_DIR}/../src)

add_executable(
	milter-alias-test
	../src/extstring.c
	extstring.c
	main.c
)
target_link_libraries(milter-alias-test check)

enable_testing()
add_test(NAME milter-alias-test COMMAND milter-alias-test)
