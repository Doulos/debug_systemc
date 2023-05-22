#include "test.hpp"
#include "debug.hpp"
using namespace sc_core;
using namespace std;

Test_module::Test_module( const sc_module_name& nm )
: sc_module{ nm }
{
  SC_HAS_PROCESS(Test_module);
  SC_THREAD( producer_thread );
  sensitive << clock.pos();
  SC_THREAD( observer_thread );
  SC_METHOD( consumer_method );
  sensitive << clock.pos();
}

namespace { bool bug(bool&); }

//------------------------------------------------------------------------------
void Test_module::observer_thread()
{
  static const auto observerType = std::string{msg_type} + "/observer";
  for(;;) {
    wait( producer_event );
    SC_REPORT_INFO_VERB(
          observerType.c_str(),
          ( "At "s + sc_time_stamp().to_string()
          + " observed production."s
          ).c_str(),
          SC_FULL
        );
  }
}

//------------------------------------------------------------------------------
// Produce incrementing data at specified rate
void Test_module::producer_thread()
{
  static const auto producerType = std::string{msg_type} + "/producer";
  for(;;) {
    ++actual_cycles;
    if ( produce ) {
      if ( fifo.nb_write( producedData ) ) {
        ++producer_total;
        SC_REPORT_INFO_VERB(
          producerType.c_str(),
          ( "At "s + sc_time_stamp().to_string()
          + " producer sent "s + to_string( producedData )
          ).c_str(),
          SC_DEBUG
        );
        ++ producedData;
        if( Debug::injecting() and producer_total == 2 ) {
          producedData = 0;
        }
      }
      else {
        SC_REPORT_WARNING(
            producerType.c_str(),
            "Unable to write to fifo -- full"
        );
      }
    }

    // Advance the producer
    ++producer_cycle;
    produce = producer_cycle >= producer_rate;
    if( produce ) {
      producer_event.notify();
      producer_cycle = 0;
    }
    if( bug( produce ) ) { std::cout << "\033[91m\033[1mOops!\033[0m"; }
    wait();
  }

}//end producer

//------------------------------------------------------------------------------
// Consume data at specified rate
void Test_module::consumer_method()
{
  static const auto consumerType = std::string{msg_type} + "/consumer";
  if ( consume ) {
    if ( fifo.nb_read( consumedData ) ) {
      if( consumedData == expectedData ) {
        SC_REPORT_INFO_VERB(
          consumerType.c_str(),
          ( "At "s + sc_time_stamp().to_string()
          + " correctly received "s + to_string( consumedData )
          ).c_str(),
          SC_DEBUG
        );
        ++expectedData;
        ++consumer_total;
      }
      else {
        SC_REPORT_ERROR(
          consumerType.c_str(),
          ( "Received   "s + to_string( consumedData )
          + " but expected "s + to_string( expectedData )
          ).c_str()
        );
      }
    }
    else {
      SC_REPORT_ERROR(
        consumerType.c_str(),
        "Unable to read fifo - empty"
      );
    }

    if ( --consumer_stop == 0 ) {
      sc_stop();
    }
  }

  ++consumer_cycle;
  consume = consumer_cycle >= consumer_rate;
  if( consume ) {
    consumer_cycle = 0;
  }

}//end consumer

//------------------------------------------------------------------------------
void Test_module::start_of_simulation()
{
  consumer_stop = Debug::get_count("reps");
  if ( consumer_stop == 0 ) {
    consumer_stop = 20;
  }
  SC_REPORT_INFO_VERB(
    msg_type,
    ( string( 80, '-' ) + "\n"s
    + "Configuration\n"s
    + "-------------\n"s
    + "  Production rate: "s + to_string( producer_rate ) + "\n"s
    + "    Consumer rate: "s + to_string( consumer_rate ) + "\n"s
    + "    Consumer stop: "s + to_string( consumer_stop ) + "\n"s
    + "        Verbosity: "s + ( Debug::debugging() ? "DEBUG"s : "medium"s ) + "\n"s
    + "        Injecting: "s + ( Debug::injecting() ? "true"s : "false"s ) + "\n"s
    + "          Tracing: "s + ( Debug::tracing() ? "true"s : "false"s ) + "\n"s
    + string( 80, '-' )
    ).c_str(),
    SC_NONE
  );
  if ( Debug::trace_file() != nullptr ) {
    sc_trace( Debug::trace_file(), clock,        "clock"        );
    sc_trace( Debug::trace_file(), producedData, "producedData" );
    sc_trace( Debug::trace_file(), expectedData, "expectedData" );
    sc_trace( Debug::trace_file(), consumedData, "consumedData" );
    sc_trace( Debug::trace_file(), produce,      "produce"      );
    sc_trace( Debug::trace_file(), consume,      "consume"      );
  }
}//end start_of_simulation

//------------------------------------------------------------------------------
void Test_module::end_of_simulation()
{
  SC_REPORT_INFO_VERB(
    msg_type,
    ( "\n"s + string( 80, '-' ) + "\n"s
    + "Summary:\n"s
    + "  Produced "s + to_string(producer_total) + " items\n"
    + "  Consumed "s + to_string(consumer_total) + " items\n"
    + "  Effective data rate "s + to_string( 1.0 * actual_cycles / consumer_total ) + "\n"s
    + string( 80, '-' )
    ).c_str(),
    SC_NONE
  );
}//end end_of_simulation

//------------------------------------------------------------------------------
#include <random>
namespace {
  bool bug(bool& value)
  {
    // Modifies value based on random distribution, and returns true if modified.
    static mt19937 gen{}; 
    enum                                   { FALSE, TRUE, FLIP, KEEP };
    static discrete_distribution<int> dist{{ 0,     10,   30,   60  }};
    // Randomly vary the rate
    switch ( dist( gen )) {
      case FALSE: {
        if( value ) {
          value = false;
          return true;
        }
        return false;
      }
      case TRUE: {
        if( not value ) {
          value = true;
          return true;
        }
        return false;
      }
      case FLIP: {
        value = not value;
        return true;
      }
      default: {
        // no change
        return false;
      }
    }
  }
}
