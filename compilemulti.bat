@echo off
rem low level CRT-Less compiler flags
gcc multithrd.c -o multithrd.exe -nostdlib -e entry -lkernel32 -luser32 -lws2_32 -O -s
pause