#pragma once

#include "transaction.hpp"
#include <systemc>

SC_MODULE( Consumer_module )
{
  static constexpr const char* mesgType = "/Doulos/debugging_systemc/consumer";
  sc_core::sc_fifo_in<Transaction> data_in{"data_in"};
  explicit Consumer_module( const sc_core::sc_module_name& instance ); // Constructor
  void consumer_thread();
  size_t received_count{0};
};

