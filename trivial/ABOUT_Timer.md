## About the Doulos::Timer

The Doulos::Timer class is used to obtain rough estimates of performance using the C++11 chrono library's high resolution clock.

The class was designed to be used with or without SystemC.

NOTE: This is NOT a SystemC module (e.g., `Timer_module`).

### Usage

The easiest way to use the timer is to simply instantiate it with a name inside a code block or function. Timing begins during instantion. The timer will display elapsed time upon destruction.

### Examples

```cpp
#include "timer.hpp"

void work( uint32_t effort ) {
  volatile auto d = 0.0;
  auto repetitions = uint64_t{ UINT32_MAX/32*effort };
  for( auto i=0ULL; i < ; ++i ) {
    ++d;
  }
}

int main() {
  Doulos::Timer t1 { "t1-automatic" };
  work( 7 );
  {
    Doulos::Timer t2 { "t2-manual", false };
    work( 2 );
    t2.report();
    t2.reset();
    work( 3 );
    t2.report();
  }
}
```

### Copyrights

This document and accompanying files are all Copyright 2023 by Doulos and licensed under Apache 2.0. Please see `LICENSE` file for details.
