define sc-methods
  Report_info "sc-methods"
  rbreak \b[A-Z][a-z].*_method\b
  set $bp_method = $bpnum
  commands
    # Interested in first call only
    disable $_hit_bpnum.$_hit_locno
  end
end
document sc-methods
  Break on user SC_METHODs
end
printf "Defined sc-methods\n"

# vim:syntax=gdb:nospell
