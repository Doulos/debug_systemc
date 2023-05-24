## User commands to invoke others 

+ sc-all
+ sc-common
+ sc-demo

## User commands to setup, enable, or disable breakpoints

User-command    | Description
--------------- | --------------------------------------------------------------
sc-basics       | break on sc_interrupt_here, sc_stop_here and catch exceptions
sc-ctors        | place breakpoints on constructors
sc-methods      | enable/disable breakpoints on SC_METHODS
sc-next-trigger | enable/disable breakpoint on sc_module::next_trigger
sc-overrides    | enable/disable breakpoints on overrides (e.g., start_of_simulation)
sc-resume       | breakpoint on Disable::resume() used after blocking methods
sc-skip         | skip single-stepping std and systemc libraries
sc-threads      | enable/disable breakpoints on SC_THREADS
sc-wait         | enable/disable breakpoints on sc_module::wait

## User commands to display or get information

User-command    | Description
--------------- | --------------------------------------------------------------
sc-brief        | provides this list
sc-debug        | enable/disable debugging and SC_DEBUG verbosity
sc-help         | more detailed help
sc-info         | various informational scripts
sc-name OBJ     | display the name of an object
sc-opts         | display current option selections
sc-process      | display the current systemc process
sc-show         | display a std::string

--------------------------------------------------------------------------------
