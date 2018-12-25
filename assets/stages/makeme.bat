::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::                         Sexy Makefile Executor                               ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@echo off
for %%I in (.) do set CurrDirName=%%~nxI
IF "%~1"=="-silent" goto compile

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::                               Change Here                                    ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

set ROOT=c:\ultra


::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::                          Color Function Setup                                ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

cls && color 07
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (set "DEL=%%a")


::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::                                Setup Make                                    ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

call :PainText 09 "Setting up make for the N64"
set gccdir=%ROOT%\gcc
set PATH=%ROOT%\gcc\mipse\bin;%ROOT%\usr\sbin
set gccsw=-mips3 -mgp32 -mfp32 -funsigned-char -D_LANGUAGE_C -D_ULTRA64 -D__EXTENSIONS__
set n64align=on
set GCC_CELF=ON

cd %~dp0

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::                           Cleanup Old Files                                  ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

echo.
echo.
call :PainText 02 "Cleaning up old files"
del /s /q /f *.o >nul 2>&1

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::                                Run Make                                      ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:compile
echo.
echo.
call :PainText 0C "Running %CurrDirName% Make"
echo.
make
move %CurrDirName%.o ../include
del *.o
IF "%~1"=="-silent" (
exit /B 2
)

echo.
call :PainText 0E "Done!"
echo.
echo.
pause
exit

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::                                Color Function                                ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:PainText
<nul set /p=". ."
<nul set /p "=%DEL%" > "%~2"
\Windows\system32\findstr /v /a:%1 /R "+" "%~2" nul
del "%~2" > nul
<nul set /p=""
goto :eof