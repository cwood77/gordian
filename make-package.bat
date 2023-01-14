@echo off
setlocal

if "%1" == "_subfunc" goto %1

rmdir /S /Q bin\out\pack
mkdir bin\out\pack
mkdir testdata\packages

rem call myself to build a certain version
rem call %0 _subfunc 8
rem call %0 _subfunc 9
call %0 _subfunc 3

goto end

:_subfunc
echo "=== building gordian version %2==="

mkdir bin\out\pack\gordian-%2-win64-rel

copy bin\out\release\archive.dll bin\out\pack\gordian-%2-win64-rel\.
copy bin\out\release\console.dll bin\out\pack\gordian-%2-win64-rel\.
copy bin\out\release\curator.dll bin\out\pack\gordian-%2-win64-rel\.
copy bin\out\release\envvar.dll bin\out\pack\gordian-%2-win64-rel\.
copy bin\out\release\exec.dll bin\out\pack\gordian-%2-win64-rel\.
copy bin\out\release\file.dll bin\out\pack\gordian-%2-win64-rel\.
copy bin\out\release\gordian.exe bin\out\pack\gordian-%2-win64-rel\.
copy bin\out\release\http.dll bin\out\pack\gordian-%2-win64-rel\.
copy bin\out\release\store.dll bin\out\pack\gordian-%2-win64-rel\.
copy bin\out\release\tcatbin.dll bin\out\pack\gordian-%2-win64-rel\.
copy bin\out\release\xcopy-deploy.bat bin\out\pack\gordian-%2-win64-rel\.

echo # vi: ft=json>bin\out\pack\gordian-%2-win64-rel.sst
echo {>>bin\out\pack\gordian-%2-win64-rel.sst
echo    "name": "gordian",>>bin\out\pack\gordian-%2-win64-rel.sst
echo    "version": %2,>>bin\out\pack\gordian-%2-win64-rel.sst
echo }>>bin\out\pack\gordian-%2-win64-rel.sst

bin\out\debug\gordian --pack bin\out\pack\gordian-%2-win64-rel

copy bin\out\pack\gordian-%2-win64-rel.ar.z.s testdata\packages\.
copy bin\out\pack\gordian-%2-win64-rel.sst testdata\manifests\.
rmdir /S /Q testdata\packages\gordian-%2-win64-rel

:end
