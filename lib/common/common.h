#ifndef COMMON_H
#define COMMON_H

#define SSID_LENS 255
#define IPv4_LENS 15
#define MAX_LIST_AP 255


typedef struct{
    char SSID[SSID_LENS];
    int channel;
    int RSSI;
    char IPv4[IPv4_LENS];
}AP;



#endif