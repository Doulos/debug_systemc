define sc-basics
  dont-repeat
  init-if-undefined $did_basics = 0
  if $did_basics != 0
    printf "\e[1m\e[91mWarning: systemc already executed -- skipping\e[0m\n"
  else
    set $did_basics = 1
    set pagination off
    Report_info "sc-basics - "
    #---------------------------------------------------------------------------
    set breakpoint pending on
    tbreak Top_module::Top_module
    set variable $b1 = $bpnum
    commands $b1
      printf "\e[1m\e[95mEntered top-level constructor.\e[0m\n"
    end
    break sc_interrupt_here
    set variable $b2 = $bpnum
    break sc_stop_here
    set variable $b3 = $bpnum
    break breakpoint
    commands $b2 $b3
      silent
      frame 3 #< varies with systemc implementation
      tbreak
      commands
      end
        backtrace
      continue
    end
    catch throw
    catch catch
    #---------------------------------------------------------------------------
    set pagination on
  end
end
document sc-basics
  Break on main, top, sc_interrupt_here, sc_stop_here, throw and catch
end
printf "Defined sc-basics\n"

# vim:syntax=gdb:nospell