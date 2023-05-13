#pragma once

#include <string>
#include <chrono>
#include <cmath>
#include <ratio>
/** @brief Provide to_string( time )
 *
 * Two syntax's:
 * 1. to_string( duration, digits=3 ), where digits specifies precision
 * 2. to_string( duration, units, digits=3), where units: 1h, 1min, 1s, 1ms, etc.
 *
 * Also includes a special `long double Round<T>( T, digits )` that
 * may be of interest to some.
 *
 * TODO: to_string( duration, hms, units ), includes hours & minutes as needed
 */
template<typename T=long double>
constexpr long double Round( T value, int digits=3 )
{
  T resolution = 1.0;
  while( digits-- > 0 ) resolution *= 10.0;
  return std::round(value*resolution)/resolution;
}

class Chronout {
public:
  using ns_t = std::chrono::nanoseconds;
  using double_ns_t = std::chrono::duration<long double,std::nano>;

  // Use the largest time unit possible (e.g. 100s => "1.667min")
  static std::string to_string( double_ns_t t, int digits = 3 )
  {
    using namespace std::chrono_literals;
    std::string suffix, result;
    // Round to the nearest appropriate time unit
    if      ( t >= 1.0h   ) { result = std::to_string( Round( t/1.0h  , digits )); suffix = "h";  }
    else if ( t >= 1.0min ) { result = std::to_string( Round( t/1.0min, digits )); suffix = "min";}
    else if ( t >= 1.0s   ) { result = std::to_string( Round( t/1.0s  , digits )); suffix = "s";  }
    else if ( t >= 1.0ms  ) { result = std::to_string( Round( t/1.0ms , digits )); suffix = "ms"; }
    else if ( t >= 1.0us  ) { result = std::to_string( Round( t/1.0us , digits )); suffix = "us"; }
    else                    { result = std::to_string( Round( t/1.0ns , digits )); suffix = "ns"; }
    // Remove trailing zeroes after the decimal point except if the entire fraction is zero
    auto pos = result.find_last_of(".123456789");
    if ( pos != std::string::npos and pos < result.size() ) result.erase( pos+1 );
    if ( result.back() == '.' ) result += '0';
    return result + suffix;
  }

  // Specify specific base time units desired (e.g. 1min in ms => "60000.0ms")
  static std::string to_string( double_ns_t t, ns_t units, int digits = 3 )
  {
    using namespace std::chrono_literals;
    std::string suffix, result;
    // Round to the specified time unit
    if      ( units == 24h  ) { result = std::to_string( Round( t/24.0h , digits )); suffix = "d";  }
    else if ( units == 1h   ) { result = std::to_string( Round( t/1.0h  , digits )); suffix = "h";  }
    else if ( units == 1min ) { result = std::to_string( Round( t/1.0min, digits )); suffix = "min";}
    else if ( units == 1s   ) { result = std::to_string( Round( t/1.0s  , digits )); suffix = "s";  }
    else if ( units == 1ms  ) { result = std::to_string( Round( t/1.0ms , digits )); suffix = "ms"; }
    else if ( units == 1us  ) { result = std::to_string( Round( t/1.0us , digits )); suffix = "us"; }
    else                      { result = std::to_string( Round( t/1.0ns , digits )); suffix = "ns"; }
    // Remove trailing zeroes after the decimal point except if the entire fraction is zero
    auto pos = result.find_last_of(".123456789");
    if ( pos != std::string::npos and pos < result.size() ) result.erase( pos+1 );
    if ( result.back() == '.' ) result += '0';
    return result + suffix;
  }

};

// TAGS: Doulos, SystemC, sc_main, SOURCE
// ----------------------------------------------------------------------------
//
// This file is licensed under Apache-2.0, and
// Copyright 2022 David C Black <mailto:<david.black@doulos.com>>
// See accompanying LICENSE or visit <https://www.apache.org/licenses/LICENSE-2.0.txt> for more details.
