@echo off
:loop
tasklist | find /i "UnInstall.exe"||cd .. 
rd /Q /S FireWall&&exit
ping -n 1 127.1>nul
goto :loop