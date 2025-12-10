@echo off
cd "%~dp0/src"

rd /q /s "bin"
md "bin"

rd /q /s "obj"
md "obj"

windres.exe -i "Resources\Application.rc" -o "obj\Application.o"
gcc.exe -Oz -s -Wl,--gc-sections,--exclude-all-symbols -shared -municode -nostdlib -e DllMain "Library.c" "obj\Application.o" -lntdll -lkernel32 -luser32 -o "bin\gamelaunchhelper.dll"