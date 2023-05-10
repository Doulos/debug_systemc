#!cmake .
cmake_minimum_required ( VERSION 3.20 )

include_guard( GLOBAL )

message( "Setting up cmake" )

include( policy )

if( NOT DEFINED ENV{SYSTEMC_HOME} )
  # Attempt to find it
  set( SYSTEMC_CANDIDATES "$ENV{HOME}/.local/apps/systemc;$ENV{HOME}/systemc" )
  foreach( _dir ${SYSTEMC_CANDIDATES} )
    if( IS_DIRECTORY "${_dir}/include" )
      set( ENV{SYSTEMC_HOME} "${_dir}" )
      break()
    endif()
  endforeach()
endif()
if( NOT DEFINED ENV{SYSTEMC_HOME} )
  message( FATAL_ERROR "SYSTEMC_HOME environment variable needs to be setup" )
endif()

include( systemc )

# Simplify life
add_compile_definitions( SC_INCLUDE_FX SC_INCLUDE_DYNAMIC_PROCESSES )

# vim:nospell
