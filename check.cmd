@echo off

:: Check with cppcheck

cppcheck %* --enable=all^
    --inline-suppr^
    --suppress=missingIncludeSystem^
    --suppress=uninitMemberVar^
    src\*.cxx
    examples\*.cxx
