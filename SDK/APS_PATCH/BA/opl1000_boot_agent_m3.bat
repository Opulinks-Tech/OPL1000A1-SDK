SET Project=nl1000
SET TargetPath=..\..\APS\project\%Project%\Output\Objects

C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe %TargetPath%\opl1000_boot_agent_m3.axf --m32 --output %TargetPath%\opl1000_boot_agent_m3.s19
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe %TargetPath%\opl1000_boot_agent_m3.axf --bin --output %TargetPath%\opl1000_boot_agent_m3.bin
..\..\APS\tools\srec_cat.exe %TargetPath%\opl1000_boot_agent_m3.bin -binary -byte-swap 4 -o %TargetPath%\opl1000_boot_agent_m3_srecord.hex -vmem 32
..\..\APS\tools\srec_cat.exe %TargetPath%\opl1000_boot_agent_m3.bin -binary -byte-swap 4 -o %TargetPath%\opl1000_boot_agent_m3_srecord.vhx -vmem 32 -line_length 16