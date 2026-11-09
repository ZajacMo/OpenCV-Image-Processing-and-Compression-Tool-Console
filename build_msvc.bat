@echo off
REM Helper used during development to compile the project with MSVC.
REM Usage: build_msvc.bat [output-dir]   (default output dir: build)
setlocal
set "PATH=C:\Program Files (x86)\Microsoft Visual Studio\Installer;C:\Program Files\opencv\build\x64\vc16\bin;%PATH%"
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
cd /d "%~dp0"
if "%~1"=="" (set "OUTDIR=build") else (set "OUTDIR=%~1")
if not exist "%OUTDIR%" mkdir "%OUTDIR%"
cl /nologo /EHsc /std:c++17 /O2 /I "C:\Program Files\opencv\build\include" ^
   Compress_gray.cpp Bits.cpp Block.cpp RBlock.cpp main.cpp ^
   /Fe:"%OUTDIR%\compression_tool.exe" ^
   /Fo:"%OUTDIR%\\" ^
   /link /LIBPATH:"C:\Program Files\opencv\build\x64\vc16\lib" opencv_world4100.lib
if errorlevel 1 (
  echo BUILD FAILED
  exit /b 1
)
echo BUILD OK -^> %OUTDIR%\compression_tool.exe
