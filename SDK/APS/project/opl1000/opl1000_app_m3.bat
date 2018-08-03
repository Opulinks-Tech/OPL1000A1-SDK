@SET OBJ_PATH=%1
@SET AXF_FULL_PATH=%2
@SET OUTPUT_NAME=%3
@REM #1 [$L] = ..\..\APS\targets\opl1000\Output\Objects\
@REM #2 [!L] = ..\..\APS\targets\opl1000\Output\Objects\opl1000_app_m3.axf
@REM #3 [@L] = opl1000_app_m3

@REM #Remove file
@IF EXIST %OBJ_PATH%%OUTPUT_NAME%.s19 DEL %OBJ_PATH%%OUTPUT_NAME%.s19 /Q
@IF EXIST %OBJ_PATH%%OUTPUT_NAME%.bin DEL %OBJ_PATH%%OUTPUT_NAME%.bin /Q

@SET AUTO_GEN_RELEASE=0
@SET FROM_ELF=C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe
@SET SREC_CAT=..\..\tools\srec_cat.exe

%FROM_ELF% %AXF_FULL_PATH% --m32combined --output %OBJ_PATH%%OUTPUT_NAME%.s19
%FROM_ELF% %AXF_FULL_PATH% --bin --output %OBJ_PATH%%OUTPUT_NAME%.bin
%SREC_CAT% %OBJ_PATH%%OUTPUT_NAME%.bin -binary -byte-swap 4 -o %OBJ_PATH%%OUTPUT_NAME%_srecord.hex -vmem 32

@REM C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe ..\..\APS\targets\opl1000\Output\Objects\opl1000_app_m3.axf --m32 --output ..\..\APS\targets\opl1000\Output\Objects\opl1000_app_m3.s19
@REM C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe ..\..\APS\targets\opl1000\Output\Objects\opl1000_app_m3.axf --bin --output ..\..\APS\targets\opl1000\Output\Objects\opl1000_app_m3.bin
@REM ..\..\tools\srec_cat.exe ..\..\APS\targets\opl1000\Output\Objects\opl1000_app_m3.bin -binary -byte-swap 4 -o ..\..\APS\targets\opl1000\Output\Objects\opl1000_app_m3_srecord.hex -vmem 32

@IF %AUTO_GEN_RELEASE%==0 GOTO :END

@SET PATCH_FOLDER=..\..\..\PatchFlash\Bin\
@IF NOT EXIST %PATCH_FOLDER% MD %PATCH_FOLDER%
@IF EXIST %PATCH_FOLDER%\*_m3.bin DEL %PATCH_FOLDER%\*_m3.bin
@COPY Output\Objects\opl1000_app_m3.bin %PATCH_FOLDER% /Y > NUL

:END
