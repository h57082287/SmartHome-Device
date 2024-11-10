#ifndef WEBMGR_H
#define WEBMGR_H

#include <common.h>
#include <WebServer.h>

class webmgr
{
private:
    WebServer *server;
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