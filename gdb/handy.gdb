# Handy commands for interactive gdb

init-if-undefined $loaded_handy = 0
if $loaded_handy != 0
  printf "\e[1m\e[91mWarning: handy already loaded -- skipping \e[0m\n"
  # cause an error to abort
  skip_loading
end
printf "loading handy\n"
set $loaded_handy = 1

#------------------------------------------------------------------------------
define exit
  quit
end
document exit
  Alias for quit
end
printf "Defined exit\n"

#------------------------------------------------------------------------------
define clear
  shell clear
end
document clear
  clear screen
end
printf "Defined clear\n"

# vim:nospell:syntax=gdb
