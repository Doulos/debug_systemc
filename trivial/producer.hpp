#pragma once

#include "transaction.hpp"
#include <systemc>

SC_MODULE( Producer_module )
{
  static constexpr const char* mesgType = "/Doulos/debugging_systemc/producer";

  // Constants
  static constexpr int depth = 6;

  sc_core::sc_export<sc_core::sc_fifo_in_if<Transaction>> data_out{"data_out"};
  explicit Producer_module( const sc_core::sc_module_name& instance ); // Constructor
  size_t transmit_count{0};
private:
  [[maybe_unused]] void producer_thread();
  sc_core::sc_fifo<Transaction> fifo{ depth };
};

