#include <string.h>
#include "mqtt_api.h"
//#include "dns.h"
//#include "transport.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cmsis_os.h"
#include "event_loop.h"
#include "wifi_api.h"
#include "wifi_event.h"
#include "wifi_event_handler.h"
#include "lwip_helper.h"
#include "tcp_client.h"
#include "mqtt_client.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/netif.h"
#include "errno.h"

//#include "SysGlobal.h"

//#define     MQTT_DBG(...)        printf(...)

int  MQTT_Socket = -1;
int   Socket_Local;
uint16_t Socket_LocalPort;
uint8_t  Socket_SserverIP[4];
uint16_t Socket_ServerPort;

struct sockaddr_in MQTT_ServerAdd;  
char MQTT_ServerIP[32];
int MQTT_ServerPort = TCP_SERVER_PORT; 
    
    
    
uint8_t  MQTT_SocketStatus = 0;
uint8_t  miss_ping_ack_count = 0;
    
    


int MQTT_Begin(int p_s,uint16_t p_port)
{
    uint8_t retryNum = 0;

    MQTT_ServerAdd.sin_family = AF_INET; 
    MQTT_ServerAdd.sin_addr.s_addr = inet_addr(TCP_SERVER_ADDR);  
    MQTT_ServerAdd.sin_port = htons(TCP_SERVER_PORT); 

    strcpy(MQTT_ServerIP, TCP_SERVER_ADDR);

//    if (g_connection_flag == true) 
//      printf("Opulinks-TEST-AP connected \r\n");

    printf("Connect %s at port %d \r\n", MQTT_ServerIP, MQTT_ServerPort); 

    while(1) 
    {
        p_s = socket(AF_INET, SOCK_STREAM, 0);
        if (p_s < 0) 
        {
            printf("... Failed to allocate socket. \r\n");
            osDelay(1000);
            retryNum++;
            if(retryNum > 100)
            {
                return -1;
            }
            continue;
        }
        else
        {
            return 1;
        }
    }
}


/*
@return  1:ok;
        -1:tcp connect err,check socket,server_ip,server_port;
        -2:mqtt server no ack or ack err;
*/
int  MQTT_Connect(void)
{
    uint8_t  buf[128] = {0};
    int32_t len=0;
    int rc = 0;
    int tmp;
    //uint8_t  t = 30;
		uint8_t  t = 5; //20190628-Update.
    uint8_t retryNum = 0;
    uint32_t tmp_time;
    int buflen = sizeof(buf);

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

    
    MQTT_ServerAdd.sin_family = AF_INET; 
    MQTT_ServerAdd.sin_addr.s_addr = inet_addr(TCP_SERVER_ADDR);  
    MQTT_ServerAdd.sin_port = htons(TCP_SERVER_PORT); 
    
    printf("Connect %s at port %d \r\n", MQTT_ServerIP, MQTT_ServerPort); 
    printf("MQTT_ServerAdd.sin_family = %d\n", MQTT_ServerAdd.sin_family);
    printf("MQTT_ServerAdd.sin_addr = %08X\n", MQTT_ServerAdd.sin_addr.s_addr);
    printf("MQTT_ServerAdd.sin_port = %02X\n", MQTT_ServerAdd.sin_port);
    
    while(1) 
    {
        MQTT_Socket = socket(AF_INET, SOCK_STREAM, 0);
        if (MQTT_Socket < 0) 
        {
            printf("... Failed to allocate socket. \r\n");
            retryNum++;
					  osDelay(1000);
            if(retryNum > 10)
            {
                return -1;
            }
            continue;
        }
        else
        {
            break;
        }
    }
    
    while(t)
    {
	    t--;
		rc = connect(MQTT_Socket, (struct sockaddr *)&MQTT_ServerAdd, sizeof(MQTT_ServerAdd));
        if( rc != 0) 
        {
            printf("... socket connect failed errno=%d \r\n", rc);
            //close(MQTT_Socket);20190628-Update.
            osDelay((5 - t) * 2000);
            continue;
        }
        else
        {
            MQTT_SocketStatus = 1;
					  miss_ping_ack_count = 0; //initialize it to 0 when connection setting up.
            rc = 1;
            break;
        }
        
    }
		
    if(rc == 1)
    {
        printf("tcp connect server is ok !\r\n");
    }
    else
    {
				// update status and close socket failed to be connected. 20190628-Update.
				MQTT_SocketStatus = 0;
				close(MQTT_Socket);    
        printf("tcp connect server is failed:%d\r\n",rc);
        return -1;
    }

    /*make link package*/
//  data.clientID.cstring = "me";
//	data.cleansession = 1;
//	data.keepAliveInterval = 5;
//    if(anonymous == 0)
//    {//如果不使用匿名登录，设置登录名称和密码
//        data.username.cstring = user_name;
//        data.password.cstring = password;
//    }
    
    data.clientID.cstring = MQTT_SUB_TOPIC;//"SN888880";//Device_SN;//"SN888888"
    data.keepAliveInterval = 120;
    data.cleansession = 1;
    data.MQTTVersion = 3;
//    data.username.cstring = Device_SN;//"SN888888";
//	data.password.cstring = connect_pwd_buf;//"1481619167365_704889539_939f3a5c71e63a33536f54d605c94beb";
	data.will.qos=1;//遗嘱
	data.will.message.cstring = "ReportNetworkStatus V1.0 \n{\"online\":false}";
	data.will.topicName.cstring = MQTT_SUB_TOPIC;//= "SN888880"; //MQTT_PublishTopic;//"VirtualTopic/dmf/SN888888";
    data.willFlag = 1;
    
    len = MQTTSerialize_connect(buf, buflen, &data); /* 1 */
    /*send package*/
    tcp_write_data(buf,len);

    
    /* wait for connack */
    tmp_time = 0;//millis();
    do
    {
        tmp = MQTTPacket_read(buf, buflen, tcp_read_data);
        if(tmp == CONNACK)
        {
            unsigned char sessionPresent, connack_rc;
            if(MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
            {
                printf("Unable to connect, return code %d\n", connack_rc);
            }
            printf("connect mqtt server is ok !\r\n");
            return 1;
        }
        osDelay(100);
        tmp_time++;
    }
    while(tmp != CONNACK && (tmp_time < 20));
//    while(tmp != CONNACK && (millis() - tmp_time < 1000));
    return -2;
    
    
}

/*
@return  1:ok;
*/
int MQTT_Disconnect()
{
  uint8_t  buf[128] = {0};
  int32_t len=0;
  int buflen = sizeof(buf);
    
  len = MQTTSerialize_disconnect(buf, buflen ); 
  tcp_write_data(buf,len);
    
  close(MQTT_Socket);

  return 1;
}
/*
@return  1:ok;
        -1:send topick failed;
        -2:publish failed;
*/
int MQTT_Publish(char *topick, char *message)
{
    uint16_t i = 0;
    uint8_t  buf[256] = {0};
    int32_t len=0;
    int rc = 0;
    int buflen = sizeof(buf);  
    MQTTString topicString = MQTTString_initializer;
    int msglen = strlen(message);


    topicString.cstring = topick;


    printf("----------start:publish-----------------\n");


    //组织消息
    len = MQTTSerialize_publish(buf, buflen, 0, 1, 0, 0, topicString, (unsigned char*)message, msglen); /* 2 */
    printf("message : %s\n",message);
//    osDelay(100);
    rc = tcp_write_data(buf, len);
    printf("mqtt pub topic:%s; len:%d\n", topick, rc);
    for(i=0; i<rc; i++)
    {
        printf("%02X ", buf[i]);
    }
    printf("\n");
    if(rc > 0)
    {
        printf("topick  : %s\n",topick);
        printf("message : %s\n",message);
        printf("trans is ok,len = %d\n",rc);
    }
    else
    {
        printf("trans is failed,len = %x\n",rc);
        printf("---------------------------\n");
        return -1;
    }
    if(rc == len)
    {
        printf("Successfully published\n");
    }
    else
    {
        printf("Publish failed\n");
        return -2;
    }
    printf("-----------end------------------\n");
    return 1;
}

int MQTT_Subscribe(char *topick)
{
    uint8_t  buf[128] = {0};
    int32_t len=0;
    int tmp = 0;
    uint32_t tmp_time;
    int buflen = sizeof(buf);  
	  int msgid = 1;
	  int req_qos = 1;

	  char* payload = "mypayload";
    MQTTString topicString = MQTTString_initializer;
	  int payloadlen = strlen(payload);
    topicString.cstring = topick;


	  /* subscribe */
	  topicString.cstring = topick;
	  len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);

	  tcp_write_data(buf, len);
    tmp_time = 0;//retry 5 times at most here;
    do{
			  tmp_time++;
        tmp = MQTTPacket_read(buf, buflen, tcp_read_data);
        if (tmp == SUBACK) 	/* wait for suback */
        {
            unsigned short submsgid;
            int subcount;
            int granted_qos;

            MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
            if (granted_qos != 0)
            {
                printf("granted qos != 0, %d\n", granted_qos);
            }
            printf("subscribe:ok!\r\n");
            printf("topick   :\"%s\"\r\n",topick);
            printf("submsgid:%d\r\n",submsgid);
            printf("subcount:%d\r\n",subcount);
            printf("granted_qos:%d\r\n",granted_qos);
            return 0;
        }
				osDelay(100);
    }
    while(tmp != SUBACK && (tmp_time < 5));
	  return -1;
}

void MQTT_Loop(uint16_t *last_ping_time)
{
    uint8_t buf[128] = {0};
    uint8_t ping_buf[2];
    uint8_t ack_buf[10] = {0};
    uint8_t ack_buf_len = 10;
    uint8_t ack_len = 0;
//    uint16_t last_ping_time = 0;
	int     ping_buf_len = sizeof(ping_buf);
    int     ping_len = MQTTSerialize_pingreq(ping_buf, ping_buf_len);
    int     buflen = sizeof(buf);  

//    if(millis() - last_ping_time > 5000)
//    {
//        
//        last_ping_time = millis();
//        transport_sendPacketBuffer(ping_buf, ping_len);
//        printf("send ping\r\n");
//    }
    if(*last_ping_time > 30)
    {
        
        *last_ping_time = 0;// = millis();
        tcp_write_data(ping_buf, ping_len);
        printf("send ping\r\n");
        osDelay(10);
    }
    if(1)//callback
    {
        /* transport_getdata() has a built-in 1 second timeout,
        your mileage will vary */
        int tmp = MQTTPacket_read(buf, buflen, tcp_read_data);
        if (tmp == PUBLISH)
        {
            unsigned char dup;
            int qos = 0;
            unsigned char retained;
            unsigned short msgid;
            int payloadlen_in;
            unsigned char* payload_in;
            int rc;
            MQTTString receivedTopic;

            rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic, &payload_in, &payloadlen_in, buf, buflen);
            if(rc == 1)
            {
                printf("message  :\" %.*s\"   qos:%d  msgid:%d\n", payloadlen_in, payload_in, qos, msgid);
//                callback(receivedTopic.lenstring.data,receivedTopic.lenstring.len,(char*)payload_in,payloadlen_in);
                
                /*******************返回puback******************/
                if(qos)
                {
                    ack_len = MQTTSerialize_puback(ack_buf, ack_buf_len, msgid);

                    rc = tcp_write_data(ack_buf, ack_len);
                    if (rc == ack_len)
                        printf("send puback ok\n\r");
                    else
                        printf("send Puback failed\n\r");
                    
                }
            }
            else
            {
                printf("message  err :%d",rc); 
            }                
        }
        else if(tmp == PINGRESP)
        {
            printf("recv ping\r\n");
        }
        else
        {
            printf("other pack! %d\r\n",tmp);
        }
    }
//    osDelay(100);
}

//MQTT& MQTT::setCallback(void(*callback)(char*,int, char*, int))
//{
//    this->callback = callback;
//    return *this;
//}
