#include <common.h>
#include <wifimgr.h>

wifimgr::wifimgr(){
    wifi = new ESP8266WiFiClass();
    this->wifi->mode(WIFI_AP_STA);
}

wifimgr::~wifimgr(){
    free(this->wifi);
}

bool wifimgr::connectAP(char* ssid, char * password){
    int i;
    bool status = false;
    this->wifi->begin(ssid, password);
    for(;;){
        if((this->wifi->status()) != WL_CONNECTED){
            Serial.println("[wifimgr][INFO] Try to connect to AP.");
            this->end = millis();
        }else{
            Serial.println("[wifimgr][INFO]WiFi connected ~~");
            status = true;
            break;
        }
        if((this->end - this->begin) > (this->timeout*1000)){
            this->timeout = 0;
            Serial.println("[wifimgr][Error] Connect failed !!!");
            break;
        }
        delay(1000);
    }
    return status;
}

AP wifimgr::getAPStatus(){
    AP info;
    memcpy(info.IPv4, (char*)this->wifi->localIP().toString().c_str(), IPv4_LENS);
    memcpy(info.SSID, this->wifi->SSID().c_str(), SSID_LENS);
    info.channel = this->wifi->channel();
    info.RSSI = this->wifi->RSSI();
    return info;
}

int wifimgr::getAPList(AP *aps){
    int i, ap_nums = this->wifi->scanNetworks();
    for(i=0;i<ap_nums;i++){
        memcpy(aps[i].SSID, (char*)this->wifi->SSID(i).c_str(), MAX_SSID_LEN);
        aps[i].channel = this->wifi->channel(i);
        aps[i].RSSI = this->wifi->RSSI(i);
    }
    return ap_nums;
}


