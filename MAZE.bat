@echo off
color a
setlocal enabledelayedexpansion

echo ======================================================
echo    Maze Solver Pro: Derleme ve Calistirma Baslatildi
echo ======================================================

:: 1. Backend'i (C) Derle
echo [1/3] C dosyalarini toplaniyor ve derleniyor...

:: PowerShell ile debugNoGUI.c haricindeki tum .c dosyalarini bulup derliyoruz
powershell -Command "$files = Get-ChildItem -Path 'backend\src' -Recurse -Filter *.c | Where-Object { $_.Name -ne 'debugNoGUI.c' } | Select-Object -ExpandProperty FullName; gcc -I 'backend\include' $files -o 'frontend\src\Demo.exe'"

if %errorlevel% neq 0 (
    echo [HATA] Backend derlenirken bir sorun olustu!
    pause
    exit /b %errorlevel%
)
echo [OK] Demo.exe olusturuldu.

:: 2. Frontend'i (Java) Derle
echo [2/3] Java GUI derleniyor...
cd frontend\src
javac MazeApp.java

if %errorlevel% neq 0 (
    echo [HATA] Java derlenirken bir sorun olustu!
    pause
    exit /b %errorlevel%
)
echo [OK] MazeApp.class hazir.

:: 3. Uygulamayı Çalıştır
echo Uygulama baslatiliyor...
start javaw MazeApp
exit