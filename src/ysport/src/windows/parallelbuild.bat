setlocal
pushd %~dp0

ysrunparallel.exe parallelbuild_win.txt
nmake -nologo

popd
endlocal
