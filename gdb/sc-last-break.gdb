define last-location-delete
  delete $_hit_bpnum.$_hit_locno
end
printf "Defined last-location-delete\n"

define last-breakpoint-delete
  delete $_hit_bpnum
end
printf "Defined last-breakpoint-delete\n"

define last-location-disable
  disable $_hit_bpnum.$_hit_locno
end
printf "Defined last-location-disable\n"

define last-breakpoint-disable
  disable $_hit_bpnum
end
printf "Defined last-breakpoint-disable\n"

# vim:syntax=gdb:nospell
