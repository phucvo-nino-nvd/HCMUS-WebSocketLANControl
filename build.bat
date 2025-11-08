@echo off
REM ================================
REM Build script for comp_net client & server
REM ================================

REM 1. Cấu hình compiler path
set COMPILER=C:\msys64\mingw64\bin\g++.exe

REM 2. Include paths
set BOOST_INC=lib\boost_1_89_0
set OPENCV_INC=lib\opencv-mingw\include

REM 3. Library paths
set OPENCV_LIB=lib\opencv-mingw\x64\mingw\lib

REM 4. Output file names
set CLIENT_OUTPUT=client.exe
set SERVER_OUTPUT=server.exe

REM =====================================
REM Build SERVER
REM =====================================
echo Building %SERVER_OUTPUT% ...
"%COMPILER%" server.cpp -o %SERVER_OUTPUT% ^
 -I%BOOST_INC% ^
 -lws2_32 ^
 -static-libstdc++ -static-libgcc

IF %ERRORLEVEL% NEQ 0 (
    echo Server build failed!
    pause
    exit /b %ERRORLEVEL%
) ELSE (
    echo Server build successful! Output: %SERVER_OUTPUT%
)

echo.

REM =====================================
REM Build CLIENT
REM =====================================
echo Building %CLIENT_OUTPUT% ...
"%COMPILER%" client.cpp -o %CLIENT_OUTPUT% ^
 -I%BOOST_INC% ^
 -I%OPENCV_INC% ^
 -L%OPENCV_LIB% ^
 -lopencv_core455 -lopencv_highgui455 -lopencv_videoio455 -lopencv_imgproc455 ^
 -lws2_32 -luser32 -lgdi32 -lcomdlg32 -lole32 -loleaut32 -luuid -static-libstdc++ -static-libgcc

IF %ERRORLEVEL% NEQ 0 (
    echo Client build failed!
    pause
    exit /b %ERRORLEVEL%
) ELSE (
    echo Client build successful! Output: %CLIENT_OUTPUT%
)

echo.
echo =====================================
echo Build completed successfully!
echo Server: %SERVER_OUTPUT%
echo Client: %CLIENT_OUTPUT%
echo =====================================

pause