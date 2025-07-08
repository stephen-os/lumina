@echo off

pushd ..
Dependencies\premake\premake5.exe --file=lumina.lua vs2022
popd
pause