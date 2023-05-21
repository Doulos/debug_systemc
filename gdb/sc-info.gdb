# This goes with the Debug class

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
define sc-debugging
  call Debug::set_debugging(mask1)
end
document sc-debugging
  Set verbosity to SC_DEBUG
end
printf "Defined sc-debugging\n"

# vim:syntax=gdb:nospell
