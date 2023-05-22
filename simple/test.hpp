#pragma once

#include <systemc>
#include <string>
#include "debug.hpp"
using namespace std::literals;

SC_MODULE( Test_module )
{
  static constexpr const char* msg_type = "/Doulos/debugging_systemc";
  sc_core::sc_in<bool>  clock;

  // Local channels
  sc_core::sc_fifo<sc_dt::sc_int<8>> fifo{ 6 };
  sc_core::sc_event producer_event{};

  sc_dt::sc_int<8>  producedData{ 0 };
  sc_dt::sc_int<8>  expectedData{ 0 };
  sc_dt::sc_int<8>  consumedData;

  size_t actual_cycles { 0 };
  size_t producer_rate { 2 };
  size_t producer_cycle{ 0 };
  size_t producer_total{ 0 };
  bool produce{ true };

  size_t consumer_rate { 2 };
  size_t consumer_cycle{ 0 };
  size_t consumer_total{ 0 };
  size_t consumer_stop { 1'000 };
  bool consume{ false };

  explicit Test_module( const sc_core::sc_module_name& nm ); // Constructor
  void producer_thread();
  void observer_thread();
  void consumer_method();
  void start_of_simulation() override;
  void end_of_simulation() override;

};
