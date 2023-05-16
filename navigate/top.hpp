#pragma once

#include "processes.hpp"
#include <systemc>

struct Top_module : sc_core::sc_module
{
  explicit Top_module( const sc_core::sc_module_name& instance )
    : sc_module( instance )
  {
    // No interconnect for this design
  }
  Processes_module processes{"processes"};
};
