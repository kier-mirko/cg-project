@echo off

if not exist build mkdir build

set opts=/nologo /FC /W3 /WX /wd4101 /Zi /Zc:preprocessor /I..\src\third_party\glfw\include /I..\src\third_party\glad\include
pushd build
cl %opts% ..\src\cg\main.c user32.lib gdi32.lib shell32.lib glfw3.lib -Femain /link /LIBPATH:..\src\third_party\glfw\lib
popd
