# GDB initialization for SystemC

alias ld=source
break sc_main
run
source gdb/init.gdb
show args

# vim:syntax=gdb:nospell
