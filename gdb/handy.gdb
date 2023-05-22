# Handy commands for interactive gdb

printf "Loading handy commands\n"
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
define cls
  shell clear
end
document cls
  clear screen
end
printf "Defined cls\n"

# vim:nospell:syntax=gdb
