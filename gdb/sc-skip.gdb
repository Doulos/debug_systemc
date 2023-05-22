define sc-skip
  Report_info "sc-skip"
  init-if-undefined $did_skip = 0
  if $did_skip != 0
    printf "\e[1m\e[91mWarning: systemc already executed -- skipping \e[0m\n"
  else
    set $did_skip = 1
    dont-repeat
    set pagination off
    #---------------------------------------------------------------------------
    skip -rfunction sc_core::sc_.*
    skip -rfunction tlm::.*
    skip -rfunction std::.*
    #---------------------------------------------------------------------------
    set pagination on
  end
end
document sc-skip
  Skip all the library elements for std, sysemc, and tlm.
  This list may grow.
end
printf "Defined sc-skip\n"

# vim:syntax=gdb:nospell
