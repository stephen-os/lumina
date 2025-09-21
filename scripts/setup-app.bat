@echo off

pushd ..
Dependencies\premake\premake5.exe --file=lumina-app.lua vs2022
popd
pause