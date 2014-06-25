@echo off
pushd src
ctags *.h *.cxx --language-force=c++ --c++-kinds=pf
popd
pushd examples
ctags ..\src\*.h ..\src\*.cxx *.cxx --language-force=c++ --c++-kinds=pf
popd

