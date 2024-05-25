cmake_minimum_required(VERSION 3.27)

project(milter-alias)

option(BUILD_DOC "Build documentation" ON)
option(BUILD_TESTS "Build tests" OFF)

add_subdirectory(src)

if(BUILD_TESTS)
	add_subdirectory(tests)
endif()

if(BUILD_DOC)
	add_subdirectory(doc)
endif()
