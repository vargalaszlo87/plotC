@echo off
set DIR=plotc

if "%~1"=="" (
    echo Usage: build.bat sourcefile.c
    exit /b 1
)

for %%f in (%1) do set FNAME=%%~nf

gcc %1 %DIR%/src/plotc.c %DIR%/src/events.c %DIR%/src/dyndll.c -I%DIR%/include -L%DIR%/lib/windows -o %FNAME%.exe -lopengl32 -lgdi32 -std=c99

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
) else (
    echo Build succeeded: %FNAME%.exe
)