## Purpose

Provide a SystemC class to determine when a simulation should stop in the same manner as UVM.

## Overview

Perhaps the quickest way to understand the API is to read the header, but is reproduced here for those that prefer separate documentation.

A class to stop SystemC simulation when the last objection is removed within a defined drain time.

The idea behind this class mirrors the concept of objections in the UVM methodology. This particular implementation uses RAII in concert with a Singleton model. So you instantiate an objection object on the stack inside a region where activity occurs. The constructor immediately raises an object. When you leave that region, and the objection is destroyed, it will automatically drop via the destructor. As with UVM, There is both a time-out and a drain-time that can be set via static methods. When all objections have dropped and the drain time has expired then `sc_stop` is called.

## API

| METHOD                                                   | DESCRIPTION                                                                                                                                    |
| -------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| `Objection( string name, sc_verbosity level = SC_HIGH )` | Construct an objection with the provided name, which is used to help debug. Verbosity is used to debug issues.                                  |
| `~Objection()`                                           | Destruction drops the objection and destroys the objection.  If it is the last objection, then a process is spawned to measure the drain-time. |
| `size_t Objection::total()`                              | Returns the total number of objection objects created during the lifetime of the program.                                                      |
| `size_t Objection::count()`                              | Returns the total number of objection objects currently active.                                                                                |
| `void monitor_objections( const string& name )`          | Shuts down simulation (i.e., issues `sc_stop()`) after all objections have dropped and reached drain time.                                     |
| `void Objection::set_drainTime ( const sc_time& t )`     | Establishes the current drain time. Can be dynamically changed. The default drain time is zero.                                                |
| `void Objection::set_maxTimeout( const sc_time& t )`     | Establishes the maximum time before shutdown is performed unilaterally. Can be dynamically changed. Zero, the default, means not timeout.      |
| `sc_time Objection::get_drainTime ()`                    | Returns the current drain time.                                                                                                                |
| `sc_time Objection::get_maxTimeout()`                    | Returns the last setting for timeout.                                                                                                          |
| `sc_time Objection::get_timeoutTime()`                   | Returns the actual time for the timeout.                                                                                                       |

## EXAMPLE

```c++
#include "objection.hpp"

SC_MODULE( Example ) {

  sc_clock ck { "ck" };

  SC_CTOR( Example ) {
    SC_THREAD( my_thread );
    sc_report_handler::set_verbosity_level( SC_HIGH ); // for debug
  }

  void start_of_simulation()
  {
    Objection::set_maxTimeout( 1, SC_SEC ); // or SC_ZERO_TIME for infinite (default)
    Objection::set_drainTime( 100_ns ); // or SC_ZERO_TIME for end of delta cycle (default)
  }

  void my_thread()
  {
    Objection mine{ "running my code" };
    // do some work
    for( auto i=10; i--; ) {
      wait( ck.posedge_event() );
    }
    // I'm done
  }

};
```

