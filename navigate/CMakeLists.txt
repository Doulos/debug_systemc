#!cmake .
cmake_minimum_required( VERSION 3.21 )

project( navigate VERSION 1.0 DESCRIPTION "application 1" LANGUAGES CXX )

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
add_executable( nav )
target_include_directories( nav PRIVATE
  "${WORKTREE_DIR}/include"
  "${WORKTREE_DIR}/debug"
)
target_sources( nav PRIVATE
  "${WORKTREE_DIR}/include/objection.hpp"
  "${WORKTREE_DIR}/debug/debug.hpp"
  "${WORKTREE_DIR}/debug/debug.cpp"
  "processes.cpp"
  "processes.hpp"
  "top.hpp"
  "main.cpp"
)
add_test( NAME "navigate-help" COMMAND nav --help )
add_test( NAME "navigate-run" COMMAND nav --debug --nReps=20 )
add_test( NAME "navigate-error" COMMAND nav --warn --werror -whoops )
set_tests_properties("navigate-error" PROPERTIES PASS_REGULAR_EXPRESSION "Simulation FAILED" )

# vim:syntax=cmake:nospell
