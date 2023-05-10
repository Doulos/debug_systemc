# About extern/bin

This directory contains executable scripts used or useful to the course. If the course has been setup for your the instructor, you may not need to use this at all. The intent is that these scripts are stand-alone; however, if you discover a dependency needs to be added, please consider cloning https://github.com/dcblack/portable-scripts. 

Here is a brief description of the files contained:

```
├── Essential-IO ──── BASH support functions
├── README.md ─────── this file
├── build ─────────── convenience script to invoke cmake and all of the other build scripts
├── build-catch2 ──── downloads, compiles and installs the Catch2 test framework under extern/
├── build-expected ── downloads, compiles and installs the std::expect alternative tl::expect for pre-C++20 under extern/
├── build-fmt ─────── downloads, compiles and installs the std::format alternative fmt::format for pre-C++20 under extern/
├── build-gsl ─────── downloads and installs the C++ Core Guidelines Support Library found on cppreference.com under extern/include
├── build-gtest ───── downloads, compiles and installs the GoogleTest and GoogleMock framework under extern/
├── build-systemc ─── downloads, compiles and installs Accellera SystemC into ${HOME}/.local/apps/systemc
└── utils.bash ────── BASH support functions
```

For more information, ask your Doulos instructor or contact David Black <mailto:David.Black@Doulos.com>.
