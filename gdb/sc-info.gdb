# This goes with the Debug class

#------------------------------------------------------------------------------
define sc-help
  call Debug::help()
end
document sc-help
  Display systemc internal Debug class help.
  TODO: Add help on these macros.
end
printf "Defined sc-help\n"

#------------------------------------------------------------------------------
define sc-info
  call Debug::info("itdsv")
end
document sc-info
  Display current situation
end
printf "Defined sc-info\n"

#------------------------------------------------------------------------------
define sc-opts
  call Debug::opts()
end
document sc-opts
  Display current situation
end
printf "Defined sc-opts\n"

#------------------------------------------------------------------------------
define sc-debug
  if $argc == 0
    call Debug::set_debugging(mask1)
  else
    if $arg0 == 0
      call Debug::clr_debugging(mask1)
    else
      call Debug::set_debugging(mask1)
    end
  end
end
document sc-debug
  Set or clear debug status (includes verbosity)
end
printf "Defined sc-debug\n"

#------------------------------------------------------------------------------
define sc-process
  print Debug::process()
end
document sc-process
  Display current process name
end
printf "Defined sc-process\n"

#------------------------------------------------------------------------------
define sc-name
  print Debug::name($arg0)
end
document sc-name
  Display hierarchical name of an sc_object
end
printf "Defined sc-name\n"

#------------------------------------------------------------------------------
define sc-show
  call Debug::info($arg0)
end
document sc-show
  Display a std::string
end
printf "Defined sc-show\n"

# vim:syntax=gdb:nospell
