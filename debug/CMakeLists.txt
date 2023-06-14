#!cmake .
cmake_minimum_required( VERSION 3.21 )

project( debug VERSION 1.0 DESCRIPTION "'debugging support library" LANGUAGES CXX )

#-------------------------------------------------------------------------------
# Find project directory containing defaults
#-------------------------------------------------------------------------------
set( defaults "project_defaults" )
set( _dir "${CMAKE_CURRENT_SOURCE_DIR}" )
cmake_path( GET _dir ROOT_PATH _root )
while( NOT EXISTS "${_dir}/cmake/${defaults}.cmake" )
  cmake_path( GET _dir PARENT_PATH _dir )
  if( "${_dir}" STREQUAL "${_root}" )
    message( FATAL_ERROR "Unable to find project working tree directory!" )
  endif()
endwhile()
set( WORKTREE_DIR "${_dir}" CACHE PATH "Contains cmake/${defaults}.cmake" )
list( PREPEND CMAKE_MODULE_PATH "${WORKTREE_DIR}/cmake" )
include( "${defaults}" )

#-------------------------------------------------------------------------------
# Provide debug
add_library( debugaid)
target_sources( debugaid
  PUBLIC
  ${WORKTREE_DIR}/include/report.hpp
  debug.hpp 
  PRIVATE
  debug.cpp 
)
set_target_properties( debugaid PROPERTIES PUBLIC_HEADER debug.hpp )
target_sources( debugaid PUBLIC debug.hpp PRIVATE debug.cpp )
if(CMAKE_GENERATOR MATCHES "Visual Studio")
  set_source_files_properties(debugaid PROPERTIES COMPILE_FLAGS -Od)
else()
  set_source_files_properties(debugaid PROPERTIES COMPILE_FLAGS -O0)
endif()
install (TARGETS debugaid
  LIBRARY DESTINATION "${WORKTREE_DIR}/shared/lib"
  ARCHIVE DESTINATION "${WORKTREE_DIR}/shared/lib"
  INCLUDES DESTINATION "${WORKTREE_DIR}/shared/include"
  PUBLIC_HEADER DESTINATION "${WORKTREE_DIR}/shared/include"
  )

#-------------------------------------------------------------------------------
# Test the features
add_executable( test_debug )
target_link_libraries( test_debug PRIVATE debugaid )
target_sources( test_debug
  PRIVATE
  test_debug.cpp
)
add_test( NAME test-raw      COMMAND test_debug --nGrade=100 -sName=Dan -tStart=20sec -fMember=yes )
add_test( NAME test-help     COMMAND test_debug --help )
add_test( NAME test-trace    COMMAND test_debug --trace dump --nGrade=80 )
add_test( NAME test-error    COMMAND test_debug --werror --warn Whoops )
set_tests_properties(test-error PROPERTIES PASS_REGULAR_EXPRESSION "Simulation FAILED" )
add_test( NAME test-expect   COMMAND test_debug --expect=1 --nGrade=80 )
add_test( NAME test-surprize COMMAND test_debug --expect=2 --nGrade=80 )
set_tests_properties(test-surprize PROPERTIES PASS_REGULAR_EXPRESSION "Simulation FAILED" )
add_test( NAME test-values   COMMAND test_debug -n --nCount=5 --tDelay=4_ns --sName="Hello" --fValid=off )
add_test( NAME test-config   COMMAND test_debug --config "${WORKTREE_DIR}/debug/test_debug.cfg" )

#-------------------------------------------------------------------------------
# vim:syntax=cmake:nospell
