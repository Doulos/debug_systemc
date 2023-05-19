#pragma once

/**
 * @file   objection.hpp
 * @brief  Stop SystemC simulation when the last objection is removed.
 * The idea behind this class mirrors the concept of objections in the UVM
 * methodology. This particular implementation uses RAII in concert with
 * a Singleton model. So you instantiate an objection object on the stack
 * inside a region where activity occurs. The constructor immediately raises
 * an object. When you leave that region, and the objection is destroyed, it
 * will automatically drop via the destructor. As with UVM, There is both
 * a time-out and a drain-time that can be set via static methods. When all
 * objections have dropped and the drain time has expired then sc_stop is
 * called.
 *
 * @notes
 *
 * @function Objection::set_drainTime defaults to SC_ZERO_TIME
 * @function Objection::set_maxTimeout will cause timeout monitoring to start if called with a non-zero time. It can be set multiple times.
 *
 */

#include <systemc>
#include <string>
#include <set>
#include <utility>
using namespace std::literals;

struct Objection
{

  /* @method Objection creates and raises an objection
   * @parameter name specifies the purpose of this objection to aid debug.
   * @parameter verbose causes all state changes to be dumped. Defaults to false.
   */
  explicit Objection( const std::string& name, sc_core::sc_verbosity verbosity = sc_core::SC_HIGH, bool quiet = false ) ///< Create an objection
  : m_name( name ), m_verbosity_level( verbosity ), m_quiet( quiet )
  {
    sc_assert( not name.empty() and s_objections.count( m_name ) == 0 ); // name required and must be unique
    s_objections.emplace( m_name );
    if( not get_quiet() )
      SC_REPORT_INFO_VERB( mesgType
                         , ( "Raising objection "s + m_name
                           + " at "s + sc_core::sc_time_stamp().to_string()
                           ).c_str()
                         , m_verbosity_level
      );
    ++s_created;
  }

  /* @method ~Objection drops the objection and destroys the objection.
   * If it is the last objection, then a process is spawned to measure
   * the drain-time.
   */
  ~Objection() ///< Remove an objection
  {
    auto elt = s_objections.find( m_name );
    sc_assert( elt != s_objections.end() );
    s_objections.erase( elt );
    if( not get_quiet() )
      SC_REPORT_INFO_VERB( mesgType
                         , ( "Dropping objection "s + m_name
                           + " at "s + sc_core::sc_time_stamp().to_string()
                           ).c_str()
                         , m_verbosity_level
      );
    monitor_objections( m_name );
  }

  /* @method total returns the total number of objection objects created during the lifetime of the program. */
  static size_t total() { return s_created; } ///< Return total times used
                                              ///
  /* @method count returns the total number of objection objects currently active. */
  static size_t count() { return s_objections.size(); }
  bool set_quiet( bool flag = true ) { std::swap(m_quiet, flag); return flag; }
  bool get_quiet() { return m_quiet; }

  /* @method monitor_objections shuts down simulation (i.e., issues `sc_stop()`) after all objections dropped and reached drain time. */
  inline static void monitor_objections( const std::string& name )
  { 
    static std::string last_name{}; //< records last objection dropping to help debug
    last_name = name;
    // Run until drain-time reached if still simulating
    if( not s_objections.empty() or not sc_core::sc_is_running() ) return;
    static sc_core::sc_event objections_dropped;
    objections_dropped.notify(); //< ignored when spawning
    // Launch a process once (static) to wait until the end of the drain-time.
    // Then shutdown the simulation if no objections are present
    static auto h = sc_core::sc_spawn( []()
    {
      for(;;) {
        sc_core::wait( s_drainTime );
        if( s_objections.empty() ) {
          SC_REPORT_INFO_VERB( mesgType
                             , ( "Shutting down due to last objection lowered in "s
                               + last_name
                               ).c_str()
                             , sc_core::SC_NONE );
          sc_core::sc_stop();
        }
        // wait for next
        wait( objections_dropped );
      }
    });
  }
  
  /* @method set_drainTime establishes the current drain time. Can be dynamically changed. */
  inline static void set_drainTime ( const sc_core::sc_time& t ) { s_drainTime = t; }

  /* @method set_drainTime establishes the maximum time before shutdown is performed unilaterally. Can be dynamically changed. */
  inline static void set_maxTimeout( const sc_core::sc_time& t ) {
    sc_core::sc_spawn( [&]() //< ensure processes are running before starting
    {
      if( s_drainTime == sc_core::SC_ZERO_TIME ) {
        s_drainTime = sc_core::sc_get_time_resolution();
      }
      s_timeoutMax = t;
      s_timeoutTime = t;
      if( s_timeout_process.valid() ) s_timeout_process.kill();
      if( t == sc_core::SC_ZERO_TIME ) return;
      s_timeoutTime += sc_core::sc_time_stamp();
      s_timeout_process = sc_core::sc_spawn( []()
      {
        wait( s_timeoutMax );
        auto note{ "Shutting down due to timeoutMax reached"s };
        SC_REPORT_ERROR( mesgType, note.c_str() );
        sc_core::sc_stop();
      });
    });
  }

  /* @method Objection::get_drainTime returns the current drainTime. */
  inline static sc_core::sc_time get_drainTime () { return s_drainTime; }

  /* @method Objection::get_maxTimeout returns the last setting for timeout. */
  inline static sc_core::sc_time get_maxTimeout() { return (s_timeoutMax == sc_core::SC_ZERO_TIME) ? sc_core::sc_max_time() : s_timeoutMax; }

  /* @method Objection::get_timeoutTime returns the actual time for the timeout. */
  inline static sc_core::sc_time get_timeoutTime() { return s_timeoutTime; }

private:

  // Per object properties to aid debug
  std::string m_name{};
  sc_core::sc_verbosity m_verbosity_level{};
  bool m_quiet;

  // Static stuff (s_ prefix)
  static constexpr const char* const       mesgType { "/Doulos/Objection" };
  inline static size_t                     s_created{ 0u };
  inline static std::set<std::string>      s_objections{};
  inline static sc_core::sc_time           s_drainTime{};
  inline static sc_core::sc_time           s_timeoutMax{};
  inline static sc_core::sc_time           s_timeoutTime{};
  inline static sc_core::sc_process_handle s_timeout_process{}; //< references timeout process
};

// TAGS: 
// ----------------------------------------------------------------------------
//
// This file is licensed under Apache-2.0, and
// Copyright 2022 David C Black <mailto:<david.black@doulos.com>>
// See accompanying LICENSE or visit <https://www.apache.org/licenses/LICENSE-2.0.txt> for more details.

// vim:nospell
