:: ########################################################################################
::  Start
:: ########################################################################################
@echo off

:: ### Check Python3
call :info
echo * CHECKING FOR PYTHON3
echo.
echo ---
python --version
echo ---
echo.
if %errorlevel% neq 0 (
echo Python3 is not installed or cannot be found.
call :weiter "Press any key to start manually installation ..."
goto :install_python_pip
)
echo Python3 is installed
call :weiter

:: ###  Check Pip3
call :info
echo * CHECKING FOR PIP3
echo.
echo ---
pip3 --version
echo ---
echo.
if %errorlevel% neq 0 (
echo Pip3 is not installed or cannot be found.
call :weiter "Press any key to start a manual installation ..."
goto :install_python_pip
)
echo Pip3 is installed
echo.
echo * UPDATING PIP3
echo.
echo ---
python -m pip install --upgrade pip
echo ---
echo.
echo Pip3 is updated
call :weiter "Press any key to check install the required modules"

:: ###  Check Python modules
call :info
echo * INSTALLATION OF REQUIRED PYTHON MODULES
echo.
echo ---
pip install --user PyQt5 --upgrade
set errmod=%errorlevel%
echo ---
echo.
if %errmod% neq 0 goto :modtesterr
goto :modtestok
:modtesterr
echo Something went wrong ... Sorry ...
echo.
echo Please check the protocol an install everything manually ...
call :weiter "Press key to start information on manual installation"
goto :module_installation_error
:modtestok
echo All modules have been installed successfully
call :weiter

:: ### Start BSRNtoolbox und Ende
:startbsrn
call :info
echo * STARTING BSRNtoolbox
echo.
echo BSRNtoolbox should open any second now ...
echo.
start /B python start.pyw
goto :ende

:: ########################################################################################
:: Functions
:: ########################################################################################

:: ###  Info: Weiter
:weiter
if [%1]==[] (set text="Press any key to proceed ...") else (set text=%1%)
set text=%text:"=%
echo.
echo ----------------------------------------------------
echo %text%
echo ----------------------------------------------------
pause >nul
exit /b 0

:: ###  Install Python/Pip
:install_python_pip
start "" https://www.python.org/download
call :info
echo * DOWNLOAD AND INSTALL PYTHON3 AND PIP3 MANUALLY
echo.
echo 1) Download the latest Python3 version (Version 3.5 or later) form
echo    this location: "https://www.python.org/download"
echo    The website should have been opened in your browser now.
echo.
echo 2) During installation ...
echo - ... first check "Add Python to PATH"
echo - ... then click on "Customize installation"
echo - ... check everything there
echo - ... click on "Next"
echo - ... in "Advanced Option" check "Associate files with Python"
echo - ... finally click on "Install"
echo - ... wait untill the installation is done.
echo.
echo 3) Quit this script and restart it manually.
call :weiter "Press any key to close this window ..."
::start cmd /c "install.bat"
goto :ende

:: ###  Module installation error
:module_installation_error
call :info
echo * Manual installation information
echo.
echo Opening manual information in text editor.
echo %cd%
start notepad "readme.txt"
call :weiter "Press any key to close this window ..."
goto :ende

:: ###  Ende
:ende
exit /b 0

:: ###  Info
:info
@echo off
cls
echo.
echo ----------------------------------------------------
echo.
echo  BSRN Toolbox
echo.
echo                Guided Installation
echo.
echo ----------------------------------------------------
echo.
exit /b 0
