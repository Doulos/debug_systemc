#pragma once

#include <systemc>
#include <string>
#include <random>
#include "debug.hpp"

struct Processes_module : sc_core::sc_module
{
  using sc_module_name = sc_core::sc_module_name;
  static constexpr const char* mesgType = "/Doulos/navigate/processes";
  explicit Processes_module( const sc_module_name& instance );
  void before_end_of_elaboration() override;
  void end_of_elaboration() override;
  void start_of_simulation() override;
  void end_of_simulation() override;
private:
  void thread1();
  void thread2();
  void thread3();
  void method4();
  void method5();
  sc_core::sc_event event4;
  sc_core::sc_event event5;
  // Helpers
  sc_core::sc_time random_time();
  void random_delays( const std::string& func, size_t n = 1000 );
  std::random_device rd;
  std::mt19937 gen;
  std::discrete_distribution<> dist;
  Debug debug{mesgType};
};
