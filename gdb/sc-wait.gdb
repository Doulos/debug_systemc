define sc-wait
  init-if-undefined $did_wait = 0
  if $did_wait == 0
    set $did_wait = 1
    set pagination off
    #-------------------------------------------------------------------------
    set $wait_count = 0
    set $wait_limit = 0
    break sc_module::wait
    set $bp_wait = $bpnum
    commands $bp_wait
      silent
      set $wait_count = $wait_count + 1
      printf "\e[1m\e[91m--- wait %d ---\e[0m\n", $wait_count
      if $wait_limit != 0
        set $wait_limit = $wait_limit - 1
      end
      if $wait_limit != 0
        continue
      end
    end
    #-------------------------------------------------------------------------
    set pagination on
  end
  if $argc != 0
    set $wait_limit = $arg0
    if $arg0 == 0
      disable breakpoint $bp_wait
      printf "\e[1m\e[91mDisabled breakpoint on sc_module::wait()\e[0m\n"
    else
      if $arg0 == 1
        enable breakpoint $bp_wait
        printf "\e[1m\e[92mEnabled breakpoint on sc_module::wait()\e[0m\n"
      end
    end
  end
end
document sc-wait
  sc-wait [#] // Output a marker when encountering wait().
  Optionally, turn breakpoint off(0), on(1), or wait N waits before stopping.
end
printf "Defined sc-wait\n"

# vim:syntax=gdb:nospell
