define sc-overrides
  Report_info "sc-overrides"
  init-if-undefined $did_overrides = 0
  if $did_overrides == 0
    set $did_overrides = 1
    set $bp_over1 = $bpnum + 1
    rbreak \b[A-Z][a-z].*::before_end_of_elaboration
    rbreak \b[A-Z][a-z].*::end_of_elaboration
    rbreak \b[A-Z][a-z].*::start_of_simulation
    rbreak \b[A-Z][a-z].*::end_of_simulation
    set $bp_over2 = $bpnum
  end
  if $argc != 0
    if $arg0 == 0
      set $bp_i = $bp_over1
      while $bp_i <= $bp_over2
        disable breakpoint $bp_i
        set $bp_i = $bp_i + 1
      end
      printf "\e[1m\e[91mDisabled breakpoints on overrides\e[0m\n"
    else
      if $arg0 == 1
        set $bp_i = $bp_over1
        while $bp_i <= $bp_over2
          enable breakpoint $bp_i
          set $bp_i = $bp_i + 1
        end
        printf "\e[1m\e[92mEnabled breakpoints on overrides\e[0m\n"
      else
        printf "\e[1m\e[91mError: sc-overrides bad syntax -- ignored\e[0m\n"
      end
    end
  end
end
document sc-overrides
  Break on all call-backs
end
printf "Defined sc-overrides\n"

# vim:syntax=gdb:nospell
