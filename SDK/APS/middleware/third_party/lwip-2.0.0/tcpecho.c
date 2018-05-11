#include "lwip_helper.h"
#include "tcpecho.h"

#include "lwip/opt.h"


/* for lwip_socket example task*/
#define SOCKET_CLINET_EXAMPLE_TASK_NAME    "client_test"
#define SOCKET_TCPS_EXAMPLE_TASK_NAME      "tcp_server"
#define SOCKET_UDPS_EXAMPLE_TASK_NAME      "udp_server"
#define SOCKET_EXAMPLE_TASK_STACKSIZE      (256 * 4)
#define SOCKET_EXAMPLE_TASK_PRIO           (tskIDLE_PRIORITY + 1)

/* for user entry example task*/
#define USER_ENTRY_TASK_NAME               "user_entry"
#define USER_ENTRY_TASK_STACKSIZE          (512 * 4)
#define USER_ENTRY_TASK_PRIO               (tskIDLE_PRIORITY + 1)

#define SOCKET_LOGI(fmt,arg...)   printf(("[Socket]: "fmt"\r\n"), ##arg)

#define SOCK_TCP_SRV_PORT        8080
#define SOCK_UDP_SRV_PORT        6060
#define TRX_PACKET_COUNT         3

#if LWIP_SOCKET && (LWIP_IPV4 || LWIP_IPV6)
#include <string.h>
#include "lwip/sockets.h"

/**
  * @brief  Tcp server create socket, wait for client connection and receive/send data
  * @param  None
  * @return None
  */
static void tcp_server_test(void)
{
    int s;
    int c;
    int ret;
    int rlen;
    struct sockaddr_in addr;
    char srv_buf[128] = {0};
    SOCKET_LOGI("tcp_server_test starts");

    memset(&addr, 0, sizeof(addr));
    addr.sin_len = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = lwip_htons(SOCK_TCP_SRV_PORT);
    addr.sin_addr.s_addr = lwip_htonl(IPADDR_ANY);

    /* Create the socket */
    s = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        SOCKET_LOGI("TCP server create failed");
        goto done;
    }

    ret = lwip_bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        SOCKET_LOGI("TCP server bind failed");
        goto clean;
    }

    ret = lwip_listen(s, 0);
    if (ret < 0) {
        SOCKET_LOGI("TCP server listen failed");
        goto clean;
    }

    do {
        socklen_t sockaddr_len = sizeof(addr);
        c = lwip_accept(s, (struct sockaddr *)&addr, &sockaddr_len);
        if (c < 0) {
            SOCKET_LOGI("TCP server accept error");
            break;
        }

        SOCKET_LOGI("TCP server waiting for data...");
        while ((rlen = lwip_read(c, srv_buf, sizeof(srv_buf) - 1)) != 0) {
            if (rlen < 0) {
                SOCKET_LOGI("read error");
                break;
            }
            srv_buf[rlen] = 0; //for the next statement - printf string.
            SOCKET_LOGI("TCP server received data:%s", srv_buf);

            lwip_write(c, srv_buf, rlen);
        }

        lwip_close(c);
    }while(0);

clean:
    lwip_close(s);
    SOCKET_LOGI("TCP server s close:ret = %d", ret);
done:
    SOCKET_LOGI("TCP server test completed");
}



/**
  * @brief  Udp server create socket and receive/send data
  * @param  None
  * @return None
  */
static void udp_server_test(void)
{
    int s;
    int ret;
    struct sockaddr_in addr, clnt_addr;
    char rcv_buf[128] = {0};
    int count = 0;
    SOCKET_LOGI("udp_server_test starts");

    memset(&addr, 0, sizeof(addr));
    addr.sin_len = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = lwip_htons(SOCK_UDP_SRV_PORT);
    addr.sin_addr.s_addr = lwip_htonl(IPADDR_ANY);

    /* Create the socket */
    s = lwip_socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        SOCKET_LOGI( "UDP server create failed");
        goto idle;
    }

    ret = lwip_bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        lwip_close(s);
        SOCKET_LOGI("UDP server bind failed");
        goto idle;
    }

    while (count < TRX_PACKET_COUNT) {
        socklen_t clnt_len = sizeof(clnt_addr);
        ret = lwip_recvfrom(s, rcv_buf, sizeof(rcv_buf), 0, (struct sockaddr *)&clnt_addr, &clnt_len);
        if (ret <= 0) {
            lwip_close(s);
            SOCKET_LOGI("UDP server recv failed");
            goto idle;
        }
        SOCKET_LOGI("UDP server received data:%s", rcv_buf);

        lwip_sendto(s, rcv_buf, strlen(rcv_buf), 0, (struct sockaddr *)&clnt_addr, clnt_len);

        count++;
    }

    ret = lwip_close(s);
    SOCKET_LOGI("UDP server s close:ret = %d", ret);
idle:
    SOCKET_LOGI("UDP server test completed");
}
#endif


#if LWIP_NETCONN

#include "lwip/sys.h"
#include "lwip/api.h"
#include <string.h>

static sys_sem_t tcpsem;

/*-----------------------------------------------------------------------------------*/

/**
  * @brief  Tcp server create socket, wait for client connection and receive/send data
  * @param  [in] arg 
  * @return None
  */
static void tcpecho_thread(void *arg)
{
  struct netconn *conn, *newconn;
  err_t err;
  LWIP_UNUSED_ARG(arg);

  /* Create a new connection identifier. */
  conn = netconn_new(NETCONN_TCP);

  /* Bind connection to well known port number 7. */
  err = netconn_bind(conn, NULL, 8080);
  
  if(err == ERR_OK)
  {	  
      /* Tell connection to go into listening mode. */
      netconn_listen(conn);

      while (1) {
        err_t err;
        /* Grab new connection. */
        err = netconn_accept(conn, &newconn);
        printf("accepted new connection %p\n", newconn);
        /* Process the new connection. */
        if (err == ERR_OK) {
          struct netbuf *buf;
          void *data;
          u16_t len;
 
          while ((err = netconn_recv(newconn, &buf)) == ERR_OK) {
            //printf("Recved\n");;
            do 
            {
                netbuf_data(buf, &data, &len);
                
                //err = netconn_write(newconn, data, len, NETCONN_COPY);
    #if 1
                if (err != ERR_OK) {
                  printf("tcpecho: netconn_write: error \"%s\"\n", lwip_strerr(err));
                }
    #endif
            } while (netbuf_next(buf) >= 0);
            *((char*)data+len) = '\0';   
            printf("tcpecho server:received data: %s\n",(char*)data);
            netbuf_delete(buf);
            
          }
          /*printf("Got EOF, looping\n");*/ 
          /* Close connection and discard connection identifier. */
          netconn_close(newconn);
          netconn_delete(newconn);
        }
      }
  }
}

void tcpecho_client(void *arg)
{
    // Connection
    struct netconn *conn;
    ip_addr_t remote_addr;   
    // Payload
	const char buf[] = "This is a Tcp echo test, are you ready";
	char send_cache[64];
    // Error checking
    static err_t err;
    int count = 0;

    IP_ADDR4(&remote_addr, 190,168,1,99);    
    // Forever, unless requested to terminate
    while (1) {
        // Create new connection
        conn = netconn_new( NETCONN_TCP );
        
        // Connect to ip
        err = netconn_connect(conn, &remote_addr, 8080 );
        // Verify connection
        if(err != ERR_OK){
            printf("tcpecho: netconn_connect: error \n");
        }
        else
        {
            // Continously send a datastring with systime
            do{
                // Assemble datastring in cache
                int size = sprintf(send_cache,"%s[%d]\r\n", buf,count++);

                //TCP segments are only sent when a call to tcp_output() function is made, this
                //function is also automatically triggered by lwip in the following cases
                //1. Inside the tcp_input() function, when TCP ack has to be sent right away
                //2. Inside the slow and fast timers, where retransmitting TCP segments can be required
                sys_msleep(100);
                // Write cache to tcp, blocking without copy to lwip data
                err = netconn_write(conn, send_cache, size, NETCONN_COPY);
            }while(err == ERR_OK );
            // report disconnect
            printf("client write err %d\n", netconn_err(conn));
         }
         
         // Failed, close connection (probably caused by a disconnect or ignore by the server)
         netconn_close(conn);
         // Delete connection and start over
         netconn_delete(conn);
         sys_msleep(10);         
      } // end of while
}

void tcp_client_send(void *arg)
{
    // Connection
    struct netconn *conn;
    ip_addr_t remote_addr;
    // Payload
    char buf[] = "This is a Tcp echo test, are you ready";
    char send_cache[64];
    // Error checking
    static err_t err;
    int count = 0;

    IP_ADDR4(&remote_addr, 192,168,0,101);
    // Forever, unless requested to terminate
    while (1) {
        // Create new connection
        conn = netconn_new( NETCONN_TCP );

        // Connect to ip
        err = netconn_connect(conn, &remote_addr, 8080 );
        // Verify connection
        if(err != ERR_OK){
            printf("tcpecho: netconn_connect: error \n");
        }
        else
        {
            // Continously send a datastring with systime
            do{
                // Assemble datastring in cache
                int size = sprintf(send_cache,"%s[%d]\r\n", buf,count++);

                //TCP segments are only sent when a call to tcp_output() function is made, this
                //function is also automatically triggered by lwip in the following cases
                //1. Inside the tcp_input() function, when TCP ack has to be sent right away
                //2. Inside the slow and fast timers, where retransmitting TCP segments can be required
                sys_msleep(100);
                // Write cache to tcp, blocking without copy to lwip data
                err = netconn_write(conn, send_cache, size, NETCONN_COPY);
            }while(err == ERR_OK );
            // report disconnect
            printf("client write err %d\n", netconn_err(conn));
         }

         // Failed, close connection (probably caused by a disconnect or ignore by the server)
         netconn_close(conn);
         // Delete connection and start over
         netconn_delete(conn);
         sys_msleep(10);
      } // end of while
}

/**
  * @brief  Tcp server create socket, wait for client connection and receive data
  * @param  [in] arg 
  * @return None
  */
static void tcp_server_thread(void *arg)
{
    struct netconn *conn, *newconn;
    err_t err;
    LWIP_UNUSED_ARG(arg);

    SOCKET_LOGI("tcp_server_thread starts\r\n");

    /* Create a new connection identifier. */
    conn = netconn_new(NETCONN_TCP);

    /* Bind connection to well known port number. */
    err = netconn_bind(conn, NULL, SOCK_TCP_SRV_PORT);
  
    if(err == ERR_OK)
    {	  
        /* Tell connection to go into listening mode. */
        netconn_listen(conn);

        while (1) {
            err_t err;
            /* Grab new connection. */
            err = netconn_accept(conn, &newconn);
            SOCKET_LOGI("TCP server accepted new connection %p\n", newconn);
            SOCKET_LOGI("TCP server waiting for data...");
            /* Process the new connection. */
            if (err == ERR_OK) {
                struct netbuf *buf;
                void *data;
                u16_t len;

                while ((err = netconn_recv(newconn, &buf)) == ERR_OK) {
                    do {
                        netbuf_data(buf, &data, &len);
                    } while (netbuf_next(buf) >= 0);

                    *((char*)data+len) = '\0';   
                    SOCKET_LOGI("TCP socket server received data: %s\n",(char*)data);
                    netbuf_delete(buf);
                }

                /* Close connection and discard connection identifier. */
                err = netconn_close(newconn);
                SOCKET_LOGI("TCP server s close:ret = %d", err);
                netconn_delete(newconn);
                SOCKET_LOGI("TCP server test completed");              
            }
        }
    }
}


/*-----------------------------------------------------------------------------------*/
void tcpecho_init(void)
{
    sys_sem_new(&tcpsem, 0);
    sys_thread_new("tcpecho_thread", tcpecho_thread, NULL, DEFAULT_THREAD_STACKSIZE, 2);
    sys_thread_new("tcpecho_Client", tcpecho_client, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);  
}
/*-----------------------------------------------------------------------------------*/

void tcp_client(char *param[])
{
    sys_thread_new("tcp_clint_thread", tcp_client_send, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}

static void user_entry(void *args)
{
    lwip_net_ready();
    tcp_server_thread(args);
    vTaskDelete(NULL);
}

int socket_server_demo(void)
{
    /* Initialize wifi stack and register wifi init complete event handler*/

    //TODOL: wifi_init(&config);

    /* Tcpip stack and net interface initialization,  dhcp client, dhcp server process initialization*/
    lwip_network_init(WIFI_MODE_STA_ONLY);
    lwip_net_start(WIFI_MODE_STA_ONLY);

    /* Create a user task for demo */
    if (NULL == sys_thread_new(SOCKET_TCPS_EXAMPLE_TASK_NAME, 
                               user_entry, 
                               NULL,
                               SOCKET_EXAMPLE_TASK_STACKSIZE/sizeof(portSTACK_TYPE),
                               SOCKET_EXAMPLE_TASK_PRIO))
    {
        printf("create socket server task failed\r\n");
        return -1;
    }
    return 0;
}

#endif /* LWIP_NETCONN */
