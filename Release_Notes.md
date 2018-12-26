## Release Version:
SDK Package: MP v1.4
Patch_Lib : 2995
ROM_CODE: 1655 

## Release Date: 2018/12/26

## Function & Feature Update 
1. Documents updating:
 1.1 OPL1000-AT-instruction-set-and-examples,  R05-v31, add description of OPL1000 acts as TCP server to send data.
 1.2 OPL1000-Power-Consumption-Measurement-Guide, R01-v03, Add board re-work introduction of how to make DEVKIT to “minimum system”
 1.3 OPL1000-multiple-dev-download-tool-user-guide, R01-v03, update document content according to v1.0 MP tool SW functions.
 1.4 OPL1000_peripheral_PWM_application_notes, v0.3, updated for A1 chip. 
2. Tool version information and update:
 2.1 download tool: v0.23. No change since v1.1.1.5;        
 2.2 pin-mux tool: v0.8. No change since v1.1.1.7;
 2.3 MP download tool: v1.0, add UART CTS/RTS trigger mode.         
3. Update gpio example, add D1 LED turn on/off function.
4. tcp_client example is updated, add smart sleep feature.
5. update pwm example, change ch2 from pin22 to pin 18.
6. Add AT cmds for 32K XTAL setting and  MP MAC access.
7. Change RF setting WIFI TX HP from CF to DF.   
8. Optimize the power consumption of SMPS-RF.
9. Token Switch priority optimization.
10. Add API to add WIFI MAC address for MP use.
11.Add BLE TX single tone function.
12.Add AUXADC calibration function.


## Bug Fix List 
1. Fix "Sometimes can not get IP after WIFI connecting" issue .
2. Fix "WiFi MAC PS mode is inconsistent" issue.

## Notes List 
1. Websocket feature is on-developing 
2. More examples such as MQTT is on-developing. 

*****

## Release Version:
SDK Package: MP v1.3
Patch_Lib : 2899
ROM_CODE: 1655 

## Release Date: 2018/12/6

## Function & Feature Update 
1. Documents updating:
 1.1 OPL1000-AT-instruction-set-and-examples,  R05-v30, Add AT+RFTM (BeaconOnlyMode).
2. Tool version information and update:
 2.1 download tool: v0.23. No change since v1.1.1.5;        
 2.2 pin-mux tool: v0.8. No change since v1.1.1.7;   
 2.3 MP download tool: v0.8, no change since MP1.2. 
3. WiFi power-saving enhancement.
4. Modified SDK image to support "at+cipstamac" and "at+bleaddr" command.   
5. AT command support skipping any DATA traffic during connection.

## Bug Fix List 
1. Fix “PA setting would be abnormal" issue if BLE operation is terminated by Token Switch.
2. Fix "V017 deep sleep fail" issue.

## Notes List 
1. Websocket feature is on-developing 
2. More examples such as MQTT, power save are on-developing. 

*****

## Release Version:
SDK Package: MP v1.2
Patch_Lib : 2882
ROM_CODE: 1655 

## Release Date: 2018/11/28

## Function & Feature Update 
1. Documents updating:
    1.1 OPL1000-multiple-dev-download-tool-user-guide, R01-v01, add user guide for download-tool to support mass production.
 1.2 OPL1000-DEVKIT-getting-start-guide, R02-v07, update using ICE interface part.
 1.3 OPL1000-Flash-User-Guide, R01-v03, update chapter 5.
 1.4 OPL1000-Iperf-Measurement-Guide, R01-v03, user guide document for iperf demo. 
 1.5 OPL1000-Demo-BLE-setup-network-guide，R01-v03, update test steps.   
2. Tool version information and update:
 2.1 download tool: v0.23. No change since v1.1.1.5;        
 2.2 pin-mux tool: v0.8. No change since v1.1.1.7;   
 2.3 MP download tool: v0.8, support multiple device firmware download for mass production. 
3. Release multiple device download tool to support mass production. Place folder: Tool\Mass_Production .
4. update all examples pinmux setting, enable IO20,IO21 to use M3 ICE interface.
5. update SPI_master example, add multi-byte write function as master.   
6. Build Iperf Example for wifi conection stress test. Folder: examples\wifi\iperf
7. Add new API for update AP’s channel in auto connect list.
8. Rearrange stack and heap size for HTTP with MbedTLS example.
9. Redefine wifi_config_set_skip_dtim function prototype.
10. BLE configure AP demo is updated, mobile APP supports BLE OTA function. 
11. Implement new DCOC algorithm to improve RF performance.
12. Enhance power saving realization, easy to enter into power save mode when there is no wifi data traffic.
13. Refine FIM groups for MAC address.
14. Add new feature of "store the bonding information to flash for LE host" 
15. Support run-time modify retry times and interval in DHCP mechanism.

## Bug Fix List 

1. Fix “can't sleep" issue in smart sleep scenario.
2. Fix "BLE meets disconnection caused by Wi-Fi scanning enabled" issue. 
3. Fix “Add xtal ready checking at warm-boot” issue.
4. Fix bug of “M3 sometimes asserts and lead to watchdog timer reset”.
5. Fix a bug in I2C target address change operation. Before change the target address, need to wait that the master Tx/Rx behavior is finished.
6. Fix "Too short delay time for host changing baud rate” issue.
7. Fix bug of "Send service rsp error in LE function"
8. Fix bug of "token switch priority tuning for WiFi/BLE stable coexistence" 
9. Fix bug of "ICE cannot be connected under power save mode" 


## Notes List 
1. Websocket feature is on-developing 
2. More examples such as MQTT, power save are on-developing.

*****

## Release Version:
SDK Package: MP v1.1
Patch_Lib : 2726
ROM_CODE: 1655 

## Release Date: 2018/10/30

## Function & Feature Update 
1. Documents updating:
1.1 OPL1000-AT-instruction-set-and-examples: R05-V29,  Update AT+CWJAP description.
1.2 OPL1000-SDK-Development-guide: R01-V06, Update section2, example directory is updated. 
1.3 OPL1000-WIFI-BLE-API-guide: MP1.1, update partial WIFI API.
1.4 OPL1000-BLEWIFI-Application-Dev-Guide: R01-V06, add BLE/WiFi ota commands description.
1.5 OPL1000-Demo-BLE-setup-network-guide: R01-V02, update OPL1000 3rd APK, wifi/ble API and the configure scenario.
2. Tool version information and update:
2.1 download tool: v0.23. No change since v1.1.1.5;        
2.2 pin-mux tool: v0.8. No change since v1.1.1.7;   
3. Update blewifi example, beside support BLE configure WIFI AP function, also support BLE OTA and WIFI OTA.  
4. Remove old blewifi example from Bluetooth sub-directory, add new blewifi example under “system” sub-directory.
5. Support over write RF setting parameters in flash by both AT command and CLI command.
6. Enable SARADC to improve WIFI scan AP performance. 
7. Add a new AT command “at+phydump” to support  physical layer information dump function 
8. Update OTP setting and timing function.
9. Add xtal ready checking at warm-boot, this is used to reduce the average warm-boot cost. 
10.  Add a feature “return connect failed info when try to auto connect and failed.”
11. Add API to query current numbers of AP saved in flash.
12. Add setting for BLW HP (high power amplifier) 
13. Implement new physical setting for AGC module. 


## Bug Fix List 
1. Fix bug “ enter sleep fail when timer sleep is waked up by IO trigger” 
2. Fix bug “when read the internal VBAT, the result sometimes is 0.”
3. Fix a pin-mux restore error bug, solve 1mA residual current issue when enter into sleep over 300ms. 
4. Solve “connect security AP failed issue when use directly connect from flash”
5. Solve issue of “WiFi connected distance become shorter after BLE ADV”
6. Solve issue of "Assertion-Fail happens in Token Switch during smart-sleep”
7. For WIFI function, event shall be returned to indicate the status “when invoked auto connect start but there is no ap in the auto connect list”


## Notes List 
1. Websocket feature is on-developing 
2. More examples such as MQTT, power save are on-developing. 

*****

## Release Version:
SDK Package: MP v1.0
Patch_Lib : 2632
ROM_CODE: 1655 

## Release Date: 2018/10/12

## Function & Feature Update 
1. Documents updating:
 1.1 OPL1000-AT-instruction-set-and-examples: R05-V28,  add RF related AT instructions description.    
 1.2 Add a new document "OPL1000-AT-UART-Switch-Command" , introduce how to set and switch AT/Debug UART mode for IO0/IO1.    
2. Tool version information and update:
 2.1 download tool: v0.23. No change since v1.1.1.5;        
 2.2 pin-mux tool: v0.8. No change since v1.1.1.7;   
3. hello_world and blink two examples are updated, add  IO0/1 UART work mode setting code.    
4. Add I2C speed configuration API functions.
5. Add 2nd bootloader, it supports higher UART baud rate (up to 921600bps) for firmware download.   
6. Add BLE 1Mbps/2Mbps switch AT command. 
7. Add AT command to support Uart and DbgUart switch function for IO0 & IO1.
8. Implement new physical setting to improve WiFi performance. 
9. Reduce MSQ stack size to extend IRAM3 size.
10. Add DHCP OPTION to support more APs.
11. Re-architecture LWIP timer scheduling mechanism for smart sleep.
12. Add "dynamically turn on/off OTP and Security IP clock" feature. 


## Bug Fix List 
1. Fix "WiFi MAC stuck during establishing connection" issue. 
2. Fix "TCP tx stress test cause frame encryption failed" issue. 
3. Delay CPOR 16 cycles to avoid siwtch-to-xtal issue.
4. Fix  "DTM cannot transmit over 64 length" issue.  
5. Fix  a bug in BLE HCI command when controller cannot find a RPA associated with the Peer Identity Address.
6. Fix a bug in AT+CWJAP command when connect to a WPA/WPA2 AP without key in password.
7. For power save feature, fix a bug in "minimum sleep duration limitation".
8. Delay SW1 and RET_OFF time to avoid HW RESET issue on warm-boot.
9. Solve auto connect & retry mechanism conflict problem.
10. Update "at+rfhp=" command to solve RF HP setting not working problem. 



## Notes List 
1. Websocket feature is on-developing 
2. More examples such as MQTT, power save are on-developing. 

*****

## Release Version:
SDK Package: 1.1.1.10
Patch_Lib : 2517
ROM_CODE: 1655 

## Release Date: 2018/9/25

## Function & Feature Update 
1. Documents updating:
 1.1 OPL1000-AT-instruction-set-and-examples: R05-V23,  add several new AT instructions description.    
 1.2 OPL1000-WIFI-BLE-API-guide: v1.1.1.10, wifi part API is updated.   
2. Tool version information and update:
 2.1 download tool: v0.23. No change since v1.1.1.5;        
 2.2 pin-mux tool: v0.8.No change since v1.1.1.7;   
3. spi_master example under peripherals folder is updated.   
4. Add WIFI data rate AT command  AT+MACDATARATE.
5. Add http post blocking API.   
6. Close M0 print log in order to improve system performance.
7. Update power saving RF module setting. 
8. Refine http client API request realization in lwip module.
9. Refine stack size and task priority for running "AT+CIPSEND".
10. Implement new RF setting for WIFI CH11/12 configuration.


## Bug Fix List 
1. Fix a bug in "security API" by temporarily disable dynamic switch of OTP/Security IP clock. 
2. Fix a bug in "LE 2M PHY configuration" implementation. 
3. Fix a bug in "HCI_Reset command" implementation.
4. Fix a bug in WIFI-releasred example realization, change "opl_event_loop" task local variable allocation method.  
5. Fix  "IO wake-up not working" issue 


## Notes List 
1. Websocket feature is on-developing 
2. More examples such as MQTT, power save are on-developing. 

*****

## Release Version:
SDK Package: 1.1.1.7
Patch_Lib : 2475
ROM_CODE: 1655 

## Release Date: 2018/9/14

## Function & Feature Update 
1. Documents updating:
 1.1 OPL1000-AT-instruction-set-and-examples: R05-V17,  add several new AT instruction description.    
 1.2 OPL1000-pinmux-tool-user-guide: R02-V05, add generated include file introduction, updated according to v0.8 SW.   
2. Tool version information and update:
 2.1 download tool: v0.23. No change since v1.1.1.5.        
 2.2 pin-mux tool: v0.8. Add include file generation function.   
3. All examples are updated because pinmux setting method is changed.   
4. Firmware opl1000_at.bin is added under FW_Binary folder. This patch bin file is used to evaluate AT command. 
5. RF HPA (high power amplifier) setting is updated. AT+RFHP instrument is supported in this release.   
6. Refine stack size and priority of tasks.
7. Add AT Uart and DbgUart switch function for IO0 & IO1. 
    When  IO0/IO1 is AT uart, type AT command "at+switchdbg" will switch it to DbgUart.
    When IO0/IO1 is Dbg uart, type CLI command  "switchat" will switch it to AT Uart.
8. Support both new-line and none new-line mode in AT_CMD.

## Bug Fix List 
1. Fix a bug in "at+counters?" instruction.  
2. Solve "hidden AP connection fail" problem. 
3. Fix a bug in sleep duration calculation.
4. Update valid range for AT+WRITEFLASH and AT+ERASEFLASH. 


## Notes List 
1. TCP/IP AT instructions may make stack overflow, this issue will be fixed in next release. 
2. Websocket feature is on-developing 
3. More examples such as MQTT, power save are on-developing. 

*****

## Release Version:
SDK Package: 1.1.1.6
Patch_Lib : 2430
ROM_CODE: 1655 

## Release Date: 2018/9/7

## Function & Feature Update 
1. Documents updating:
    1.1 OPL1000-AT-instruction-set-and-examples: R05-V15,  Add several new AT instruction description.    
    1.2 OPL1000-pinmux-tool-user-guide: R02-V04, updated according to v0.7 SW. GPIO setting is changed.  
2. Tool version information and update:
    2.1 download tool: v0.23. No change since v1.1.1.5.        
    2.2 pin-mux tool: v0.7. GPIO setting is changed. Fixed one bug in SPI quad mode setting.   
3. Switch off unused SRAM in deep sleep mode to improve power save performance.  
4. Support multiple external input wake-up mechanism. 
5. Add mbedTLS library to support TLS feature.   
6. Reduce AT patch code to enlarge ram size, provide more space for user application.       
7. Update driver to use correct deep sleep behavior.
8. Refine stack size and priority of tasks. 
9. Set WiFi/BLE power level separately and HIGH FI/FC in Main_PowerSetup to improve RF power consumption. 
10. Replace SW crypto module with hardware-accelerated function for mbedTLS module.
11. Implement SHA-related API with HW security engine

## Bug Fix List 
1. Fix bugs in I2C master initialization and receive data operation.    
2. Fix a bug in SSID scan list store implementation. 
3. Fix a bug in smart sleep when it is configured at SysInit.
4. Fix a bug in wifi mode setting when not initial wifi module. 


## Notes List 
1. TCP/IP AT instructions has issue in this release and will be fixed in next version.  
2. websocket feature is on-developing 
3. More examples such as MQTT, power save are on-developing. 

*****

## Release Version:
SDK Package: 1.1.1.5
Patch_Lib : 2382
ROM_CODE: 1655 

## Release Date: 2018/8/30

## Function & Feature Update 
1. Documents updating:
    1.1 OPL1000-Flash-User-Guide: R01-V02,  Flash layout is updated.  
2. I2C low driver is updated.     
3. AT instruction ending is changed from "\r" to "\r\n" 
4. Tool version information and update:
    4.1 download tool: v0.23. AT instruction calling format is changed. Add minimize processing on UI window.        
    4.2 pin-mux tool: v0.6. No change since v1.1.1.1;    
5. RF mode setting is updated.  
6. Start address of BLE host data and tcpip data in Flash are adjusted. Refer OPL1000-Flash-User-Guide document.         
7. Improve certain AP connection success rate, such as ASUS AP.
8. Support IPC command to control WIFI data rate. 
9. Implement SHA-1 API with HW security engine.
10. Support DHCP APR check mechanism in AT+DHCPARPCHK command.

## Bug Fix List 
1. Fix a bug in I2C driver.    
2. Fix a bug in ISR UART RX buffer process of AT and CLI command.
3. Fix a bug in GPIO driver, original driver does not handle race condition case.  


## Notes List 
1. websocket feature is on-developing 
2. More examples such as MQTT, power save are on-developing. 

*****

## Release Version:
SDK Package: 1.1.1.4
Patch_Lib : 2350
ROM_CODE: 1655 

## Release Date: 2018/8/24

## Function & Feature Update 
1. Documents updating:
    1.1 OPL1000-Power-Saving-Introduction: R01-V02,  document format is  structured.  
    1.2 Add a new document OPL1000-Demo-ota-wifi-guide R01-V01 under Demo\OTA_WIFI folder, this document introduce OTA WIFI implementation method and verification flow.   
    1.3 Add a new document OPL1000-Power-Consumption-Measurement-Guide R01-V01, which introduce how to measure power consumption for OPL1000 different power save mode. 
2. A new demo "OTA_WIFI"  is added to Demo\OTA_WIFI. This demo shows how to setup http server and complete OTA function by WIFI.     
3. OTA_WIFI example is updated to fit new OTA firmware image file format. 
4. Tool version information and update:
    4.1 download tool: v0.21. No change since v1.1.1.3;       
    4.2 pin-mux tool: v0.6. No change since v1.1.1.1;    
5. Optimize current for APS/MSQ peripheral blocks clock control.  
6. Switch off unused SRAM in case of deep sleep to reduce current consumption.       
7. Improve WIFI AP connection success rate for certain brand AP, such as CISCO AP.
8. Use the average method to get the ADC value in order to improve AUXADC precision.

## Bug Fix List 
1. Fix a bug of deep sleep abnormal behavior.    
2. Set default IP address ad 0.0.0.0 for ST1 interface instead of 192.168.1.101.
3. Fix some compiling warnings for power save module.  


## Notes List 
1. websocket feature is on-developing 
2. More examples such as MQTT, AT commands are on-developing. 

*****

## Release Version:
SDK Package: 1.1.1.3
Patch_Lib : 2314
ROM_CODE: 1655 

## Release Date: 2018/8/17

## Function & Feature Update 
1. Documents updating:
    1.1 OPL1000-WIFI-BLE-API-guide: v1.1.1.3,  updated document according to BLE and WIFI part APIs.  
    1.2 OPL1000-DEVKIT-getting-start-guide: R02-V07, Update for A1 Devkit board, add OTA image pack introduction.   
    1.3 OPL1000-SDK-getting-start-guide:  R02-V06, add introduction of how to build OTA image file .
    1.4 OPL1000-SDK-Development-guide: R01-V08, add introduction to two new adding examples, OTA_wifi and https_request. 
    1.5 Add a new document OPL1000-PowerSave  V01, which introduce OPL1000 power save mode and setting method . 
2. A new example https_request is added. This example shows how to send request to https server and get response based on mbedtls module.     
3. Add and enable watchdog mechanism
4. Tool version information and update:
    4.1 download tool: v0.21. Fix a bug in OTA header define; Remove "scan system process" feature to avoid some anti-virus software recognize download tool as virus.       
    4.2 pin-mux tool: v0.6. No change since v1.1.1.1;    
5. GPIO example is updated, add IO20 support.  
6. Power saving APIs are re-defined.       
7. Improve channel selection algorithm.
8. Add new APIs to support connect to certain AP from auto connect list.

## Bug Fix List 
1. Fix a bug in deep sleep function.  
2. Fix bug in GPIO example when program IO20 as output pin.
3. Fix a bug in LE decide next adv time function. 
4. Fix a bug of M0 hang up when do BLE advertising and WIFI connection simultaneously
5. Fix a bug in AT+BLEADDR implementation 
6. Enlarge MSQ state3 to state4 timeout time in AT+CWJAP implementation. 
7. Fix a bug in "at+cipserver?" instruction  

## Notes List 
1. websocket feature is on-developing 
2. More examples such as MQTT, AT commands are on-developing. 

*****

## Release Version:
SDK Package: 1.1.1.2
Patch_Lib : 2273
ROM_CODE: 1655 

## Release Date: 2018/8/7

## Function & Feature Update 
1. Documents updating:
    1.1 OPL1000-WIFI-BLE-API-guide: v1.1.1.2,  updated document according to BLE and WIFI part APIs.  
    1.2 OPL1000-patch-download-tool-user-guide: R03-V05, add OTA image pack introduction, update download procedure according.   
    1.3 OPL1000-AT-instruction-set-and-examples: R05-V12, add more descrption for AT+CWMODE, AT+CWLAP and AT+WRITEFLASH commands.
    1.4 OPL1000-OTA-User-Guide: R01-V04, update OTA image pack operation figure according to v0.20 download tool. 
    1.5 Add a new document OPL1000-IOT-Application-Framework-User-Guide R01-V01, which introduce BLE configure WIFI AP working principle. 
2. A new example ota (via WIFI) is added. This example shows how to implement OTA over WIFI connection.     
3. DEVKIT USB to UART is connected to IO0/IO1 instead of IO8/IO9. 
4. Tool version information and update:
    4.1 download tool: v0.20. Add OTA image pack function. Unify download method to pure patch bin file and OTA image file.      
    4.2 pin-mux tool: v0.6. No change since v1.1.1.1    
5. AT command is mapping to IO0/IO1, not IO8/IO9. IO8 and IO9 is used for debug UART log information output.  
6. Improve WIFI TX path throughput, waiting time is reduced to 1ms from 2000ms when TX queue is full.     
7. Remove the limitation of change WIFI mac address before disconnect.
8. Change hardware sleep related configuration to improve power saving performance. 

## Bug Fix List 
1. Fix bugs in AT Command " AT+CWLAP", "AT+CWAUTOCONN",  "AT+CWJAP?" and "AT+CWAUTOCONN?" , 
2. Fix bug of "getting auto connect AP info incorrect" issue
3. Fix bug of "the UART buffer of AT Cmd is not initialized"
4. Fix bug of "update connected AP info failed" issue 
5. Fix AT task produce memory leak problem

## Notes List 
1. Completed power saving functions are on-developing 
2. More examples such as MQTT, AT commands are on-developing. 

*****

## Release Version:
SDK Package: 1.1.1.1
Patch_Lib : 2227
ROM_CODE: 1655

## Release Date: 2018/8/3

## Function & Feature Update 
1. Documents updating:
    1.1 OPL1000-WIFI-BLE-API-guide: v1.1.1.1, Both BLE and WIFI part APIs are updated.  
    1.2 OPL1000-system-initialization-brief-introduction: R01-V03, add cold boot and warm boot time state information.    
    1.3 OPL1000-AT-instruction-set-and-examples: R05-V11, add new AT command for flash read, write and erase.
    1.4 OPL1000-pinmux-tool-user-guide: R01-V03,  A1 chip opens two more IO pin resource , PinMux tool is updated. 
    1.5 OPL1000-DEVKIT-getting-start-guide: R02-V06, updated for A1 chip.
    1.6 OPL1000-SDK-getting-start-guide: R02-V05, update section 3 because A1 chip SDK project setting is changed.
    1.7 OPL1000-SDK-Development-guide: R01-V07, release package folder structure and example project building description are updated.
2. All examples project setting is updated from A0 configuration to A1 configuration.    
3. "log output" method is updated in several example projects. 
4. Tool version information and update:
    4.1 download tool: v0.19. Support A1 chip.     
    4.2 pin-mux tool: v0.6. Support A1 chip, add IO16,IO17 additional PIN resource    
5. Add several new AT commands：AT+READFLASH, AT+WRITEFLASH and AT+ERASEFLASH etc. 
6. Support RF high/low power configuration   
7. Add a new feature of "DCOC dump".  Use at+showdc and at+dcoc command to dump DC and calibration data.

## Bug Fix List 
1. Fix bugs in AT Command "AT+BLECONNPARAM", "AT+CWAUTOCONN",  "AT+CWJAP?" and "AT+CWAUTOCONN?" , 
2. Fix a bug of "task not works after warm-boot"
3. Fix a bug of "Inconsistent value when read/write AP numbers to FIM"
4. Fix a bug of "get IP failed when mac address is OTP default"

## Notes List 
1. WIFI OTA example is on-developing.  
2. More examples such as MQTT, power saving are on-developing. 

*****

## Release Version:
SDK Package: 1.0.1.28
Patch_Lib : 2109
ROM_CODE: 809 

## Release Date: 2018/7/27

## Function & Feature Update 
1. Documents updating:
    1.1 OPL1000-WIFI-BLE-API-guide: v1.0.1.28, WIFI part APIs are updated.  
    1.2 OPL1000-RF-Testing-Guide: R01-V02, several captured figures are updated.    
    1.3 OPL1000-AT-instruction-set-and-examples: R05-V10, add more description for AT+BLEADDR and AT+CIPSTAMAC.
    1.4 OPL1000-Demo-tcp-client-guide: R01-V02, add TCP server send ACK message introduction. 
    1.5 OPL1000-OTA-User-Guide: R01-V03, fix several misdescription 
2. Update TCP_Client demo, add TCP client receive TCP server ACK message processing.    
3. Tool version information:
    3.1 download tool: v0.18. No change since v1.0.1.26.     
    3.2 pin-mux tool: v0.5. No change since v1.0.1.19    
4. Add several new AT commands：AT+BLEADDR, AT+MACADDRDEF, AT+RFHP etc. 
5. Update scan configuration API, support specific channel or all channels scan, scan duration time for different scan mode.   
6. Add readme.md for each example project, briefly introduce example function, work flow, application notice etc.  
7. Support LE multi-connection up to 8 devices. 

## Bug Fix List 
1. Fix bugs in AT Command "AT+CIPDINFO", "AT+CIPSTATUS",  "at+tx" and "AT+CWAUTOCONN?" , 
2. Fix the patch method of partition memory pool
3. Fix a bug in "tracer_cli" function in "AT" and "Diag" tasks
4. Fix a bug of wrong return value of AES-CCM and AES-ECB functions

## Notes List 
1. WIFI OTA function is on-developing.  
2. More examples such as MQTT, power saving are on-developing. 

*****

## Release Version:
SDK Package: 1.0.1.27
Patch_Lib : 2031
ROM_CODE: 809 

## Release Date: 2018/7/20

## Function & Feature Update 
1. Documents updating: 
    1.1 OPL1000-WIFI-BLE-API-guide: v1.0.1.27, WIFI part several APIs are updated.  
    1.2 Add a new document "OPL1000-RF-Testing-Guide" which introduce how to do RF testing on OPL1000 DEVKIT board.   
    1.3 Add a new document "OPL1000-BLEWIFI-Application-Dev-Guide" to introduce BLE configure WIFI network working flow and principle.
    1.4 A new document "OPL1000-OTA-User-Guide" is added under "Demo\OTA" folder which introduce OPL1000 BLE OTA function.
2. Add a new demo to introduce how to complete OTA through BLE. This demo is placed under "Demo\OTA" folder.  
3. Tool version information:
    3.1 download tool: v0.18. No change since v1.0.1.26.     
    3.2 pin-mux tool: v0.5. No change since v1.0.1.19    
4. Improve stability for reduplicative AP connect/disconnect 
5. Add new API function to query Wi-Fi status and clean Wi-Fi AP record



## Bug Fix List 
1. Fix bug in AT Command "AT+BLENAME", "AT+BLEDATALEN", "AT+BLECONNPARAM" and "AT+CWAUTOCONN?" , 
2. Fix "MAC stuck" bug in connect/disconnect AP when link quality is poor 

## Notes List 
1. WIFI OTA function is on-developing.  
2. More examples such as MQTT, power saving are on-developing. 

*****

## Release Version:
SDK Package: 1.0.1.26
Patch_Lib : 1978
ROM_CODE: 809 

## Release Date: 2018/7/13

## Function & Feature Update 
1. Documents updating: 
    1.1 OPL1000-WIFI-BLE-API-guide: v1.0.1.26, WIFI part several APIs are updated.  
    1.2 OPL1000-SDK-Development-guide: R01-V06, SDK package folder and exmaples are updated.  
    1.3 OPL1000-DEVKIT-getting-start-guide: R02-V05, how to download OTA loader bin file and OTA image files are introduced.
    1.4 OPL1000-patch-download-tool-user-guide: R03-V04. OTA loader and image file download function is introduced according to v0.18 version program. 
2. Boot agent (OTA loader) project is added in this release.    
3. Tool version information:
    3.1 download tool: v0.18. OTA loader and image file download function are added.     
    3.2 pin-mux tool: v0.5. No change since v1.0.1.19    
4. OTA through BLE function is working, demonstration  program will be provided soon.
5. Fixed several bugs in AT instructions.    
6. update calibration data for temperature sensor


## Bug Fix List 
1. Fix bug in "AT+CWJAP " instruction implementation. 
2. Fix issue of OPL1000 SSID connect to hidden AP fail issue 
3. Fix a bug in RF Channel 14 setting process. 
4. Update lwip netif module, fix "IP address obtain failed" issue under certain conditions. 

## Notes List 
1. OTA example project is on-developing.  
2. More examples such as MQTT, power saving are on-developing. 

*****

## Release Version:
SDK Package: 1.0.1.25
Patch_Lib : 1930
ROM_CODE: 809    

## Release Date:  2018/07/06

## Function & Feature Update
1. Documents updating: 
    1.1 OPL1000-WIFI-BLE-API-guide: v1.0.1.25, WIFI part APIs are updated.  
    1.2 OPL1000-AT-instruction-set-and-examples: R05-V07, refined some description of several AT commands.  
2. WIFI related examples are updated. AP scan mode is changed from active to mixed mode.   
3. Tool version information:
    3.1 download tool: v0.17. No change since v1.0.1.23.    
    3.2 pin-mux tool: v0.5. No change since v1.0.1.19    
4. Low level FW lib supports BLE OTA feature, high level application module is on-developing.
5. Fixed several bugs in AT instructions.    
6. BLE connection rate and stability is improved. 
7. WIFI Interoperability for fast connection is improved.  
8. Implement AES-CMAC API with HW security engine 

## Bug Fix List 
1. Fix bug in "AT+CIPSENDEX" instruction implementation. 
2. Fix bug in "AT+UART_CUR?" and "AT+UART_DEF?" instruction display 
3. Fix baud-rate and stop bit setting bug in "AT+UART_CUR" instruction
4. Fix bug in "AT+CIPSTART" instruction UDP mode
5. Fix bug in "AT+CWSTAMAC" instruction
6. Fix bug in "AT+CWAUTOCONN?" instruction print information 
7. Fix bug in "AT+BLEADVPARAM", "AT+CWLAPOPT" and "AT+CWHOSTNAME" instructions 
8. Fix bug in query Wi-Fi Status and clean Wi-Fi AP record operations 

## Notes List 
1. OTA high level application module is on-developing.  
2. More examples such as MQTT, power saving are on-developing. 

*****
## Release Version:
SDK Package: 1.0.1.24
Patch_Lib : 1849
ROM_CODE: 809    

## Release Date:  2018/06/28

## Function & Feature Update
1. Documents updating: 
    1.1 OPL1000-WIFI-BLE-API-guide: v1.0.1.24, WIFI part APIs are updated.  
    1.2 OPL1000-AT-instruction-set-and-examples: R05-V06, fixed wrong description in several AT commands.  
2. Example codes updating:
    2.1 sntp example is updated, wifi ap SSID is changed to Opulinks-TEST-AP.    
    2.2 add a new protocol example: mDNS, service broadcast function is supported.   
3. Tool version information:
    3.1 download tool: v0.17. No change since v1.0.1.23.    
    3.2 pin-mux tool: v0.5. No change since v1.0.1.19    
4. Fixed bugs in several AT commands  
5. Optimize IPC module realization    
6. Solve "printf instruction may cause caller task stack overflow" issue
7. Extend WIFI scan channel list range from 1~11 to 1~13 
8. Extend Adaptive DTIM interval settings to maximum 255 

## Bug Fix List 
1. Solve OPL1000 connecting SSID hidden AP failure issue. 
2. Fix "no message of success or failure" bug in AT command AT+BLECONNPARAM
3. Fix bugs in AT commands AT+CIPCLOSE, AT+BLEADVPARAM, AT+BLENAME, AT+BLECONNPARAM, AT+BLENAME
4. Fix the bug of "auto connect failed when AP's mode is changed from security to open"

## Notes List 
1. OTA, open SSL features are on-developing.  
2. mDNS service inquiring function is on-developing. 
3. More examples such as MQTT, power saving development are on-going. 

*****
## Release Version:
SDK Package: 1.0.1.23
Patch_Lib : 1807 
ROM_CODE: 809    

## Release Date:  2018/06/22

## Function & Feature Update
1. Documents updating: 
    1.1 OPL1000-WIFI-BLE-API-guide: v1.0.1.23, WIFI part APIs are updated.  
    1.2 OPL1000-AT-instruction-set-and-examples: R05-V05, several AT commands are updated.  
2. Example codes updating:
    2.1 hello_world example, enable debug UART input and output operation.    
3. Tool version information:
    3.1 download tool: v0.17. Add patch lib version reading function.    
    3.2 pin-mux tool: v0.5. No change since v1.0.1.19    
4. Support patch lib version reading through APS UART port by CLI command "at+gmr"
5. SPI0 Flash access throughput is improved 3 times for both Standard and Quad mode.   
6. Provide "Adaptive DTIM" API calling interface 

## Bug Fix List 
1. fixed several bugs in AT command implementation, include AT+SLEEP,AT+WIFIMACCFG,AT+CWLAPOPT,AT+CIPSTATUS etc. 
2. fix a bug in auto connect mode.

## Notes List 
1. v1.0.1.22 version is skipped. 
2. More examples such as mDNS, MQTT, power saving development are on-going. 

*****

## Release Version:
SDK Package: 1.0.1.21
Patch_Lib : 1751 
ROM_CODE: 809    

## Release Date:  2018/06/15

## Function & Feature Update
1. Documents updating: 
    1.1 OPL1000-WIFI-BLE-API-guide: v1.0.1.21, two WIFI APIs are updated.  
2. Add 3 new documents: 
    2.1 OPL1000-WiFi-Interoperability-Results: OPL1000 WIFI Interoperability test result summary   
    2.2 OPL1000-Flash-User-Guide: OPL1000 Flash introduction and use guide 
    2.3 OPL1000-Adaptive-DTIM-Solution-Guide: OPL1000 adaptive DTIM introduction and use guide    
3. Example codes updating:
    3.1 Enable internal module log output for several examples, such as wpa2_station, http_request etc. This helps user to know more detailed information.    
    3.2 Add a new protocol example SNTP which show how to implement SNTP function by socket communication method. 
4. Tool version information:
    4.1 download tool: v0.16. No change since v1.0.1.19     
    4.2 pin-mux tool: v0.5. No change since v1.0.1.19    
5. Support RF power-off function for power saving feature

## Bug Fix List 
1. fixed several bugs in AT command implementation, include AT+CWLAP,AT+BLEGATTSSETATTR and AT+CWJAP. 
2. fix a bug in RSSI report.

## Notes List 
1. More protocol examples such as mDNS, MQTT development are on-going. 

*****

## Release Version:
SDK Package: 1.0.1.20
Patch_Lib : 1731 
ROM_CODE: 809    

## Release Date:  2018/06/08

## Function & Feature Update
1. Documents updating: 
    1.1 OPL1000-WIFI-BLE-API-guide: v1.0.1.20, several WIFI APIs are updated.  
    1.2 OPL1000-DEVKIT-getting-start-guide: R02_V04, add DEVKIT USB driver installation on WinXP/Win10 introduction.   
    1.3 OPL1000-SDK-getting-start-guide: R02_V04, update project setting related description. 
    1.4 OPL1000-SDK-Development-guide: R01_V05, update release package file list and project setting related description .
    1.5 Add a new document OPL1000-system-initialization-brief-introduction.pdf to introduce OPL1000 boot procedure.   
2. Example codes updating:
    2.1 Update SPI_master example, add SPI0 access on-board flash code, folder: SDK\APS_PATCH\examples\peripherals\spi_master   
    2.2 Update all example project setting, unify chip name to OPL1000
3. Tool version information:
    3.1 download tool: v0.16. No change since v1.0.1.19     
    3.2 pin-mux tool: v0.5. No change since v1.0.1.19    
4. DEVKIT USB driver (CP210x USB to UART bridge) is updated. Win10/WinXP drivers are provided. Refer OPL1000-DEVKIT-getting-start-guide section 3.1 to know how to install driver.     
5. Add README.md to introduce release package content briefly. 
6. WIFI AP connection/ping operation compatibility is improved. 

## Bug Fix List 
1. fix timer print log operation cause stack overflow issue in blink example code. 
2. fix download tool AT UART port on Win10 system not working issue. User needs to update CP210x driver.

## Notes List 
1. More protocol examples such as mDNS, SNTP development are on-going. 

*****

## Release Version:
SDK Package: 1.0.1.19
Patch_Lib : 1679 
ROM_CODE: 809    

## Release Date:  2018/06/01

## Function & Feature Update
1. Documents updating: 
    1.1 OPL1000-AT-instruction-set-and-examples: R04, add new AT command and fix some description errors. 
    1.2 OPL1000-DEVKIT-getting-start-guide: R02_V03, AT and APS UART port usage are updated.   
    1.3 OPL1000-SDK-getting-start-guide: R02_V03, firmware upgrade introduction is updated. 
    1.4 OPL1000-SDK-Development-guide: R02_V04, update section 2 SDK content introduction.
    1.5 OPL1000-patch-download-tool-user-guide: R03, updated according to v0.16 version SW.
    1.6 OPL1000-pinmux-tool-user-guide: R02, updated according to v0.5 version SW.  
    1.7 Add a new document "OPL1000_peripheral_PWM_application_notes", introduces how to use PWM module.   
2. Example codes updating:
    2.1 Add a new example TCP client, folder: SDK\APS_PATCH\examples\protocols\tcp_client 
    2.2 Update SPI_master example, folder: SDK\APS_PATCH\examples\peripherals\spi_master  
    2.3 Add a new demo folder "Demo\TCP_Client", introduce how to verify TCP client function.   
3. Tool version information:
    3.1 download tool: v0.16, mini-USB is mapped to APS UART and can be used for "on-line FW upgrade"    
    3.2 pin-mux tool: v0.5. pwm and SPI setting are changed.    
4. WIFI AP connection stability is improved.     
5. Update and add several AT commands    
6. Add a new demo of TCP_Client, corresponding folder: Demo\TCP_Client

## Bug Fix List 
1. fix bug in AT+CIPSTAMAC
2. fix SPI setting bug in pinmux tool
3. fix "wpa2_station wifi connection dropped" issue   

## Notes List 
1. More protocol examples such as mDNS, SNTP development is on-going. 

*****

## Release Version:
SDK Package: 1.0.1.18
Patch_Lib : 1622 
ROM_CODE: 809    

## Release Date:  2018/05/25

## Function & Feature Update
1. Below documents are updated: 
    1.1 OPL1000-AT-instruction-set-and-examples: R03, correct some description errors. 
    1.2 OPL1000-WIFI-BLE-API-guide: v1.0.1.18, updated several APIs' parameters description.   
2. Add three new example codes:
    2.1 I2C slave example. User need to connect an external I2C Master device to communicate with OPL1000 DEVKIT 
    2.2 Log on/off setting example. This example shows how to enable/disable ROM internal module and user application log information   
    2.3 http request example, provides two solution. One is based on CMSIS RTOS, the other is based on FreeRTOS.   
3. Update peripheral PWM example. Provide a complex setting mode to reach higher precision duty rate. 
4. Improved stability on AP connection    
5. http request example location: SDK\APS_PATCH\examples\protocols   
6. Log on/off setting example location: SDK\APS_PATCH\examples\get_started\log
7. I2C slave example location: SDK\APS_PATCH\examples\peripherals\i2c_slave

## Bug Fix List 
1. Fix a bug in auto-connection operation
2. Fix several bugs in BLE AT commands 

## Notes List 
1. Protocol examples such as https, tcp client/server etc. are on-developing 

*****

## Release Version:
SDK Package: 1.0.1.17 
Patch_Lib : 1516 
ROM_CODE: 809    

## Release Date:  2018/05/18

## Function & Feature Update
1. Below documents are updated: 
    1.1 OPL1000-AT-instruction-set-example: R02, add AT instruction example 
    1.2 OPL1000-DEVKIT-getting-start-guide: R02, add J-link emulator selection etc. 
    1.3 OPL1000-patch-download-tool-user-guide: R02, update according to v0.15 SW 
    1.4 OPL1000-SDK-Development-guide: R02, add log output setting section 
    1.5 OPL1000-SDK-getting-start-guide: R02, update download tool operation figures. 
    1.6 OPL1000-WIFI-BLE-API-guide: v1.0.1.17, several APIs' parameter type are changed.   
2. Firmware download tool is updated to v0.15, provide log message saving function. 
3. Support AT UART download patch bin file function. (need M3 bin file support AT UART download instruction)    
4. Add a demonstration folder "Demo\BLE_Config_AP" which show how to use OPL1000 BLE setup WIFI network 
5. Add API that support to turn ON/OFF show-Log on Debug-UART port 
6. Update formula of RSSI estimation

## Bug Fix List 


## Notes List 
1. Protocol examples are on-developing 

*****

## Release Version:
SDK Package: 1.0.1.16 
Patch_Lib : 1488 
ROM_CODE: 809    

## Release Date:  2018/05/11

## Function & Feature Update
1. WIFI: 802.11b station 
2. BLE: BLE 4.2 slave 
3. Firmware download supports from both APS and AT UART port  
4. Example codes include: peripheral, WIFI, BLE, BLEWIFI.  
5. Provide 6 documents include DEVKIT and SDK getting start, SDK/App development guide, AT command and API introduction, tool use guide etc. 
6. Provide 2 auxiliary tools, firmware download tool and pin-mux setting tool. 
7. Provide 2 UART driver: one is DEVKIT UART driver, the other is CH340 UART winXP driver which is used for APS UART port.  

## Bug Fix List 


## Notes List 
1. Trace log has disorder phenomenon
2. Protocol examples are on-developing 
3. System examples are on-developing 


