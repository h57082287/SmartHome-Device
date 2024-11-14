#ifndef COMMON_H
#define COMMON_H

#define SSID_LENS 255
#define IPv4_LENS 18
#define MAX_LIST_AP 10
#define MAX_SSID_LEN 255
#define MAC_LEN 20

typedef struct{
    char SSID[SSID_LENS];
    int channel;
    int RSSI;
    char IPv4[IPv4_LENS];
    char MAC[MAC_LEN];
    }AP;



#endif