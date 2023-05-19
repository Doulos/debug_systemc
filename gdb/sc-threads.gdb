define sc-threads
  init-if-undefined $did_threads = 0
  if $did_threads == 0
    set $did_threads = 1
    rbreak \b[A-Z][a-z].*_thread\b
    set $bp_threads = $bpnum
  end
  if $argc != 0
    if $arg0 == 0
      disable breakpoint $bp_threads
      printf "\e[1m\e[91mDisabled breakpoints on threads\e[0m\n"
    else
      if $arg0 == 1
        enable breakpoint $bp_threads
        printf "\e[1m\e[92mEnabled breakpoints on threads\e[0m\n"
      else
        printf "\e[1m\e[91mError: sc-threads bad syntax -- ignored\e[0m\n"
      end
    end
  end
end
document sc-threads
  sc-threads [0|1]
  Break on user SC_THREADS or turn them off.
end
printf "Defined sc-threads\n"

# vim:syntax=gdb:nospell
