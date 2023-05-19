define sc-next-trigger
  init-if-undefined $did_nt = 0
  if $did_nt == 0
    set $did_nt = 1
    dont-repeat
    set pagination off
    #-------------------------------------------------------------------------
    set $nt_count = 0
    set $nt_limit = 0
    break sc_module::next_trigger
    set $bp_nt = $bpnum
    commands $bp_nt
      silent
      set $nt_count = $nt_count + 1
      printf "\e[1m\e[91m--- next_trigger %d ---\e[0m\n", $nt_count
      if $nt_limit != 0
        set $nt_limit = $nt_limit - 1
      end
      if $nt_limit != 0
        continue
      end
    end
    #-------------------------------------------------------------------------
    set pagination on
  end
  if $argc != 0
    set $nt_limit = $arg0
    if $arg0 == 0
      disable breakpoint $bp_nt
      printf "\e[1m\e[91mDisabled breakpoint on sc_module::next_trigger()\e[0m\n"
    else
      if $arg0 == 1
        enable breakpoint $bp_nt
        printf "\e[1m\e[92mEnabled breakpoint on sc_module::next_trigger()\e[0m\n"
      end
    end
  end
end
document sc-next-trigger
  sc-next-trigger [#] // Output a marker when encountering next_trigger().
  Optionally, turn breakpoint off(0), on(1), or wait N next_triggers before stopping.
end
printf "Defined sc-next-trigger\n"

# vim:syntax=gdb:nospell
