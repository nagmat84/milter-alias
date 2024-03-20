cmake_minimum_required(VERSION 3.27)

project(milter-alias)

option(BUILD_DOC "Build documentation" ON)

add_subdirectory(src)
add_subdirectory(tests)

if(BUILD_DOC)
	add_subdirectory(doc)
endif()
