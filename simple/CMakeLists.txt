#!cmake .
cmake_minimum_required( VERSION 3.21 )

project( example1 VERSION 1.0 DESCRIPTION "'Debugging SystemC with GDB' webinar code" LANGUAGES CXX )

#-------------------------------------------------------------------------------
# Find project working tree directory containing extern/ and cmake/)
set( _dir "${CMAKE_CURRENT_SOURCE_DIR}" )
cmake_path( GET _dir ROOT_PATH _root )
while( NOT IS_DIRECTORY "${_dir}/extern" AND NOT IS_DIRECTORY "${_dir}/cmake" )
  cmake_path( GET _dir PARENT_PATH _dir )
  if( "${_dir}" STREQUAL "${_root}" )
    message( FATAL_ERROR "Unable to find project workintg tree directory!" )
  endif()
endwhile()
set( WORKTREE_DIR "${_dir}" CACHE PATH "Points to directory containing cmake/ and extern/ subdirectories for the project." )
list( PREPEND CMAKE_MODULE_PATH "${WORKTREE_DIR}/cmake" )
include( project_defaults )

#-------------------------------------------------------------------------------
add_subdirectory( "${WORKTREE_DIR}/debug" "${CMAKE_BINARY_DIR}/lib" )

set_target( "simple" )
add_executable( "${Target}" )
target_sources( "${Target}" PRIVATE
  ${WORKTREE_DIR}/include/chronout.hpp
  ${WORKTREE_DIR}/include/timer.hpp
  ${WORKTREE_DIR}/debug/debug.hpp
  ${WORKTREE_DIR}/debug/debug.cpp
  test.hpp
  test.cpp
  top.cpp
  top.hpp
  main.cpp
)
target_include_directories( "${Target}" PRIVATE "${WORKTREE_DIR}/debug" )
target_link_libraries( "${Target}" debugging  )
add_test( NAME "${Target}-help" COMMAND "${Target}" --help )
set_tests_properties("${Target}-help" PROPERTIES PASS_REGULAR_EXPRESSION "Synopsis" )
add_test( NAME "${Target}-trace" COMMAND "${Target}" --trace )

# Test a default configuration file
set( CFG_SRC "${WORKTREE_DIR}/config" )
set( RUN_DIR "${CMAKE_BINARY_DIR}/${Target}-run" )
file( REMOVE_RECURSE "${RUN_DIR}" )
file( MAKE_DIRECTORY "${RUN_DIR}" )
file( COPY "${CFG_SRC}/default.cfg" DESTINATION "${RUN_DIR}/" )
file( RENAME "${RUN_DIR}/default.cfg" "${RUN_DIR}/${Target}.cfg" )
add_test( NAME "${Target}-config" WORKING_DIRECTORY "${RUN_DIR}" COMMAND "${Target}" -n )
add_test( NAME "${Target}-noconfig" WORKING_DIRECTORY "${RUN_DIR}" COMMAND "${Target}" --no-config -n )

file( COPY "${CFG_SRC}/inject.cfg" DESTINATION "${RUN_DIR}" )
add_test( NAME "${Target}-inject" COMMAND "${Target}" --debug --inject --trace error.vcd )
set_tests_properties("${Target}-inject" PROPERTIES PASS_REGULAR_EXPRESSION "simulation FAILED" )

# vim:syntax=cmake:nospell
