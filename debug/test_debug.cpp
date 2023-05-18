#include "debug.hpp"
#include <systemc>
#include <string>


struct Top_module : sc_core::sc_module
{
  using report_handler = sc_core::sc_report_handler;
  using string = std::string;
  using time = sc_core::sc_time;

  static constexpr const char *mesgType = "/Doulos/debugging_systemc/top";

  // Constructor
  explicit Top_module( const sc_core::sc_module_name& instance )
    : sc_module( instance )
  {
    // No interconnect for this design
    SC_HAS_PROCESS(Top_module);
    SC_THREAD( test_thread );
  }

  void before_end_of_elaboration() override
  {

    report_handler::set_actions( sc_core::SC_WARNING, sc_core::SC_DISPLAY | sc_core::SC_INTERRUPT );
    report_handler::set_actions( sc_core::SC_ERROR, sc_core::SC_DISPLAY   | sc_core::SC_INTERRUPT );

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
    studentGrade = Debug::get_count("nGrade");
    studentStart = Debug::get_time("tStart");
    studentName  = Debug::get_text("sName");
    studentMember = Debug::get_flag("fMember");
    SC_REPORT_INFO_VERB( mesgType, "Starting report...", sc_core::SC_DEBUG );
    if ( studentGrade < 70 ) SC_REPORT_ERROR( mesgType, "You failed the exam!" );
    else if ( studentGrade < 80 ) SC_REPORT_ERROR( mesgType, "You barely passed" );
    else if ( studentGrade < 90 ) SC_REPORT_WARNING( mesgType, "You did fairly well, but you can do better" );
    else if ( studentGrade < 100 ) SC_REPORT_INFO( mesgType, "You are an A student!" );
    else if ( studentGrade > 100 ) SC_REPORT_INFO_VERB( mesgType, "This looks fishy!", sc_core::SC_NONE );
    sc_core::sc_stop();
  }

};

#include <memory>
using namespace sc_core;
using namespace std::literals;

// Entry point called externally
int sc_main( [[maybe_unused]] int argc, [[maybe_unused]] char* argv[] )
{
  static constexpr const char* mesgType = "/Doulos/navigation/main";
  // Place most of code on the heap -- sc_start will use this indirectly
  [[maybe_unused]] auto top = std::make_unique<Top_module>( "top" );
  sc_start();

  if ( not sc_end_of_simulation_invoked() ) {
    sc_stop();  // triggers end_of_simulation() callback
  }

  return Debug::exit_status( mesgType );
}
