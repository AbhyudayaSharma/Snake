# Snake

[![Build Status](https://travis-ci.com/AbhyudayaSharma/Snake.svg?branch=master)](https://travis-ci.com/AbhyudayaSharma/Snake)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/AbhyudayaSharma/Snake.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/AbhyudayaSharma/Snake/context:cpp)

A Snake game made in C using libncurses

For Windows, the executable is built inside the <code>Debug</code> or the <code>Release</code> folder, depending on the configuration chosen while building through Visual Studio. The executable is called <code>game</code> on Linux and macOS.

**Note**: PDCurses submodule is required to build on Windows but is NOT required for Linux and macOS. However, Xcode is required on macOS. On Linux, it required to install the ncurses package for your distro.

## Building on Linux
1. Install libncurses
    - For Debian based distros: <code>apt-get install libncurses5-dev</code>
    - For Fedora: <code>yum install ncurses-devel</code>
    - For OpenSUSE: <code>zypper in ncurses-devel</code>
2. Clone the repository
3. Run <code>make</code>
4. Run the executable <code>game</code>

## Building on macOS
1. Download Xcode from Apple Store
2. Run <code>xcode-select --install</code>
3. Clone the repository and <code>cd</code> into the cloned folder
4. Run <code>make</code>

Run <code>make clean</code> to remove compiled files

## Building on Windows with Visual Studio 2017
1. Clone the repository with <code>--recurse-submodules</code> flag.
2. Navigate to <code>PDCurses\wincon</code> and run <code>nmake -f Makefile.vc</code> using the Developer Command Prompt.
3. Open the <code>Snake.sln</code> with Visual Studio. Select x86 Debug or Release and click on the run button.

We are able to to make only 32-bit binaries as PDCurses currently provides only a 32 bit library.
