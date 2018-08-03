@SET OBJ_PATH=%1
@SET AXF_FULL_PATH=%2
@SET OUTPUT_NAME=%3
@REM #1 [$L] = .\Output\Objects\
@REM #2 [!L] = .\Output\Objects\opl1000_app_m3.axf
@REM #3 [@L] = opl1000_app_m3

@REM #Remove file
@IF EXIST %OBJ_PATH%%OUTPUT_NAME%.s19 DEL %OBJ_PATH%%OUTPUT_NAME%.s19 /Q
@IF EXIST %OBJ_PATH%%OUTPUT_NAME%.bin DEL %OBJ_PATH%%OUTPUT_NAME%.bin /Q

@SET AUTO_GEN_RELEASE=0
@SET FROM_ELF=C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe
@SET SREC_CAT=..\..\..\..\APS\tools\srec_cat.exe

%FROM_ELF% %AXF_FULL_PATH% --m32combined --output %OBJ_PATH%%OUTPUT_NAME%.s19
%FROM_ELF% %AXF_FULL_PATH% --bin --output %OBJ_PATH%%OUTPUT_NAME%.bin
%SREC_CAT% %OBJ_PATH%%OUTPUT_NAME%.bin -binary -byte-swap 4 -o %OBJ_PATH%%OUTPUT_NAME%_srecord.hex -vmem 32

@REM C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe .\Output\Objects\opl1000_app_m3.axf --m32 --output .\Output\Objects\opl1000_app_m3.s19
@REM C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe .\Output\Objects\opl1000_app_m3.axf --bin --output .\Output\Objects\opl1000_app_m3.bin
@REM ..\..\tools\srec_cat.exe .\Output\Objects\opl1000_app_m3.bin -binary -byte-swap 4 -o .\Output\Objects\opl1000_app_m3_srecord.hex -vmem 32














