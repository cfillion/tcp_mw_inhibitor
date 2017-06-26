## Build Setup

Download these files into the `vendor` directory:

1. reaper_plugin.h from
  [reaper.fm](http://www.reaper.fm/sdk/plugin/reaper_plugin.h)
2. [WDL](http://www.cockos.com/wdl/) from Cockos:
  `git clone http://www-dev.cockos.com/wdl/WDL.git vendor/WDL`
3. reaper_plugin_functions.h from the REAPER action
  "[developer] Write C++ API functions header"

### OS X

1. Install [Homebrew](http://brew.sh/) and Xcode Command Line Tools
2. Install [tup](http://gittup.org/tup/):
  `brew tap homebrew/fuse && brew install tup`
3. Run `tup` from this directory
4. Copy or link `x64/bin/reaper_tcp_mw_inhibitor64.dylib` or `x86/bin/reaper_tcp_mw_inhibitor32.dylib`
   to REAPER's extension directory

### Windows

1. Install [tup](http://gittup.org/tup/win32/tup-explicit-variant-v0.7.3-45-gcf6a829.zip)
  (explicit-variant branch, see [this
  thread](https://groups.google.com/d/topic/tup-users/UNUSE15PQdA/discussion))
  and [Visual Studio 2015, with Visual C++](https://www.visualstudio.com/products/visual-studio-community-vs)
2. Prevent Microsoft's C++ compiler from saving telemetry outside of the build directory:
   [instructions here](https://msdn.microsoft.com/en-us/library/ee225238.aspx#Anchor_5)
   or set the `OptIn` registry key to `0` in
   `HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\VSCommon\14.0\SQM`
6. Run `tup` from this directory using "Developer Command Prompt for VS2015"
7. Copy or symlink `x64\bin\reaper_tcp_mw_inhibitor64.dll` or `x86\bin\reaper_tcp_mw_inhibitor32.dll`
   to your REAPER plugin folder
