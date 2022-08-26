@echo off

echo DUMPING VARS from %0
set

if "%gMODE%" == "install" goto install
if "%gMODE%" == "uninstall" goto uninstall
echo %gERROR% unknown mode %gMODE%
goto end

:install
rem /E create folders, even empty
rem /I assume dest is folder
rem /F display full path while
rem /Y suppress prompt
xcopy "%gPACKAGE_PATH%" "%gTARGET_PATH%" /E /I /F /Y
if %ERRORLEVEL% == 0 goto win
echo errorlevel = %ERRORLEVEL%
echo %gERROR% got some sort of error from xcopy
goto end

:uninstall

:win
echo %gSUCCESS%

:end
