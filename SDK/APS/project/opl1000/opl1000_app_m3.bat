@ECHO OFF
SET OBJ_PATH=%1
SET AXF_FULL_PATH=%2
SET OUTPUT_NAME=%3
SET LST_PATH=%OBJ_PATH%..\Listings\
REM #1 [$L] = ..\..\APS\targets\opl1000\Output\Objects\
REM #2 [!L] = ..\..\APS\targets\opl1000\Output\Objects\opl1000_app_m3.axf
REM #3 [@L] = opl1000_app_m3


REM #Remove file
IF EXIST %OBJ_PATH%%OUTPUT_NAME%.s19 DEL %OBJ_PATH%%OUTPUT_NAME%.s19 /Q
IF EXIST %OBJ_PATH%%OUTPUT_NAME%.bin DEL %OBJ_PATH%%OUTPUT_NAME%.bin /Q

SET FROM_ELF="C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe"

%FROM_ELF% %AXF_FULL_PATH% --m32combined --output %OBJ_PATH%%OUTPUT_NAME%.s19
%FROM_ELF% %AXF_FULL_PATH% --bin --output %OBJ_PATH%%OUTPUT_NAME%.bin

REM ====================================================================
REM Dump object sizes
SET SIZE_FILE=%LST_PATH%%OUTPUT_NAME%_size.txt
SET PATCH_LIB=%OBJ_PATH%opl1000_patch_m3.lib
SET ERRORLEVEL=0
IF EXIST %SIZE_FILE% ( DEL %SIZE_FILE% )
ECHO "%OUTPUT_NAME%" | FINDSTR /C:"at" > NUL && (
    SET SDK_LIB=%OBJ_PATH%opl1000_sdk_at_m3.lib
) || (
    SET SDK_LIB=%OBJ_PATH%opl1000_sdk_m3.lib
)

ECHO.
ECHO %PATCH_LIB% >> %SIZE_FILE%
%FROM_ELF% --info=totals %PATCH_LIB% >> %SIZE_FILE%
ECHO %SDK_LIB% >> %SIZE_FILE%
ECHO.
%FROM_ELF% --info=totals %SDK_LIB% >> %SIZE_FILE%
ECHO.
ECHO %AXF_FULL_PATH% >> %SIZE_FILE%
%FROM_ELF% --info=totals %AXF_FULL_PATH% >> %SIZE_FILE%
ECHO.
ECHO.
ECHO.
