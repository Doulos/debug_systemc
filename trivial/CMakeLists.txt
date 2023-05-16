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

set_target( "app" )
add_executable( "${Target}" )
target_sources( "${Target}" PRIVATE
# Following should/will eventually be in a library
  ${WORKTREE_DIR}/include/chronout.hpp
  ${WORKTREE_DIR}/include/timer.hpp
  ${WORKTREE_DIR}/include/objection.hpp
  ${WORKTREE_DIR}/debug/debug.hpp
  ${WORKTREE_DIR}/debug/debug.cpp
# Design to debug
  producer.hpp
  producer.cpp
  consumer.hpp
  consumer.cpp
  top.cpp
  top.hpp
  main.cpp
)
add_test( NAME "${Target}-help" COMMAND "${Target}" --help )
set_tests_properties("${Target}-help" PROPERTIES PASS_REGULAR_EXPRESSION "Synopsis" )
add_test( NAME "${Target}-noargs" COMMAND "${Target}" )
add_test( NAME "${Target}-badargs" COMMAND "${Target}" --warn --werror --Goofy )
set_tests_properties("${Target}-badargs" PROPERTIES PASS_REGULAR_EXPRESSION "simulation FAILED" )

add_test( NAME "${Target}-debug" COMMAND "${Target}" --warn --debug --tPeriod=10ns --nReps=7 --nDump=2)
#set_tests_properties("${Target}-inject" PROPERTIES PASS_REGULAR_EXPRESSION "simulation FAILED" )

# vim:syntax=cmake:nospell
