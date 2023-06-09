#!cmake .
cmake_minimum_required( VERSION 3.21 )

project( debugging_demo VERSION 1.0 DESCRIPTION "application 1" LANGUAGES CXX )

# Find project working tree directory containing cmake/project_defaults.cmake
# - this directory will be at the current location on in a parent directory
set( _dir "${CMAKE_CURRENT_SOURCE_DIR}" )
cmake_path( GET _dir ROOT_PATH _root )
while( NOT EXISTS "${_dir}/cmake/project_defaults.cmake" )
  cmake_path( GET _dir PARENT_PATH _dir )
  if( "${_dir}" STREQUAL "${_root}" )
    message( FATAL_ERROR "Unable to find project workintg tree directory!" )
  endif()
endwhile()
set( WORKTREE_DIR "${_dir}" CACHE PATH "Points to directory containing project_defaults.cmake for the project." )
message( "Info: WORKTREE_DIR='${WORKTREE_DIR}'" )
list( PREPEND CMAKE_MODULE_PATH "${WORKTREE_DIR}/cmake" )
include( project_defaults )


#-------------------------------------------------------------------------------
# Compile the app
add_executable( demo )
target_include_directories( demo PRIVATE
  "${WORKTREE_DIR}/include"
  "${WORKTREE_DIR}/debug"
)
target_sources( demo PRIVATE
  "${WORKTREE_DIR}/include/objection.hpp"
  "${WORKTREE_DIR}/debug/debug.hpp"
  "${WORKTREE_DIR}/debug/debug.cpp"
  "processes.cpp"
  "processes.hpp"
  "top.hpp"
  "main.cpp"
)
add_test( NAME "demo-help"  COMMAND demo --help )
add_test( NAME "demo-run"   COMMAND demo --debug --nReps=3 --trace)
add_test( NAME "demo-error" COMMAND demo --warn --werror -whoops )
set_tests_properties("demo-error" PROPERTIES PASS_REGULAR_EXPRESSION "Simulation FAILED" )

# vim:syntax=cmake:nospell
