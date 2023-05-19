define Report_info
  printf "\e[1m\e[96mInfo: %s\e[0m\n", $arg0
end
printf "Defined Report_info\n"

define Report_warning
  printf "\e[1m\e[93mWarning: %s\e[0m\n", $arg0
end
printf "Defined Report_warning\n"

define Report_error
  printf "\e[1m\e[91mError: %s\e[0m\n", $arg0
end
printf "Defined Report_error\n"

# vim:syntax=gdb:nospell
