define sc-ctors
  Report_info "sc-ctors"
  init-if-undefined $did_ctors = 0
  if $did_ctors == 0
    set $did_ctors = 1
    set $bp_ctor1 = $bpnum + 1
    rbreak \b[A-Z][a-z][a-z0-9A-Z_].*_module::[A-Z][a-z][a-z0-9A-Z_].*
    set $bp_ctor2 = $bpnum
  end
  if $argc != 0
    if $arg0 == 0
      set $bp_i = $bp_ctor1
      while $bp_i <= $bp_ctor2
        disable breakpoint $bp_i
        set $bp_i = $bp_i + 1
      end
      printf "\e[1m\e[91mDisabled breakpoints on ctors\e[0m\n"
    else
      if $arg0 == 1
        set $bp_i = $bp_ctor1
        while $bp_i <= $bp_ctor2
          enable breakpoint $bp_i
          set $bp_i = $bp_i + 1
        end
        printf "\e[1m\e[92mEnabled breakpoints on ctors\e[0m\n"
      else
        printf "\e[1m\e[91mError: sc-ctors bad syntax -- ignored\e[0m\n"
      end
    end
  end
end
document sc-ctors
  sc-ctors [0|1]
  Break, enable, or disable all user module constructors.
  Assumes naming convention Name_module::*
end
printf "Defined sc-ctors\n"

# vim:syntax=gdb:nospell
