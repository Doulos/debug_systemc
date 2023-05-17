#!gdb
# GDB initialization for SystemC

define Load
  set pagination off
  printf "\e[1m\e[94m> Sourcing %s --------------------------------\e[0m\n", $arg0
  source gdb/$arg0
  set pagination on
end

break sc_main
run
Load init
show args

#The end
