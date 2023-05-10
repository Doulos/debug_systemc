# About this example

This example investigates debugging SystemC under GDB.

Note that building and running the ctests will indicate all tests passed, but that is because we intentionally test for the "simulation FAILED" string.  There is a .gdbinit file in this directory. To enable automatic loading you will need to enable this in `$HOME/.gdbinit`  by inserting the following:

```gdb
add-auto-load-safe-path /PATH/TO/THIS/DIRECTORY
```

Alternately, you may simply type the following from the GDB command-prompt:

```gdb
source .gdbinit
```

To invoke GDB (and get to the command-prompt) use either of the following approaches:

```bash
build -gdb
# -OR-
gdb -tui build/debug/debug_systemc
```

#### The end
