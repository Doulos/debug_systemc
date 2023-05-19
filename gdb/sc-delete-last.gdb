define lld
  disable $_hit_bpnum.$_hit_locno
end
printf "Defined lld -- disable breakpoint at last location\n"

define lbd
  disable $_hit_bpnum
end
printf "Defined lbd -- disable entire set of breakpoints associated with most recent\n"

# vim:syntax=gdb:nospell
