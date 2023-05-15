#!cmake .
cmake_minimum_required( VERSION 3.21 )

project( debug VERSION 1.0 DESCRIPTION "'debugging support library" LANGUAGES CXX )

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
add_library( debugging debug.cpp )
if(CMAKE_GENERATOR MATCHES "Visual Studio")
  set_source_files_properties(debugging PROPERTIES COMPILE_FLAGS -Od)
else()
  set_source_files_properties(debugging PROPERTIES COMPILE_FLAGS -O0)
endif()

# vim:syntax=cmake:nospell
