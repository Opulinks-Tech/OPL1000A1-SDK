# Content
This folder contains binary firmware that used to:

- Use AT command to evaluate OPL1000 functions
- evaluate the RAM usage of  user's applications

How to use these binary file and complete firmware pack, download operation, please refer document "OPL1000-patch-download-tool-user-guide.pdf" (Chinese version under .\Doc\zh_CN directory) or "OPL1000-patch-download-tool-user-guide_ENG.pdf" (English version under .\Doc\en directory)  to know detailed information. 

# File list and brief description
1. opl1000.bin:  OPL1000 minimum size firmware file. Users can use it to evaluate the RAM usage of  their applications.  
2. opl1000_at.bin: OPL1000 integral firmware file that support AT command.  
3. opl1000_at_ext_ldo.bin: OPL1000 integral firmware file that support AT command. It works with devkits using  external LDO power supply. 

[Note] 

1. In order to understand AT command. Please refer document "OPL1000-AT-instruction-set-and-examples.pdf" (Chinese version under .\Doc\zh_CN directory) or "OPL1000-AT-instruction-set-and-examples_ENG.pdf" (English version under .\Doc\en directory)  to know detailed information. Update firmware through BLE connection (BLE OTA). 
2. OPL1000 hardware setting is different when adopt different power supply method. Hence user shall choose opl1000_m0.bin or opl1000_m0_ldo.bin according to hardware design scheme. 
3. When user develops own application, M3 binary will be created by user. And it shall be packed with  M0 binary file.  

