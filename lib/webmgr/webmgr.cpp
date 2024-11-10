#include <webmgr.h>
#include <ArduinoJson.h>

webmgr::webmgr(){
    this->server = new ESP8266WebServer(80);
}

webmgr::~webmgr(){
    free(this->server);
}

void webmgr::__handler_getAPList(AP aps){
    int i;
    // ArduinoJson json = new ArduinoJson();
    // for(i=0; i<this->ap_nums; i++){
        
    // }
    // this->server->send(200, "text/json", content);
}

void webmgr::setupAPI(AP aps){
    this->server->on("/get/aplist", [this, aps]{
        this->__handler_getAPList(aps);
    });
}

void webmgr::setAPNums(int nums){
    this->ap_nums = nums;
}






