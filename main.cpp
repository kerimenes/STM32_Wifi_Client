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
uint8_t RemoteIP[] = {192, 168, 2, 5};
uint8_t RxData [500];
char* modulename;
uint8_t TxData[] = "STM32 : Hello!\n";
uint16_t RxLen;
uint8_t  MAC_Addr[6]; 
uint8_t  IP_Addr[4]; 


uint16_t myPort = 8002;
int32_t socket = -1;

int initWifi(void);
int openWifiClientConnection(uint8_t *ipaddr, uint16_t port);

int main()
{
    uint16_t Datalen;
    uint16_t Trials = CONNECTION_TRIAL_MAX;

    pc.baud(115200);

    printf("\n");
    printf("Starting...");
    printf("************************************************************\n");

    /*Initialize  WIFI module */
    int ret = initWifi();
    if (ret != 0) {
        printf("Closing\r\n");
        return -1;
    }

    ret = openWifiClientConnection(RemoteIP, myPort);
    if (ret != 0) {
        printf("Closing\r\n");
        return -1;
    }

    while(1){
        if(socket != -1) {
            if(WIFI_ReceiveData(socket, RxData, sizeof(RxData), &Datalen, WIFI_READ_TIMEOUT) == WIFI_STATUS_OK){
                if(Datalen > 0) {
                    if(WIFI_SendData(socket, TxData, sizeof(TxData), &Datalen, WIFI_WRITE_TIMEOUT) != WIFI_STATUS_OK) {
                        printf("> ERROR : Failed to send Data.\n");
                    } 
                }
            } else {
                printf("> ERROR : Failed to Receive Data.\n");
            }
        }
    }
}

int initWifi(void)
{
    int ret = 0;
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
            ret = -1;
        }

        if( WIFI_Connect("Mickey2018", "mickey2018", WIFI_ECN_WPA2_PSK) == WIFI_STATUS_OK) {
            printf("> es-wifi module connected \n");
            if(WIFI_GetIP_Address(IP_Addr) == WIFI_STATUS_OK) {
                printf("> es-wifi module got IP Address : %d.%d.%d.%d\n",
                       IP_Addr[0],
                       IP_Addr[1],
                       IP_Addr[2],
                       IP_Addr[3]);
            } else {
                printf("> ERROR : es-wifi module CANNOT get IP address\n");
                ret = -2;
            }
        } else {
            printf("> ERROR : es-wifi module NOT connected\n");
            ret = -3;
        }
    } else {
        printf("> ERROR : WIFI Module cannot be initialized.\n");
        ret = -4;
    }
    return ret;
}

int openWifiClientConnection(uint8_t *ipaddr, uint16_t port)
{
    int ret = 0;
    printf("> Trying to connect to Server: %d.%d.%d.%d:%d ...\n",
           ipaddr[0],
           ipaddr[1],
           ipaddr[2],
           ipaddr[3],
            myPort);
    uint16_t Trials = CONNECTION_TRIAL_MAX;
    while(Trials--) {
        if (WIFI_OpenClientConnection(0, WIFI_TCP_PROTOCOL, "TCP_CLIENT",ipaddr, port, 0) == WIFI_STATUS_OK) {
            printf("> TCP Connection opened successfully.\n");
            socket = 0;
            ret = 0;
        }
    }
    if (Trials == 0)
        ret = -1;
    return ret;
}
