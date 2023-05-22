define sc-wait
  Report_info "sc-wait"
  init-if-undefined $did_wait = 0
  if $did_wait == 0
    set $did_wait = 1
    set pagination off
    #-------------------------------------------------------------------------
    break sc_module::wait
    set $bp_wait = $bpnum
    commands $bp_wait
      silent
      printf "\e[1m\e[91m--- wait ---\e[0m\n"
      frame 1
      tbreak
      continue
    end
    #-------------------------------------------------------------------------
    set pagination on
  end
  # Optionally allow enable/disable (1/0)
  if $argc != 0
    if $arg0 == 0
      disable breakpoint $bp_wait.1
      disable breakpoint $bp_wait.2
      printf "\e[1m\e[91mDisabled breakpoint on sc_module::wait()\e[0m\n"
    else
      if $arg0 == 1
        enable breakpoint $bp_wait.1
        enable breakpoint $bp_wait.2
        printf "\e[1m\e[92mEnabled breakpoint on sc_module::wait()\e[0m\n"
      end
    end
  end
end
document sc-wait
  sc-wait [#] // Output a marker when encountering wait().
  Optionally, breakpoint disable(0), or enable(1)
end
printf "Defined sc-wait\n"

# vim:syntax=gdb:nospell
