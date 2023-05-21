define sc-resume
  Report_info "sc-resume""
  init-if-undefined $did_resume = 0
  if $did_resume == 0
    set $did_resume = 1
    #-------------------------------------------------------------------------
    break Debug::resume
    set $bp_resume = $bpnum
    commands $bp_resume
      silent
      frame 1
      tbreak
      continue
    end
  end
  if $argc != 0
    if $arg0 == 0
      disable breakpoint $bp_resume
      printf "\e[1m\e[91mDisabled breakpoints on resume\e[0m\n"
    else
      if $arg0 == 1
        enable breakpoint $bp_resume
        printf "\e[1m\e[92mEnabled breakpoints on resume\e[0m\n"
      else
        printf "\e[1m\e[91mError: sc-resume bad syntax -- ignored\e[0m\n"
      end
    end # $arg0 == 0
  end # $argc != 0
end
document sc-resume
  Breakpoint 0 1 Debug::resume
end
printf "Defined sc-resume\n"

# vim:syntax=gdb:nospell
