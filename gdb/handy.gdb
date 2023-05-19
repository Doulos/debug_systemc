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
define which
  shell which $arg0
end
document which
  Find an executable
end
printf "Defined which\n"

#------------------------------------------------------------------------------
define banner
  shell header $0
end
document banner
  Display a text banner
end
printf "Defined banner\n"

#------------------------------------------------------------------------------
define ls
  if $argc == 0
    shell ls
  end
  if $argc == 1
    shell ls $arg0
  end
  if $argc == 2
    shell ls $arg0 $arg1
  end
  if $argc == 3
    shell ls $arg0 $arg1 $arg2
  end
end
document ls
  execute shell ls
end
printf "Defined ls\n"

#------------------------------------------------------------------------------
define cat
  if $argc == 0
    printf "Syntax: cat FILENAME"
  end
  if $argc == 1
    shell cat $arg0
  end
end
document cat
  execute shell cat
end
printf "Defined cat\n"

#------------------------------------------------------------------------------
define nano
  if $argc == 1
    shell nano $arg0
  end
  if $argc == 2
    shell nano $arg0 $arg1
  end
  if $argc == 3
    shell nano $arg0 $arg1 $arg2
  end
end
document nano
  execute nano
end
printf "Defined nano\n"

#------------------------------------------------------------------------------
define vim
  if $argc == 1
    shell vim $arg0
  end
  if $argc == 2
    shell vim $arg0 $arg1
  end
  if $argc == 3
    shell vim $arg0 $arg1 $arg2
  end
end
document vim
  execute vim
end
printf "Defined vim\n"

#------------------------------------------------------------------------------
define less
  shell less $arg0
end
document less
  execute less
end
printf "Defined less\n"

#------------------------------------------------------------------------------
define clear
  shell clear
end
document clear
  clear screen
end
printf "Defined clear\n"

#------------------------------------------------------------------------------
define git
  if $argc == 1
    shell git $arg0
  end
  if $argc == 2
    shell git $arg0 $arg1
  end
  if $argc == 3
    shell git $arg0 $arg1 $arg2
  end
  if $argc == 4
    shell git $arg0 $arg1 $arg2 $arg3
  end
  if $argc == 5
    shell git $arg0 $arg1 $arg2 $arg3 $arg4
  end
  if $argc == 6
    shell git $arg0 $arg1 $arg2 $arg3 $arg4 $arg5
  end
  if $argc == 7
    shell git $arg0 $arg1 $arg2 $arg3 $arg4 $arg5 $arg6
  end
  if $argc == 8
    shell git $arg0 $arg1 $arg2 $arg3 $arg4 $arg5 $arg6 $arg7
  end
  if $argc == 9
    shell git $arg0 $arg1 $arg2 $arg3 $arg4 $arg5 $arg6 $arg7 $arg8
  end
  if $argc == 10
    shell git $arg0 $arg1 $arg2 $arg3 $arg4 $arg5 $arg6 $arg7 $arg8 $arg9
  end
end
document git
  Siver-searcher-git
end
printf "Defined git\n"

#------------------------------------------------------------------------------
define ag
  if $argc == 1
    shell ag $arg0
  end
  if $argc == 2
    shell ag $arg0 $arg1
  end
  if $argc == 3
    shell ag $arg0 $arg1 $arg2
  end
  if $argc == 4
    shell ag $arg0 $arg1 $arg2 $arg3
  end
  if $argc == 5
    shell ag $arg0 $arg1 $arg2 $arg3 $arg4
  end
  if $argc == 6
    shell ag $arg0 $arg1 $arg2 $arg3 $arg4 $arg5
  end
  if $argc == 7
    shell ag $arg0 $arg1 $arg2 $arg3 $arg4 $arg5 $arg6
  end
  if $argc == 8
    shell ag $arg0 $arg1 $arg2 $arg3 $arg4 $arg5 $arg6 $arg7
  end
  if $argc == 9
    shell ag $arg0 $arg1 $arg2 $arg3 $arg4 $arg5 $arg6 $arg7 $arg8
  end
  if $argc == 10
    shell ag $arg0 $arg1 $arg2 $arg3 $arg4 $arg5 $arg6 $arg7 $arg8 $arg9
  end
end
document ag
  Siver-searcher-ag
end
printf "Defined ag\n"

# vim:nospell:syntax=gdb
