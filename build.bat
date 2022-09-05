@echo off
cls
color 0f
echo [== COMPILING CODE ==]
rem IF EXIST ".\out" rmdir /s /q ".\out"
rem mkdir out
"F:\Quality Software\OperatingSystemDevTools\mingw64\bin\mingw32-make.exe"
if %ERRORLEVEL% NEQ 0 (GOTO ERR)

rem wsl sudo apt install grub-common xorriso
cd .\out
wsl grub-mkrescue -o ATA.iso isodir
cd ..\

echo.
if %ERRORLEVEL% EQU 0 (echo [== DONE ==] && "F:\Quality Software\OperatingSystemDevTools\vms\qemu-system-x86_64.exe" -kernel .\out\ATA.bin -serial stdio -hda .\TestIMG.img)
if %ERRORLEVEL% NEQ 0 (GOTO ERR)

:done
    exit

:ERR
    echo [== ERROR OCCURRED! ==]
    color 04