@echo off
setlocal

rmdir /S /Q bin\out\pack
mkdir bin\out\pack
mkdir bin\out\pack\gordian-8-win32-rel
mkdir testdata\packages

copy bin\out\debug\archive.dll bin\out\pack\gordian-8-win32-rel\.
copy bin\out\debug\console.dll bin\out\pack\gordian-8-win32-rel\.
copy bin\out\debug\curator.dll bin\out\pack\gordian-8-win32-rel\.
copy bin\out\debug\exec.dll bin\out\pack\gordian-8-win32-rel\.
copy bin\out\debug\file.dll bin\out\pack\gordian-8-win32-rel\.
copy bin\out\debug\gordian.exe bin\out\pack\gordian-8-win32-rel\.
copy bin\out\debug\http.dll bin\out\pack\gordian-8-win32-rel\.
copy bin\out\debug\store.dll bin\out\pack\gordian-8-win32-rel\.
copy bin\out\debug\tcatbin.dll bin\out\pack\gordian-8-win32-rel\.
copy bin\out\debug\xcopy-deploy.bat bin\out\pack\gordian-8-win32-rel\.

echo # vi: ft=json>bin\out\pack\gordian-8-win32-rel.sst
echo {>>bin\out\pack\gordian-8-win32-rel.sst
echo    "name": "gordian",>>bin\out\pack\gordian-8-win32-rel.sst
echo    "version": 8,>>bin\out\pack\gordian-8-win32-rel.sst
echo    "serialize-upgrade": true,>>bin\out\pack\gordian-8-win32-rel.sst
echo    "defer-uninstall": true>>bin\out\pack\gordian-8-win32-rel.sst
echo }>>bin\out\pack\gordian-8-win32-rel.sst

bin\out\debug\gordian --pack bin\out\pack\gordian-8-win32-rel

copy bin\out\pack\gordian-8-win32-rel.ar.z.s testdata\packages\.
copy bin\out\pack\gordian-8-win32-rel.sst testdata\manifests\.
rmdir /S /Q testdata\packages\gordian-8-win32-rel
