#!gdb
# GDB initialization for SystemC

define Load
  set pagination off
  printf "\e[1m\033[94m> Sourcing %s --------------------------------\033[0m\n", $arg0
  source gdb/$arg0
  set pagination on
end
Load init

show args

#The end
