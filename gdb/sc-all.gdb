define sc-all
  dont-repeat
  init-if-undefined $did_systemc = 0
  if $did_systemc != 0
    printf "\e[1m\e[91mWarning: sc-all already executed -- skipping \e[0m\e[0m\n"
  else
    set $did_systemc = 1
    #---------------------------------------------------------------------------
    sc-basics
    sc-wait
    sc-resume
    sc-next-trigger
    sc-methods
    sc-threads
    sc-overrides
    sc-skip
    set pagination off
    info breakpoints
    info skip
    printf "--------------------------------------------------------------------------------\n"
    #---------------------------------------------------------------------------
    set pagination on
  end
end
document sc-all
  Initialization for debugging systemc
end
printf "Defined sc-all\n"

# vim:syntax=gdb:nospell
