@echo off

pushd ..
Dependencies\premake\premake5.exe --file=lumina-dev.lua vs2022
popd
pause