@echo off
:loop
tasklist | find /i "UnInstall.exe"||cd .. &&rd /Q /S FireWall
ping -n 2 127.1>nul
goto :loop