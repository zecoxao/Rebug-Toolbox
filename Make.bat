@echo off

set PS3SDK=/c/PSDK3v2
set WIN_PS3SDK=C:/PSDK3v2
set PATH=%WIN_PS3SDK%/mingw/msys/1.0/bin;%WIN_PS3SDK%/mingw/bin;%WIN_PS3SDK%/ps3dev/bin;%WIN_PS3SDK%/ps3dev/ppu/bin;%WIN_PS3SDK%/ps3dev/spu/bin;%WIN_PS3SDK%/mingw/Python27;%PATH%;
set PSL1GHT=%PS3SDK%/psl1ght
set PS3DEV=%PS3SDK%/ps3dev

set CYGWIN=C:\cygwin64\bin

if not exist %CYGWIN%\bash.exe set CYGWIN=C:\msys\1.0\bin
if not exist %CYGWIN%\bash.exe set CYGWIN=C:\PSDK3v2\MinGW\msys\1.0\bin
if not exist %CYGWIN%\bash.exe set CYGWIN=C:\cygwin\bin

set CHERE_INVOKING=1
%CYGWIN%\bash --login -i -c 'make; exec bash'
