@echo off
if "%1"=="r" (goto run)
echo Compiling...
clang -std=c++17 main.cpp -o main.exe -lSDL2 -lSDL2main -luser32
if %errorlevel% == 0 (echo Successfully Compiled!) else (goto error)
if "%1"=="c" (goto done) else (goto run)

:run
echo Running...
echo.
main.exe
goto done

:error
echo Compilation failed

:done