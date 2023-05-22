define sc-methods
  Report_info "sc-methods"
  init-if-undefined $did_methods = 0
  if $did_methods == 0
    set $did_methods = 1
    set $bp_method1 = $bpnum + 1
    rbreak \b[A-Z][a-z].*_method\b
    set $bp_method2 = $bpnum
    set $bp_i = $bp_method1
    while $bp_i <= $bp_method2
      enable breakpoints delete $bp_i
      set $bp_i = $bp_i + 1
    end
  end
  if $argc != 0
    if $arg0 == 0
      set $bp_i = $bp_method1
      while $bp_i <= $bp_method2
        disable breakpoint $bp_i
        set $bp_i = $bp_i + 1
      end
      printf "\e[1m\e[91mDisabled breakpoints on methods\e[0m\n"
    end
    if $arg0 == 1
      if $arg0 == 1
        set $bp_i = $bp_method1
        while $bp_i <= $bp_method2
          enable breakpoint $bp_i
          set $bp_i = $bp_i + 1
        end
        printf "\e[1m\e[92mEnabled breakpoints on methods\e[0m\n"
      end
    end
    if $arg0 == 6
      if $arg0 == 1
        set $bp_i = $bp_method1
        while $bp_i <= $bp_method2
          delete breakpoint $bp_i
          set $bp_i = $bp_i + 1
        end
        set $did_methods = 0
        printf "\e[1m\e[92mDeleted breakpoints on methods\e[0m\n"
      end
    end
  end
end
document sc-methods
  Break on user SC_METHODs as one-shot
end
printf "Defined sc-methods\n"

# vim:syntax=gdb:nospell
