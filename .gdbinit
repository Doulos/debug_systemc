# GDB initialization for SystemC

set breakpoint pending on
break sc_main
set history filename ~/.gdb_history.txt
set history save on
set history size unlimited
enable pretty printing
show args

# vim:syntax=gdb:nospell
