#!gdb
# GDB initialization for SystemC

define Load
  set pagination off
  echo \033[1m\033[94m> Sourcing $arg0 --------------------------------\033[0m\n
  source gdb/$arg0
  set pagination on
end
Load init

show args

#The end
