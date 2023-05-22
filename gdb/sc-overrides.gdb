define sc-overrides
  Report_info "sc-overrides"
  rbreak \b[A-Z][a-z].*::before_end_of_elaboration
  rbreak \b[A-Z][a-z].*::end_of_elaboration
  rbreak \b[A-Z][a-z].*::start_of_simulation
  rbreak \b[A-Z][a-z].*::end_of_simulation
end
document sc-overrides
  Break on all call-backs
end
printf "Defined sc-overrides\n"

# vim:syntax=gdb:nospell
