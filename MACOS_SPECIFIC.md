# MacOS installation of SystemC

WARNING: The following instructions are offered without any warranty or support. You are expected to be self-sufficient. Your mileage may vary.

Apple's MacOS has some peculiarities for the uninitiated. So I offer the following to help you get started developing SystemC on this platform. I have been using Mac since 1984 and developed with SystemC since 2001.

Without further ado, here is what to do on MacOS.

1. Install **Xcode** from Apple via the App Store

2. Install the Xcode command-line tools from the terminal (I use **iTerm2** from https://iterm2.com )

   ```bash
   xcode-select --install
   ```

3. Install a recent version of **CMake** (two methods):

   1. Go to https://cmake.org/getting-started/ and follow directions
   2. Alternately (my preference), on MacOS install **HomeBrew** by going to https://brew.sh/
      1. Follow directions for brew
      2. `brew install cmake`
   3. Alternately, on Windows 10 or 11 install WSL2 (Windows Subsystem Linux)
      1. Install WSL2 using https://www.ceos3c.com/wsl-2/install-wsl2-with-windows-terminal
      2. I recommend choosing Ubuntu 22.04
      3. `sudo apt install cmake`

      Note: brew also works on ubuntu; however, I have not tested this.

Now, follow the instructions to install SystemC from the README.md on https://github.com/Doulos/debug_systemc.git

Some issues I've run into:

1. If you recently changed from Intel to Apple Silicon, you may need to find and remove x86_64 libraries to avoid them being accidentally linked. 

2. The cmake_minimum_required may need to be updated due to deprecation issues. The following hack finds and updates all instances:

   ````bash
   AFFECTED="$(find . -name CMakeLists.txt)"
   PERLSCRIPT='s/\d(\.\d+)*/3.20/ if m/cmake_minimum_required/'
   perl -pi -e "${PERLSCRIPT}" "${AFFECTED}"
   ````

3. The latest version for public comment has a spurious file in the install that needs to be skipped (it will be fixed in a later update). Meanwhile, the following will fix the problem:

   ````	bash
   # Do this from the SYSTEMC_SRC directory before configuring
   AFFECTED="$(find . -path '*/docs/CMakeLists.txt')"
   PERLSCRIPT='s/^/# / if m/DEVELOPMENT.md/'
   perl -pi -e "${PERLSCRIPT}" "${AFFECTED}"
   ````
4. Keep brew updated.

Observation: `brew` has a formulae for `systemc`. I have not tried it.


