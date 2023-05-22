define sc-threads
  Report_info "sc-threads"
  init-if-undefined $did_threads = 0
  if $did_threads == 0
    set $did_threads = 1
    set $bp_thread1 = $bpnum + 1
    rbreak \b[A-Z][a-z].*_thread\b
    set $bp_thread2 = $bpnum
  end
  if $argc != 0
    if $arg0 == 0
      set $bp_i = $bp_thread1
      while $bp_i <= $bp_thread2
        disable breakpoint $bp_i
        set $bp_i = $bp_i + 1
      end
      printf "\e[1m\e[91mDisabled breakpoints on threads\e[0m\n"
    else
      if $arg0 == 1
        set $bp_i = $bp_thread1
        while $bp_i <= $bp_thread2
          enable breakpoint $bp_i
          set $bp_i = $bp_i + 1
        end
        printf "\e[1m\e[92mEnabled breakpoints on threads\e[0m\n"
      else
        printf "\e[1m\e[91mError: sc-threads bad syntax -- ignored\e[0m\n"
      end
    end
  end
end
document sc-threads
  sc-threads [0|1]
  Break, enable, or disable all user module SC_THREADS.
  Assumes naming convention Name_module::*
end
printf "Defined sc-threads\n"

# vim:syntax=gdb:nospell
