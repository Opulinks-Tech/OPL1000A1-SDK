# BLE_Config_AP
This demo shows how to use (Android) mobile APP to control OPL1000 completes below functions 

1. Use BLE to configure wireless AP. 
2. Complete AP connection. 
3. Update firmware through BLE connection (BLE OTA). 

Note: 

(1) Refer "OPL1000-Demo-BLE-setup-network-guide.pdf" to know how to execute this demo 

(2) To evaluate BLE OTA function mobile app needs to store an OTA image file in advance     

# Iperf
This demo shows how to use opl100 internal iperf  function to evaluate WIFI communication bandwidth.

This demo supports TCP/UDP transfer while OPL1000 acting as server or client.    

Please refer OPL1000-Iperf-Measurement-Guide.pdf to know how to setup evaluation environment and execute demo. 

# OTA_WIFI 
This demo shows how to setup a TCP server on PC and complete OTA through WIFI path.  Notes:

1. AP ssid, password and TCP server IP, port etc. parameters need to be defined in "ota_wifi" example source code
2. After "ota_wifi" example is compiled, M3 bin file needs to be packed with M0 binn file and formed as OTA format image file. 

 Please refer "OPL1000-Demo-ota-wifi-guide.pdf" to know detailed information of how to build image file and execute demo. 

# TCP_Client 
This demo shows OPL1000 establish data communication with a TCP server while enable smart sleep work mode. By this demonstration user can

1. Evaluate OPL1000 power consumption while keep TCP data exchanging 
2. Know how to establish TCP communications between OPL1000 and TCP server 

Note: AP ssid, password and TCP server IP, port etc. parameters need to be defined in "ota_wifi" example source code

Please refer "OPL1000-Demo-tcp-client-guide.pdf" to know detailed information of how to build image file and execute demo. 


