@echo off
pushd src
ctags *.h *.cxx --language-force=c++ --c++-kinds=pf
popd

