#!cmake .
cmake_minimum_required( VERSION 3.21 )

project( debug_systemc VERSION 1.0 DESCRIPTION "'Debugging SystemC with GDB' webinar code" LANGUAGES CXX )

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

# Provide debug
add_library( debugging src/debug.cpp )
if(CMAKE_GENERATOR MATCHES "Visual Studio")
  set_source_files_properties(debugging PROPERTIES COMPILE_FLAGS -Od)
else()
  set_source_files_properties(debugging PROPERTIES COMPILE_FLAGS -O0)
endif()

set_target( "debug_systemc" )
add_executable( "${Target}" )
target_sources( "${Target}" PRIVATE
  src/test.hpp
  src/test.cpp
  src/top.cpp
  src/top.hpp
  src/main.cpp
)
target_link_libraries( "${Target}" debugging )
add_test( NAME "${Target}-help" COMMAND "${Target}" --help )
set_tests_properties("${Target}-help" PROPERTIES PASS_REGULAR_EXPRESSION "Synopsis" )
add_test( NAME "${Target}-trace" COMMAND "${Target}" --trace )

# Test a default configuration file
set( TEST_DIR tests )
set( CONFIG_DIR "${CMAKE_BINARY_DIR}/${Target}-config" )
file( REMOVE_RECURSE "${CONFIG_DIR}" )
file( MAKE_DIRECTORY "${CONFIG_DIR}" )
file( COPY "${TEST_DIR}/default.cfg" DESTINATION "${CONFIG_DIR}/" )
file( RENAME "${CONFIG_DIR}/default.cfg" "${CONFIG_DIR}/${Target}.cfg" )
add_test( NAME "${Target}-config" WORKING_DIRECTORY "${CONFIG_DIR}" COMMAND "${Target}" -n )
add_test( NAME "${Target}-noconfig" WORKING_DIRECTORY "${CONFIG_DIR}" COMMAND "${Target}" --no-config -n )

file( COPY "${TEST_DIR}/inject.cfg" DESTINATION "${CMAKE_BINARY_DIR}" )
add_test( NAME "${Target}-inject" COMMAND "${Target}" --config inject.cfg )
set_tests_properties("${Target}-inject" PROPERTIES PASS_REGULAR_EXPRESSION "simulation FAILED" )

# vim:syntax=cmake:nospell
