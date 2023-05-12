# GDB initialization for SystemC
echo --------------------------------------------------------------------------------\n
echo \033[1m\033[94m------ Sourcing
shell echo -n " `realpath .gdbinit` "
echo \033[1m\033[94m------\033[0m\n
echo Working directory 
pwd
set args --config tests/inject.cfg
show args
echo --------------------------------------------------------------------------------\n

define Source
  source gdb/$0
end
document
  Over-the-top convenience to run gdb scripts.
end

#The end
