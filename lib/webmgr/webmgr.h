#ifndef WEBMGR_H
#define WEBMGR_H

#include <common.h>
#include <wifimgr.h>
#include <ESP8266WebServer.h>

class webmgr
{
private:
    ESP8266WebServer *server;
    wifimgr *wifi;
    int ap_nums = 5;
    bool first_boot = true;
    void __handler_getAPList(AP *aps);
    void __Print_Log(char log[], char level[]);
public:
    webmgr(wifimgr *wifi);
    ~webmgr();
    void setupAPI(AP *aps);
    void setupWeb();
    void setAPNums(int nums);
    void start();
    void refresh();
};

#endif