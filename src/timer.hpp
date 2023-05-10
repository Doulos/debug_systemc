#pragma once

/*! Automate rough timing of performance

There are several ways to use this:

  Example1: {
    Timer t1{"work"}; //< reports when destroyed
    Do_some_hard_work();
  }

  Example2: {
    Timer t2{"manual", false}; //< require explicit calls to obtain reports
    Do_some_work();
    t2.report();
    t2.reset( "other work" ); //< set time to zero and optionally change label
    Do_some_other_work();
    t2.report();
  }

See disclaimer at end of this header.
*/

#ifndef SYSTEMC_INCLUDED
#include <iostream>
#endif
#include <string>
#include <chrono>
#include "chronout.hpp"
using namespace std::literals;
class Timer
{
public:
  using clock = std::chrono::high_resolution_clock;

  explicit Timer( std::string timerName="", bool automatic=true)
  : m_timerName { std::move(timerName) }
  , m_automatic { automatic }
  , m_timeStart { clock::now() }
  { disclaimer(); }

  void reset( const std::string& name = "" )
  {
    if( not name.empty() ) m_timerName = name;
    m_timeStart = clock::now();
  }

  [[nodiscard]]std::chrono::nanoseconds elapsed() { return clock::now() - m_timeStart; }

  [[nodiscard]]std::string to_string(const std::string& reportName="")
  {
    auto result = "Timer"s;
    if( not m_timerName.empty() ) result += "("s + m_timerName + ")"s;
    result += ": "s;
    if( not reportName.empty() ) result += reportName + " "s;
    result += "took " + Chronout::to_string(elapsed());
    return result;
  }

  void report(const std::string& reportName="") {
    #ifdef SYSTEMC_INCLUDED
    SC_REPORT_INFO_VERB( "/Doulos/timer", to_string( reportName ).c_str(), sc_core::SC_NONE );
    #else
    std::cout << to_string( reportName ) << '\n';
    #endif
  }

  ~Timer() { if ( m_automatic ) report(); }

private:
  std::string       m_timerName;
  bool              m_automatic;
  clock::time_point m_timeStart;
  static void disclaimer() {
    static bool once = false;
    if( once ) return;
    static const char* text = R"(DISCLAIMER: The Timer provides only a rough performance measure. See ABOUT_TIMER.md for details.)";
    #ifdef SYSTEMC_INCLUDED
      SC_REPORT_INFO_VERB( "/Doulos/timer", text, sc_core::SC_NONE );
    #else
      std::cout << text << '\n';
    #endif
    once = true;
  }
};

// TAGS: Doulos, SystemC, sc_main, SOURCE
// ----------------------------------------------------------------------------
//
// This file is licensed under Apache-2.0, and
// Copyright 2022 David C Black <mailto:<david.black@doulos.com>>
// See accompanying LICENSE or visit <https://www.apache.org/licenses/LICENSE-2.0.txt> for more details.
