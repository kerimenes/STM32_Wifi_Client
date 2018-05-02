#include "mbed.h"
#include "wifi.h"
/*------------------------------------------------------------------------------
Hyperterminal settings: 115200 bauds, 8-bit data, no parity

This example 
  - connects to a wifi network (SSID & PWD to set in mbed_app.json)
  - Connects to a TCP server (set the address in RemoteIP)
  - Sends "Hello" to the server when data is received

This example uses SPI3 ( PE_0 PC_10 PC_12 PC_11), wifi_wakeup pin (PB_13), 
wifi_dataready pin (PE_1), wifi reset pin (PE_8)
------------------------------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
#define WIFI_WRITE_TIMEOUT 10000
#define WIFI_READ_TIMEOUT  10000
#define CONNECTION_TRIAL_MAX          10

/* Private typedef------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Serial pc(SERIAL_TX, SERIAL_RX);
uint8_t RemoteIP[] = {MBED_CONF_APP_SERVER_IP_1,MBED_CONF_APP_SERVER_IP_2,MBED_CONF_APP_SERVER_IP_3, MBED_CONF_APP_SERVER_IP_4};
uint8_t RxData [500];
char* modulename;
uint8_t TxData[] = "STM32 : Hello!\n";
uint16_t RxLen;
uint8_t  MAC_Addr[6]; 
uint8_t  IP_Addr[4]; 

int main()
{
    int32_t Socket = -1;
    uint16_t Datalen;
    uint16_t Trials = CONNECTION_TRIAL_MAX;

    pc.baud(115200);

    printf("\n");
    printf("************************************************************\n");
    printf("***   STM32 IoT Discovery kit for STM32L475 MCU          ***\n");
    printf("***      WIFI Module in TCP Client mode demonstration    ***\n\n");
    printf("*** TCP Client Instructions :\n");
    printf("*** 1- Make sure your Phone is connected to the same network that\n");
    printf("***    you configured using the Configuration Access Point.\n");
    printf("*** 2- Create a server by using the android application TCP Server\n");
    printf("***    with port(8002).\n");
    printf("*** 3- Get the Network Name or IP Address of your phone from the step 2.\n\n"); 
    printf("************************************************************\n");

    /*Initialize  WIFI module */
    if(WIFI_Init() ==  WIFI_STATUS_OK) {
        printf("> WIFI Module Initialized.\n");  
        if(WIFI_GetMAC_Address(MAC_Addr) == WIFI_STATUS_OK) {
            printf("> es-wifi module MAC Address : %X:%X:%X:%X:%X:%X\n",     
                   MAC_Addr[0],
                   MAC_Addr[1],
                   MAC_Addr[2],
                   MAC_Addr[3],
                   MAC_Addr[4],
                   MAC_Addr[5]);   
        } else {
            printf("> ERROR : CANNOT get MAC address\n");
        }
    
        if( WIFI_Connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, WIFI_ECN_WPA2_PSK) == WIFI_STATUS_OK) {
            printf("> es-wifi module connected \n");
            if(WIFI_GetIP_Address(IP_Addr) == WIFI_STATUS_OK) {
                printf("> es-wifi module got IP Address : %d.%d.%d.%d\n",     
                       IP_Addr[0],
                       IP_Addr[1],
                       IP_Addr[2],
                       IP_Addr[3]); 
        
                printf("> Trying to connect to Server: %d.%d.%d.%d:8002 ...\n",     
                       RemoteIP[0],
                       RemoteIP[1],
                       RemoteIP[2],
                       RemoteIP[3]);
        
                while (Trials--){ 
                    if( WIFI_OpenClientConnection(0, WIFI_TCP_PROTOCOL, "TCP_CLIENT", RemoteIP, 8002, 0) == WIFI_STATUS_OK){
                        printf("> TCP Connection opened successfully.\n"); 
                        Socket = 0;
                    }
                }
                if(!Trials) {
                    printf("> ERROR : Cannot open Connection\n");
                }
            } else {
                printf("> ERROR : es-wifi module CANNOT get IP address\n");
            }
        } else {
            printf("> ERROR : es-wifi module NOT connected\n");
        }
    } else {
        printf("> ERROR : WIFI Module cannot be initialized.\n"); 
    }
  
    while(1){
        if(Socket != -1) {
            if(WIFI_ReceiveData(Socket, RxData, sizeof(RxData), &Datalen, WIFI_READ_TIMEOUT) == WIFI_STATUS_OK){
                if(Datalen > 0) {
                    if(WIFI_SendData(Socket, TxData, sizeof(TxData), &Datalen, WIFI_WRITE_TIMEOUT) != WIFI_STATUS_OK) {
                        printf("> ERROR : Failed to send Data.\n");   
                    } 
                }
            } else {
                printf("> ERROR : Failed to Receive Data.\n");  
            }
        }
    }
}
