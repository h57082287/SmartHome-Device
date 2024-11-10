#ifndef WIFIMGR_H
#define WIFIMGR_H
// #include <WiFi.h>
#include <ESP8266WiFi.h>

class wifimgr
{
private:
    ESP8266WiFiClass *wifi;
    int begin=0, end = 60, timeout=60;

public:
    wifimgr();
    ~wifimgr();
    bool connectAP(char* ssid, char* password);
    AP getAPStatus();
    int getAPList(AP *info);
};
#endif

