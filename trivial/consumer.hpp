#pragma once

#include "transaction.hpp"
#include <systemc>

SC_MODULE( Consumer_module )
{
  static constexpr const char* mesgType = "/Doulos/debugging_systemc/consumer";
  sc_core::sc_fifo_in<Transaction> data_in{"data_in"};
  explicit Consumer_module( const sc_core::sc_module_name& instance ); // Constructor
  size_t count() { return m_received_count; }
private:
  [[maybe_unused]] void consumer_thread();
  sc_core::sc_signal<Transaction> m_rx{};
  size_t m_received_count{0};
};

