@echo off
setlocal

if "%1" == "_subfunc" goto %1

rmdir /S /Q bin\out\pack
mkdir bin\out\pack
mkdir testdata\packages

rem call myself to build a certain version
call %0 _subfunc 8
call %0 _subfunc 9

goto end

:_subfunc
echo "=== building gordian version %2==="

mkdir bin\out\pack\gordian-%2-win32-rel

copy bin\out\debug\archive.dll bin\out\pack\gordian-%2-win32-rel\.
copy bin\out\debug\console.dll bin\out\pack\gordian-%2-win32-rel\.
copy bin\out\debug\curator.dll bin\out\pack\gordian-%2-win32-rel\.
copy bin\out\debug\exec.dll bin\out\pack\gordian-%2-win32-rel\.
copy bin\out\debug\file.dll bin\out\pack\gordian-%2-win32-rel\.
copy bin\out\debug\gordian.exe bin\out\pack\gordian-%2-win32-rel\.
copy bin\out\debug\http.dll bin\out\pack\gordian-%2-win32-rel\.
copy bin\out\debug\store.dll bin\out\pack\gordian-%2-win32-rel\.
copy bin\out\debug\tcatbin.dll bin\out\pack\gordian-%2-win32-rel\.
copy bin\out\debug\xcopy-deploy.bat bin\out\pack\gordian-%2-win32-rel\.

echo # vi: ft=json>bin\out\pack\gordian-%2-win32-rel.sst
echo {>>bin\out\pack\gordian-%2-win32-rel.sst
echo    "name": "gordian",>>bin\out\pack\gordian-%2-win32-rel.sst
echo    "version": %2,>>bin\out\pack\gordian-%2-win32-rel.sst
echo    "serialize-upgrade": true,>>bin\out\pack\gordian-%2-win32-rel.sst
echo    "defer-uninstall": true>>bin\out\pack\gordian-%2-win32-rel.sst
echo }>>bin\out\pack\gordian-%2-win32-rel.sst

bin\out\debug\gordian --pack bin\out\pack\gordian-%2-win32-rel

copy bin\out\pack\gordian-%2-win32-rel.ar.z.s testdata\packages\.
copy bin\out\pack\gordian-%2-win32-rel.sst testdata\manifests\.
rmdir /S /Q testdata\packages\gordian-%2-win32-rel

:end
