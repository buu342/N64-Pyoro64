::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::                         Sexy Makefile Executor                               ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@echo off
@chcp 1252
setlocal enabledelayedexpansion
set MOVEROM=1
set ERROR=0

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::                               Change Here                                    ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Ultra folder (NO QUOTATIONS)
set ROOT=C:\ultra
:: Emulator ROM folder (QUOTATIONS REQUIRED)
set ROMFOLDER="Z:\"


::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::                          Color Function Setup                                ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

cls && color 07
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (set "DEL=%%a")


::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::                            Check for folders                                 ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

IF NOT EXIST %ROOT% (
goto fix1
) ELSE (
goto fix2
)

:fix1
call :PainText 04 "Error locating the ultra folder."
echo.
call :PainText 04 "Please edit this batch file (line 16) to have the correct directory."
echo.
echo.
pause
exit

:fix2
for /F "delims=" %%a in ('@findstr /s /b "N64KITDIR" "Makefile" ') do set "n64kitdir=%%a"
for /f "tokens=2 delims==" %%a in ("%n64kitdir%") do set "result=%%a"
IF NOT EXIST %result% (
call :PainText 04 "Error locating the n64sdk folder."
echo.
call :PainText 04 "Please edit your makefile to have the correct directory."
echo.
echo.
pause
exit
)
goto fix3

:fix3
IF NOT EXIST %ROMFOLDER% (
call :PainText 04 "Error locating ROM folder."
echo.
call :PainText 04 "Please edit this batch file (line 18) to have the correct directory."
echo.
call :PainText 04 "if you ignore this error, the rom will not be moved after compilation."
echo.
echo.
set MOVEROM=0
)
goto main


::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::                                Setup Make                                    ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:main
call :PainText 09 "Setting up make for the N64"
set gccdir=%ROOT%\gcc
set PATH=%ROOT%\gcc\mipse\bin;%ROOT%\usr\sbin
set gccsw=-mips3 -mgp32 -mfp32 -funsigned-char -D_LANGUAGE_C -D_ULTRA64 -D__EXTENSIONS__
set n64align=on
set GCC_CELF=ON


::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::                           Cleanup Old Files                                  ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

echo.
echo.
call :PainText 02 "Moving to the compilation directory"
echo.
call :PainText 02 "Cleaning up old files"
echo.
call :PainText 02 "Creating Out Folder"
@mkdir out 2>nul
cd out
del /q *
echo.
call :PainText 02 "Creating Bin Folder"
@mkdir bin 2>nul
cd bin
del /q *
cd..
cd..


::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::                                Run Make                                      ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

cd assets
cd sprites
call makeme.bat -silent
cd..
cd..

cd assets
cd objects
call makeme.bat -silent
cd..
cd..

cd assets
cd stages
call makeme.bat -silent
cd..
cd..

echo.
echo.
call :PainText 0C "Running Make"
echo.
make

if NOT exist *.n64 (
move /-y *.o "out\bin\" >nul
echo.
call :PainText 04 "An error occured during compiation."
echo.
echo.
pause
exit
)

echo.
call :PainText 0C "Running makemask"
echo.
for /R "%~dp0" %%f in (*.n64) do makemask "%%f"


::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::                       Move files to output folder                            ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

echo.
call :PainText 02 "Moving Object files"
move /-y *.o "out\bin\" >nul
move /-y *.out "out\bin\" >nul
move /-y *.n64 "out\" >nul
echo.

echo.
call :PainText 0E "Compilation finished!"
echo.


::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::                       Ask to move file to ROM folder                         ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

if !MOVEROM!==1 (
goto choice
) ELSE (
goto finish
)

:choice
set /P c=Move the ROM to the Project64 ROM Folder[Y/N]?
if /I "%c%" EQU "Y" goto :move
if /I "%c%" EQU "N" call :PainText 0E "Done!" & echo. & echo. & pause & exit
goto :choice

:move
for /R "%~dp0out" %%f in (*.n64) do copy /y "%%f" %ROMFOLDER%
call :PainText 0E "Done!"
echo.
goto finish

:finish
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