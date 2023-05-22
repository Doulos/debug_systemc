define sc-common
  dont-repeat
  init-if-undefined $did_common = 0
  if $did_common != 0
    printf "\e[1m\e[91mWarning: sc-common already executed -- skipping \e[0m\e[0m\n"
  else
    set $did_common = 1
    #---------------------------------------------------------------------------
    sc-basics
    sc-resume
    sc-threads
    sc-overrides
    sc-ctors
    sc-skip
    set pagination off
    info breakpoints
    info skip
    printf "--------------------------------------------------------------------------------\n"
    #---------------------------------------------------------------------------
    set pagination on
  end
end
document sc-common
  Execute *common* scripts for debugging systemc
end
printf "Defined sc-common\n"

# vim:syntax=gdb:nospell
