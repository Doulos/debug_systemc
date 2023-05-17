# GDB initialization for SystemC

define myload
  set pagination off
  if $argc == 1
    printf "\e[1m\e[94m Sourcing %s --------------------------------\e[0m\n", $arg0
    source gdb/$arg0
  else
    printf "\e[1m\e[91mSyntax: Load FILE\e[0m\n"
  end
  set pagination on
end

break sc_main
run
myload init
show args

# vim:syntax=gdb:nospell
