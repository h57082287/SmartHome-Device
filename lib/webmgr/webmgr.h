#ifndef WEBMGR_H
#define WEBMGR_H

#include <common.h>
// #include <WebServer.h>
#include <ESP8266WebServer.h>

class webmgr
{
private:
    ESP8266WebServer *server;
    int ap_nums;
    void __handler_getAPList(AP aps);
public:
    webmgr();
    ~webmgr();
    void setupAPI(AP aps);
    void setupWeb();
    void setAPNums(int nums);
    
};

#endif