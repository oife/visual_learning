cmake_minimum_required(VERSION 2.8)

project(student_management)

aux_source_directory(${PROJECT_BINARY_DIR}/../src SRC_LIST)

include_directories(${PROJECT_BINARY_DIR}/../include)

add_library(func_shared SHARED ${SRC_LIST})
add_library(func_static STATIC ${SRC_LIST})

set_target_properties(func_shared PROPERTIES OUTPUT_NAME "myfunc")
set_target_properties(func_static PROPERTIES OUTPUT_NAME "myfunc")

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../lib)

find_library(FUNC_LIB myfunc ${PROJECT_BINARY_DIR}/../lib) 

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)

add_executable(StudentManagement ${SRC_LIST})

target_link_libraries(StudentManagement ${FUNC_LIB})