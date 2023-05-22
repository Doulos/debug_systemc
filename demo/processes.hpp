#pragma once

#include <systemc>
#include <string>
#include <random>
#include "debug.hpp"

struct Processes_module : sc_core::sc_module
{
  using sc_module_name = sc_core::sc_module_name;
  static constexpr const char* msg_type = "/Doulos/demo/processes";
  // Constructors and overrides
  explicit Processes_module( const sc_module_name& instance, unsigned seed = 1 );
  void before_end_of_elaboration() override;
  void end_of_elaboration() override;
  void start_of_simulation() override;
  void end_of_simulation() override;
private:
  void p1_thread();
  void p2_thread();
  void p3_thread();
  void p4_method();
  void p5_method();
  sc_core::sc_event event4;
  sc_core::sc_event event5;
  // Helpers
  sc_core::sc_time random_time();
  void random_delays( const std::string& func, size_t n = 1000 );
  Debug debug{msg_type};

  // Data
  std::random_device           true_random;
  std::mt19937                 random_generator;
  std::discrete_distribution<> time_distribution;
  size_t nRepetitions{ 1'000 };
};
