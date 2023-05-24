#include "debug.hpp"
#include <systemc>
#include <string>


struct Top_module : sc_core::sc_module
{
  using string = std::string;
  using time = sc_core::sc_time;

  static constexpr const char *msg_type = "/Doulos/debugging_systemc/top";
  Doulos::Info info{msg_type};

  // Constructor
  explicit Top_module( const sc_core::sc_module_name& instance )
    : sc_module( instance )
  {
    // No interconnect for this design
    SC_HAS_PROCESS(Top_module);
    SC_THREAD( test_thread );
    ::sc_core::sc_report_handler::set_actions( ::sc_core::SC_WARNING
                                             , ::sc_core::SC_DISPLAY
                                             | ::sc_core::SC_INTERRUPT
                                             );
    ::sc_core::sc_report_handler::set_actions( ::sc_core::SC_ERROR
                                             , ::sc_core::SC_DISPLAY
                                             | ::sc_core::SC_INTERRUPT
                                             );
    ::sc_core::sc_report_handler::set_actions( ::sc_core::SC_FATAL
                                             , ::sc_core::SC_DISPLAY
                                             | ::sc_core::SC_LOG
                                             | ::sc_core::SC_STOP
                                             );

    Debug::parse_command_line();
  }

  void start_of_simulation() override
  {
    Debug::stop_if_requested();
  }

  string studentName{};
  size_t studentGrade{};
  bool   studentMember{};
  time   studentStart{};
  void test_thread() {
    info.entering(__func__,this);
    studentGrade = Debug::get_count("nGrade");
    studentStart = Debug::get_time("tStart");
    studentName  = Debug::get_text("sName");
    studentMember = Debug::get_flag("fMember");
    REPORT_DEBUG( "Starting report..." );
    if ( studentGrade < 70 ) REPORT_ERROR( "You failed the exam!" );
    else if ( studentGrade < 80 ) REPORT_ERROR( "You barely passed" );
    else if ( studentGrade < 90 ) REPORT_WARNING( "You did fairly well, but you can do better" );
    else if ( studentGrade < 100 ) REPORT_INFO( "You are an A student!" );
    else if ( studentGrade > 100 ) REPORT_ALWAYS( "This looks fishy!" );
    sc_core::sc_stop();
    info.leaving(__func__,this);
  }

};

#include <memory>
using namespace sc_core;
using namespace std::literals;

// Entry point called externally
int sc_main( [[maybe_unused]] int argc, [[maybe_unused]] char* argv[] )
{
  static constexpr const char* msg_type = "/Doulos/navigation/main";
  // Place most of code on the heap -- sc_start will use this indirectly
  [[maybe_unused]] auto top = std::make_unique<Top_module>( "top" );
  sc_start();

  if ( not sc_end_of_simulation_invoked() ) {
    sc_stop();  // triggers end_of_simulation() callback
  }

  return Debug::exit_status( msg_type );
}
