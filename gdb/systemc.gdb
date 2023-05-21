# Useful for SystemC debug

init-if-undefined $loaded_systemc = 0
if $loaded_systemc != 0
  printf "\e[1m\e[91mWarning: systemc already loaded -- skipping\e[0m\n"
  # cause an error to abort
  skip_loading
end
printf "loading\n"
set $loaded_systemc = 1

source gdb/report.gdb
source gdb/sc-delete-last.gdb
#------------------------------------------------------------------------------
source gdb/sc-info.gdb
source gdb/sc-basics.gdb
source gdb/sc-ctors.gdb
source gdb/sc-overrides.gdb
source gdb/sc-threads.gdb
source gdb/sc-methods.gdb
source gdb/sc-skip.gdb
source gdb/sc-next-trigger.gdb
source gdb/sc-wait.gdb
source gdb/sc-resume.gdb
#------------------------------------------------------------------------------
source gdb/sc-all.gdb
source gdb/sc-common.gdb
source gdb/sc-demo.gdb

# vim:syntax=gdb:nospell
