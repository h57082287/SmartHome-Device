#include <Arduino.h>
#include <cJSON.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#define DEBUG 1
#define MAXLIST 10
#define MAINPAGE_FILE "/index.html"
#define HOSTNAME "Johnson-IoT-Device"
#define PASSWORD "1234567890"
#define APRECORD_FILE "/tmp/connect.json"
#define USER_INFO_FILE "/tmp/user.json"

const char* ssid     = "HITRON-3C";
const char* password = "0960000573";
// const char* ssid     = "MyHouse";
// const char* password = "034594054";
const char* root_path = "/test.txt";
// char* content;
uint8_t buffer[512];
char header[1024];
char content[4096];
char record_content[256];
char data[15][256] = {};

// Define HTTP methods
enum http_method {
  Post = 1,
  Get,
  Put,
  Delete,
  Unknown
};

struct http_request {
  http_method method;
  char* path;
  char* params;
  char* body;
};

static http_request req;

WiFiServer server(80);

// Declare function
void parse_http_request(http_request *req, const char* raw_req);
void read_file(char* cntx, const char* file);
void create_header(char* header, char* path);
void doAPIProcess(char* result, http_method method, char* path, char* params, char* body);

void setup() {
  Serial.begin(115200);
  if (LittleFS.begin()) {
    Serial.println("LittleFS mounted successfully");
  } else {
    Serial.println("LittleFS mount failed");
  }
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(HOSTNAME, PASSWORD);
  if (LittleFS.exists(APRECORD_FILE)) {
    read_file(record_content, APRECORD_FILE);
    cJSON *json = cJSON_Parse(record_content);
    if (json != NULL) {
      cJSON *ssidNode = cJSON_GetObjectItem(json, "SSID");
      cJSON *pskNode = cJSON_GetObjectItem(json, "PSK");
      if (ssidNode != NULL && pskNode != NULL) {
        const char* ssid = ssidNode->valuestring;
        const char* psk = pskNode->valuestring;
        WiFi.begin(ssid, psk);
      }
    }
    cJSON_Delete(json);
  }
  while (WiFi.status() != WL_CONNECTED && WiFi.softAPgetStationNum() == 0) {
    Serial.print(".");
    delay(1000);
  }
  if (WiFi.softAPgetStationNum() > 0) {
    Serial.println("\nStation connected to AP !!!");
  }
  if (MDNS.begin("johnson-iot-device")) {
#ifdef DEBUG
    Serial.println("MDNS responder started");
#endif
  }
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
#ifdef DEBUG
  Serial.println("Heap: " + String(ESP.getFreeHeap()));
  delay(500);
#endif
  MDNS.update();
  WiFiClient client = server.accept();
  if (client) {
# ifdef DEBUG
    Serial.println("Client was accepted");
# endif
    while (client.connected()) {
      if (client.available()) {
        parse_http_request(&req, client.readString().c_str());
        create_header(header, req.path);
        client.println(header);
        if (strcmp(req.path, "/") == 0) {
          Serial.println("Serving main page");
          File fs = LittleFS.open(MAINPAGE_FILE, "r");
          while (fs.available())
          {
              int byteRead = fs.read(buffer, sizeof(buffer));
              client.write(buffer, byteRead);
          }
          fs.close();
        } else if (strstr(req.path, "/api") != NULL) {
          doAPIProcess(content, req.method, req.path, req.params, req.body);
# ifdef DEBUG
          Serial.printf("API response: %s\n", content);
# endif
          client.println(content);
        } else {
          File fs = LittleFS.open(req.path, "r");
          while (fs.available())
          {
              int byteRead = fs.read(buffer, sizeof(buffer));
              client.write(buffer, byteRead);
          }
          fs.close();
        }
        client.flush();
        break;
      }
    }
    client.stop();
  }
  delay(100);
}

void parse_http_request(http_request *req, const char* raw_req) {
  // 1. Parse method / path / params
  // 2. Parse body (if any)
  bool has_params = true;
  char *start, *end;
  size_t len;
  // Serial.printf("--> %s\n", raw_req);
  int i=0, idx = 0, lines = 0;
  int total_len = strlen(raw_req) + 1;
  for (i=0; i < total_len; i++) {
    if (raw_req[i] == '\0') {
      data[lines][idx] = '\0';
      lines++;
      break;
    } else if (raw_req[i] == '\r' && raw_req[i+1] == '\n') {
      data[lines][idx] = '\0';
      lines++;
      idx = 0;
      i++;
    } else {
      data[lines][idx++] = raw_req[i];
    }
  }
  // for (i = 0; i < lines; i++)
  // {
  //   Serial.printf("--> %s\n", data[i]);
  // }
  if (strstr(raw_req, "GET") != NULL) {
    req->method = Get;
  } else if (strstr(raw_req, "POST") != NULL) {
    req->method = Post;
  } else if (strstr(raw_req, "PUT") != NULL) {
    req->method = Put;
  } else if (strstr(raw_req, "DELETE") != NULL) {
    req->method = Delete;
  } else {
    req->method = Unknown;
  }

# ifdef DEBUG
  if (req->method != Unknown) {
    Serial.printf("Method : %d\n", req->method);
  }
# endif
  start = strstr(raw_req, " ");
  if (start != NULL) end = strstr(start, "?");
  else end = NULL;
  if (end == NULL && start != NULL) {
    end = strstr(start + 1, " ");
    has_params = false;
    Serial.println("No params");
  }
  if (start != NULL && end != NULL) {
    len = end - (start + 1);
    req->path = (char*)malloc(len + 1);
    strncpy(req->path, start + 1, len);
    req->path[len] = '\0';
#ifdef DEBUG
    Serial.printf("Path: %s\n", req->path);
#endif
  }
  // Get parm if any
  if (has_params) {
    start = end;
    if (start != NULL) end = strstr(start, " ");
    else end = NULL;
    if (start != NULL && end != NULL) {
      len = end - (start + 1);
      req->params = (char*)malloc(len + 1);
      strncpy(req->params, start + 1, len);
      req->params[len] = '\0';
#ifdef DEBUG
      Serial.printf("Params: %s\n", req->params);
#endif
    }
  }
  if (strcmp(data[lines-1], " ") != 0 && strcmp(data[lines-1], "") != 0){
    len = strlen(data[lines-1]);
    req->body = (char*)malloc(len + 1);
    strncpy(req->body, data[lines-1], len);
    req->body[len] = '\0';
#ifdef DEBUG
      Serial.printf("Body: %s\n", req->body);
#endif
  }
}

void read_file(char* cntx, const char* file) {
  File fs = LittleFS.open(file, "r");
  if (!fs) {
    Serial.println("Failed to open file");
    return;
  }
  size_t file_size = fs.size();
  while(fs.available()){
    strncpy(cntx, fs.readString().c_str(), file_size);
  }
  fs.close();
}

void create_header(char* header, char* path) {
  strcpy(header, "HTTP/1.1 200 OK\r\n");
# ifdef DEBUG
  // Add CORS headers
  strcat(header, "Access-Control-Allow-Origin: *\r\n");
  strcat(header, "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n");
  strcat(header, "Access-Control-Allow-Headers: Content-Type\r\n");
# endif
  if (path != NULL){
    if (strstr(path, ".html") != NULL) {
      strcat(header, "Content-Type: text/html\r\n");
    } else if (strstr(path, ".js") != NULL) {
      strcat(header, "Content-Type: application/javascript\r\n");
    } else if (strstr(path, ".css") != NULL) {
      strcat(header, "Content-Type: text/css\r\n");
    } else if (strstr(path, ".png") != NULL) {
      strcat(header, "Content-Type: image/png\r\n");
    } else if (strstr(path, ".jpg") != NULL) {
      strcat(header, "Content-Type: image/jpg\r\n");
    } else if (strstr(path, ".ico") != NULL) {
      strcat(header, "Content-Type: image/x-icon\r\n");
    } else if (strstr(path, ".svg") != NULL) {
      strcat(header, "Content-Type: image/svg+xml\r\n");
    } else if (strstr(path, ".json") != NULL || strstr(path, "/api") != NULL) {
      strcat(header, "Content-Type: application/json\r\n");
    } else {
      strcat(header, "Content-Type: text/html\r\n");
    }   
  }
  if (strstr(path, ".gz") != NULL) {
      strcat(header, "Content-Encoding: gzip\r\n");
  }
  strcat(header, "\r\n");
};

void doAPIProcess(char* result, http_method method, char* path, char* params, char* body) {
  Serial.printf("API Process Path: %s\n", path);
  if (strstr(path, "/api/getAPList") != NULL) {
# ifdef DEBUG
    Serial.println("API List Called");
# endif
    int i;
    int8_t nums = WiFi.scanNetworks(false, true);
    cJSON *mainNode = cJSON_CreateObject();
    cJSON *BufferArray = cJSON_CreateArray();
    for (i=0; i< nums; i++){
      if (i >= MAXLIST) break;
      cJSON *bufferObj = cJSON_CreateObject();
      cJSON_AddNumberToObject(bufferObj, "ID", i + 1);
      cJSON_AddStringToObject(bufferObj, "SSID", WiFi.SSID(i).c_str());
      cJSON_AddNumberToObject(bufferObj, "RSSI", WiFi.RSSI(i));
      cJSON_AddStringToObject(bufferObj, "BSSID", WiFi.BSSIDstr(i).c_str());
      cJSON_AddNumberToObject(bufferObj, "Channel", WiFi.channel(i));
      cJSON_AddStringToObject(bufferObj, "EncryptionType", String(WiFi.encryptionType(i)).c_str());
      cJSON_AddItemToArray(BufferArray, cJSON_Duplicate(bufferObj, 1));
      cJSON_Delete(bufferObj);
    }
    cJSON_AddItemToObject(mainNode, "APList", BufferArray);
    char* json_str = cJSON_Print(mainNode);
    if (json_str != NULL) {
#ifdef DEBUG
      Serial.printf("%s\n", json_str);
#endif
      strncpy(result, json_str, 4095);
      result[4095] = '\0';
      free(json_str);
    }
    cJSON_Delete(mainNode);
    WiFi.scanDelete();
  } else if (strstr(path, "/api/connect") != NULL && method == Post) {
    cJSON *json = cJSON_Parse(body);
    if (json != NULL) {
      cJSON *ssidNode = cJSON_GetObjectItem(json, "SSID");
      cJSON *pskNode = cJSON_GetObjectItem(json, "PSK");
      if (ssidNode != NULL && pskNode != NULL) {
        const char* ssid = ssidNode->valuestring;
        const char* psk = pskNode->valuestring;
        WiFi.begin(ssid, psk);
        while (WiFi.status() != WL_CONNECTED && WiFi.status() != WL_CONNECT_FAILED)
        {
          Serial.print(".");
          delay(1000);
        }
        if (WiFi.status() == WL_CONNECTED)
        {
          sprintf(result, "{\"status\":\"OK\",\"ip\":\"%s\"}", WiFi.localIP().toString().c_str());
          File fs = LittleFS.open(APRECORD_FILE, "w");
          if (fs) {
            cJSON *record = cJSON_CreateObject();
            cJSON_AddStringToObject(record, "SSID", ssid);
            cJSON_AddStringToObject(record, "PSK", psk);
            char* record_str = cJSON_Print(record);
            fs.write((uint8_t*)record_str, strlen(record_str));
            fs.close();
            cJSON_Delete(record);
            free(record_str);
          } else {
            Serial.println("Failed to open AP record file for writing");
          }
        } else {
          sprintf(result, "{\"status\":\"error\",\"message\":\"Connection Failed\"}");
        }
# ifdef DEBUG
        Serial.printf("Connecting to SSID: %s with PSK: %s\n", ssid, psk);
# endif
      } else {
        strcpy(result, "{\"status\":\"error\",\"message\":\"SSID or PSK missing\"}");
      }
      cJSON_Delete(json);
    }
  } else if (strstr(path, "/api/setUserInfo") != NULL && method == Post) {
    File fs = LittleFS.open(USER_INFO_FILE, "w");
    if (fs) {
      fs.write(body, strlen(body));
      fs.close();
      strcpy(result, "{\"status\":\"OK\",\"message\":\"UserID set successfully\"}");
    } else {
      fs.close();
      strcpy(result, "{\"status\":\"error\",\"message\":\"Failed to save user info\"}");
    }
  } 
}