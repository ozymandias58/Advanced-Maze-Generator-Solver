@echo off
color a
title Git Helper Pro
setlocal enabledelayedexpansion

:MENU
cls
echo ======================================================
echo           GIT YARDIMCISI - MAZE SOLVER PRO
echo ======================================================
echo  [1] Guncellemeleri Al (GIT PULL)
echo  [2] Degisiklikleri Gonder (GIT PUSH)
echo  [3] Cikis
echo ======================================================
echo.

set /p secim="Seciminizi yapin (1-3): "

if "%secim%"=="1" goto PULL_OP
if "%secim%"=="2" goto PUSH_OP
if "%secim%"=="3" exit
goto MENU

:PULL_OP
echo.
echo [+] Guncellemeler cekiliyor...
git pull
echo.
echo [+] Islem tamamlandi. Menuye donmek icin bir tusa basin.
pause > nul
goto MENU

:PUSH_OP
echo.
echo [+] Degisiklikler kontrol ediliyor...
git add .

:: Kullanıcıdan commit mesajı alıyoruz
echo.
set /p commit_msg="Commit mesajini gir gardasim: "

:: Mesaj boşsa uyarı ver
if "%commit_msg%"=="" (
    echo [HATA] Mesaj bos olamaz! Tekrar dene.
    pause
    goto PUSH_OP
)

echo.
echo [+] Commit yapiliyor...
git commit -m "%commit_msg%"

echo.
echo [+] Repo'ya gonderiliyor (Push)...
git push

echo.
echo [+] Islem basariyla tamamlandi!
pause
goto MENU