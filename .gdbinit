# GDB initialization for SystemC
set pagination off
echo --------------------------------------------------------------------------------\n
echo \033[1m\033[94m> Sourcing
shell echo -n " `realpath .gdbinit` "
echo \033[1m\033[94m-----------------\033[0m\n
echo Working directory 
pwd
set args --config tests/inject.cfg
show args
echo --------------------------------------------------------------------------------\n

define Load
  set pagination off
  echo \033[1m\033[94m> Sourcing $arg0 --------------------------------\033[0m\n
  source gdb/$arg0
  set pagination on
end
document Load
  Over-the-top convenience to load (i.e., source) GDB scripts.
end

set pagination on

#The end
