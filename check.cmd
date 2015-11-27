@echo off

:: Check with cppcheck

cppcheck %* --enable=all^
    --suppress=missingIncludeSystem^
    --suppress=uninitMemberVar^
    src\*.cxx
