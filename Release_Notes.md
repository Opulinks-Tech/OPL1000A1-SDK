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


