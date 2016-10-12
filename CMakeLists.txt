cmake_minimum_required(VERSION 2.8.12)
project(GDAPlanner)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(${PROJECT_NAME}_VERSION_MAJOR_0)
set(${PROJECT_NAME}_VERSION_MINOR_1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")

include_directories(
  include)

link_directories(
  )

add_executable(${PROJECT_NAME}-bin
  src/main.cpp)

add_executable(prolog_tool
  src/prolog_tool.cpp)

file(GLOB_RECURSE LIB_SOURCE "src/gdaplanner/*.cpp")
add_library(${PROJECT_NAME}
 ${LIB_SOURCE})

target_link_libraries(${PROJECT_NAME}-bin
  ${PROJECT_NAME})

target_link_libraries(prolog_tool
  ${PROJECT_NAME})


# Tests
enable_testing()

macro(AddTests)
  foreach(name ${ARGN})
    add_executable(${name} src/tests/${name}.cpp)
    target_link_libraries(${name} ${PROJECT_NAME})
    
    add_test(NAME ${name}
      COMMAND ${EXECUTABLE_OUTPUT_PATH}/${name})
  endforeach(name)
endmacro(AddTests)

AddTests(expression_equality)
AddTests(expression_negate)
AddTests(parse_pddl)
AddTests(world_assertions)
AddTests(basic_prolog_operations_and)
AddTests(basic_prolog_operations_or)
AddTests(basic_prolog_operations_equal)
AddTests(basic_prolog_operations_member)
AddTests(basic_prolog_operations_not)
AddTests(basic_prolog_operations_bound)
AddTests(basic_prolog_true_false)
AddTests(basic_prolog_world)
AddTests(compound_prolog_operations)
