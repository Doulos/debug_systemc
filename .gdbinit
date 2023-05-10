# GDB initialization for SystemC
echo \033[1m\033[94m------ Sourcing
shell echo -n "\033[1m\033[94m `realpath .gdbinit` "
echo \033[1m\033[94m------\033[0m\n
pwd

# Static helpers from debug.hpp
define status
  call Debug::status()
end
define set_debugging
  call Debug::set_debugging(1)
end

define systemc
  set breakpoint pending on
  break Top::Top
  set $b1 = $bp
  commands
    echo \033[1m\033[95mEntered top-level constructor.\n\033[0m
  end
  break sc_main
  set $b2 = $bp
  break sc_interrupt_here
  set $b3 = $bp
  break sc_stop_here
  set $b4 = $bp
  commands $b3 $b4
  backtrace
  frame 3
  end
  # Ignore internals of SystemC
  skip -rfunction ^sc_
  skip -rfunction ^tlm
  skip wait
  catch throw
  catch catch
  rbreak .*_thread
  break breakpoint
  info breakpoints
end
document systemc
  Initialization for debugging systemc
end

echo \033[1m\033[91mAutomatically set up for SystemC debug by invoking systemc\n\033[0m
set args --config inject.cfg
show args

#The end
