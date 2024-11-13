#include <webmgr.h>
#include <ArduinoJson.h>


void webmgr::homepage(){
}

webmgr::webmgr(){
    this->server = new ESP8266WebServer(80);
}

webmgr::~webmgr(){
    free(this->server);
}

void webmgr::__handler_getAPList(AP *aps){
    int i;
    JsonDocument json;
    char responds[5120];
    JsonArray ap_list = json["aps"].to<JsonArray>();
    for(i=0; i<this->ap_nums; i++){
        JsonDocument buff;
        buff["SSID"] = aps[i].SSID;
        buff["RSSI"] = aps[i].RSSI;
        buff["channel"] = aps[i].channel;
        // buff["MAC"] = aps[i].MAC;
        ap_list.add(buff);
    }
    this->__Print_Log("Prepar responds data....", "INFO");
    serializeJson(json, responds);
    Serial.printf("%s\n", responds);
    this->server->send(200, "application/json", responds);
    this->__Print_Log("Data send done !", "INFO");
    this->__Print_Log("User out....", "INFO");
}

void webmgr::setupAPI(AP *aps){
    this->server->on("/aplist", [this, aps](){
        this->__Print_Log("User in....", "INFO");
        this->__handler_getAPList(aps);
    });
}

void webmgr::setAPNums(int nums){
    this->ap_nums = nums;
    Serial.printf("[webmgr][INFO]Memory allocate....\n");
    char *msg = (char*)malloc(sizeof(char)*50);
    Serial.printf("[webmgr][INFO]Combine messages....\n");
    sprintf(msg, "Scanning %d APs....", nums);
    this->__Print_Log(msg, "INFO");
    free(msg);
    Serial.printf("[webmgr][INFO]Free memory....\n");
}

void webmgr::start(){
    if(this->first_boot){
        this->server->begin();
        this->first_boot = false;
        this->__Print_Log("Web server is running....", "INFO");
    }
    this->server->handleClient();
}

void webmgr::__Print_Log(char log[], char level[]){
    Serial.printf("[webmgr][%s]%s\n", level, log);
}



