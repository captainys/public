setlocal
pushd %~dp0

ysrunparallel.exe parallelbuild_win.txt
nmake -nologo -f makefile.win

popd
endlocal
