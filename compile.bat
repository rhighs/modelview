@echo off
cmake . -B build  -S . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cd build
move compile_commands.json ..
MSBuild.exe modelview.sln /p:Configuration=Debug /m:8
move Debug\modelview.* ..
cd ..
