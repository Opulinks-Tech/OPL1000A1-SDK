c:\Keil_v5\ARM\ARMCC\bin\fromelf.exe Output\Objects\opl1000_app_m3.axf --m32 --output Output\Objects\opl1000_app_m3.s19
c:\Keil_v5\ARM\ARMCC\bin\fromelf.exe Output\Objects\opl1000_app_m3.axf --bin --output Output\Objects\opl1000_app_m3.bin
..\..\..\..\APS\tools\srec_cat.exe Output\Objects\opl1000_app_m3.bin -binary -byte-swap 4 -o Output\Objects\opl1000_app_m3_srecord.hex -vmem 32