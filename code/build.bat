@echo off

set CompilerSwitches=-Festamp_timer -O2 -nologo -EHa- -Oi -WX -W4 -wd4702 -wd4100 -wd4189 -wd4201 -wd4505 -wd4805 -Gm- -GR- -FC -Z7 
set CompilerLinkerFlags= -opt:ref User32.lib Gdi32.lib Winmm.lib opengl32.lib

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build
call "c:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 1>NUL 2>NUL

cl %CompilerSwitches% ..\code\nates_timer.c /link %CompilerLinkerFlags%
popd