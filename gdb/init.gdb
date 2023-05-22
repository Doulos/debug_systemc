set pagination off
echo --------------------------------------------------------------------------------\n
printf "\e[1m\e[94m> Sourcing %s -----------------\e[0m\n", "gdb/init"
printf "Working directory "
shell pwd
echo --------------------------------------------------------------------------------\n

set history filename ~/.gdb_history.txt
set history save on
set history size unlimited
enable pretty printing

set pagination on

# vim:syntax=gdb:nospell
