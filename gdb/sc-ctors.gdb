define sc-ctors
  Report_info "sc-ctors""
  init-if-undefined $did_ctors = 0
  if $did_ctors == 0
    set $did_ctors = 1
    rbreak \b[A-Z][a-z][a-zA-Z0-9_]+::[A-Z][a-z][a-zA-Z0-9_]+\b
    set $bp_ctors = $bpnum
  end
  if $argc != 0
    if $arg0 == 0
      disable breakpoint $bp_ctors
      printf "\e[1m\e[91mDisabled breakpoints on ctors\e[0m\n"
    else
      if $arg0 == 1
        enable breakpoint $bp_ctors
        printf "\e[1m\e[92mEnabled breakpoints on ctors\e[0m\n"
      else
        printf "\e[1m\e[91mError: sc-ctors bad syntax -- ignored\e[0m\n"
      end
    end
  end
end
document sc-ctors
  sc-ctors [0|1]
  Break on user constructors or turn them off.
end
printf "Defined sc-ctors\n"

# vim:syntax=gdb:nospell
