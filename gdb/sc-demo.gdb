define sc-demo
  dont-repeat
  init-if-undefined $did_demo = 0
  if $did_demo != 0
    printf "\e[1m\e[91mWarning: sc-demo already executed -- skipping \e[0m\e[0m\n"
  else
    set $did_demo = 1
    #---------------------------------------------------------------------------
    sc-basics
    sc-wait
    sc-resume
    sc-threads
    sc-ctors
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
document sc-demo
  Execute scripts demonstrating debugging of systemc
end
printf "Defined sc-demo\n"

# vim:syntax=gdb:nospell
