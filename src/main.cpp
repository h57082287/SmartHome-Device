#include <common.h>
#include <wifimgr.h>
#include <webmgr.h>
#include <Arduino.h>

// Define funcation
void Debug_AP_info(AP info, bool enable);
void Debug_LIST_AP(AP *aps, int ap_nums, bool enable);

wifimgr *wifi;
webmgr *web;

void setup() {
  // Init serial port
  Serial.begin(9600);

  // Setting WiFi info
  char ssid[10] = "Johnson4G";
  char key[9]= "77777777";

  Serial.printf("\n\n\n\n\nSystem boot up......\n");
  // Declare wifimgr
  wifi = new wifimgr();
  web = new webmgr(wifi);
  Serial.printf("[System][INFO]wifimgr created.\n");

  Serial.printf("[System][INFO]Aollocation memery address.....\n");
  // Init memery space
  AP info;
  AP *aps = (AP*)malloc(sizeof(AP)*MAX_LIST_AP);
  memset(aps, 0, sizeof(AP)*MAX_LIST_AP);
  Serial.printf("[System][INFO]Memery alloc done !\n");

  // Setup funcation
  wifi->connectAP(ssid, key);
  info = wifi->getAPStatus();
  int nums = wifi->getAPList(aps);
  
  // Show debug message 
  Debug_AP_info(info, true);
  Debug_LIST_AP(aps, nums, true);

  // Setup web api
  Serial.printf("[System][INFO]Ready to setup web server...\n");
  web->setAPNums(nums);
  Serial.printf("[System][INFO]Setup web API...\n");
  web->setupAPI(aps);
  Serial.printf("[System][INFO]Web Server is ready !\n");

  // Realese resource
  // free(aps);
  free(wifi);
}

void loop() {
  web->start();
}

void Debug_AP_info(AP info, bool enable=false){
  if(enable){
    Serial.printf("[MAIN][MESSAGE]IPv4 Address: %s\n", info.IPv4);
    Serial.printf("[MAIN][MESSAGE]SSID: %s\n", info.SSID);
    Serial.printf("[MAIN][MESSAGE]CH%d\n", info.channel);
    Serial.printf("[MAIN][MESSAGE]RSSI: %d dBm\n", info.RSSI);
  }
}

void Debug_LIST_AP(AP *aps, int ap_nums, bool enable=false){
  int i;
  if(enable){
    Serial.printf("[MAIN][MESSAGE]AP Nums = %d\n", ap_nums);
    for(i=0;i<ap_nums;i++){
      Serial.printf("---------------------------\n");
      Serial.printf("[MAIN][MESSAGE]SSID: %s\n", aps[i].SSID);
      Serial.printf("[MAIN][MESSAGE]CH: %d\n", aps[i].channel);
      Serial.printf("[MAIN][MESSAGE]RSSI: %d dBm\n", aps[i].RSSI);
      Serial.printf("[MAIN][MESSAGE]MAC: %s\n", aps[i].MAC);
      Serial.printf("---------------------------\n");
    }
  }
}