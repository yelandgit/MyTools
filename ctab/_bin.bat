@echo off
if not exist ..\bin  mkdir ..\bin
if exist %1*.exe  xcopy/d/y %1*.exe  ..\bin
if exist %1*.dll  xcopy/d/y %1*.dll  ..\bin
