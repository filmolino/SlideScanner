#include <Arduino.h>
#include <M5Stack.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <ESPmDNS.h>
#include "SPIFFS.h"
#include "project.h"

using namespace std::placeholders;

extern Control control;
extern Config settings;
extern WiFiManager wifimanager;
extern QueueHandle_t control_evt_queue;

static char *buf;

// Webserver
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
 
void Webmanager::begin() {
  // mDNS
  if(!MDNS.begin(CONFIG_SCANNER_NAME)) {
    Serial.println("Error starting mDNS");
    return;
  }

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("HTTP /index.hmtl");
    request->send(SPIFFS, "/index.html", String(), false);
  });
  
  // Route to load style.css file
  server.on("/pico.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/pico.min.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", String(), false);
  });
  
  // Route to set GPIO to LOW
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){   
    request->send(SPIFFS, "/favicon.ico", String(), false);
  });

  // Route for NotFound
  server.onNotFound(std::bind(&Webmanager::notFound, this, _1));

  // Init websocket
  ws.onEvent(std::bind(&Webmanager::onWsEvent, this, _1, _2, _3, _4, _5, _6));
  server.addHandler(&ws);

  // Over the Air Updates (OTA)
  AsyncElegantOTA.begin(&server);

  // Start server
  server.begin();
}

void Webmanager::onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  AwsFrameInfo * info = (AwsFrameInfo*)arg;

  buf = (char *) calloc(WEBMANAGER_WS_BUFFER_SIZE, 1);
  
  Serial.printf("WebSocket EVENT");
  switch(type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      ws_connected = true;
      ws_client_id = client->id();
      break;

    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      ws_connected = false;
      break;

    case WS_EVT_DATA:
      // Data packet
      Serial.printf("WebSocket data");

      if(info->final && info->index == 0 && info->len == len){
        //the whole message is in a single frame and we got all of it's data
        Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
        if(info->opcode == WS_TEXT){
          data[len] = 0;
          Serial.printf("%s\n", (char*)data);
          if (strcmp((char*)data,"btnStartStop") == 0) {
            Serial.println("[WS] btnStartStop");
            if (control.is_scanning()) {
              send_control_event(CONTROL_EVT_STOP);
              sprintf(buf, "AutoScan,0");               // Send scanning state
            } else {
              send_control_event(CONTROL_EVT_START);
              sprintf(buf, "AutoScan,1");               // Send scanning state
            }
          } 
          else if (strcmp((char*)data,"btnLast") == 0) {
            Serial.println("[WS] btnLast");
            send_control_event(CONTROL_EVT_LAST);
            sprintf(buf, WEBMANAGER_WS_OK);               // Websocket Answer
          }
          else if (strcmp((char*)data,"btnNext") == 0) {
            Serial.println("[WS] btnNext");
            send_control_event(CONTROL_EVT_NEXT);
            sprintf(buf, WEBMANAGER_WS_OK);               // Websocket Answer
          }
          else if (strcmp((char*)data,"btnScan") == 0) {
            Serial.println("[WS] btnScan");
            send_control_event(CONTROL_EVT_SCAN);
            sprintf(buf, WEBMANAGER_WS_OK);               // Websocket Answer
          }
          else if (strcmp((char*)data,"getCurrentSlide") == 0) {
            Serial.println("[WS] getCurrentSlide");
            control.get_current_slide(buf);
          }
          else if (strncmp((char*)data,"setCurrentSlide", 15) == 0) {
            Serial.println("[WS] setCurrentSlide");
            send_control_event(CONTROL_EVT_RESET_CURRENT_SLIDES);
          }
          else if (strcmp((char*)data,"getWiFiCfg") == 0) {
            Serial.println("[WS] getWiFiCfg");
            settings.get_csv_cfg_wifi(buf);
          }
          else if (strncmp((char*)data,"setWiFiCfg", 10) == 0) {
            Serial.println("[WS] setWiFiCfg");
            settings.set_csv_cfg_wifi((char *) data);
            sprintf(buf, WEBMANAGER_WS_OK);               // Websocket Answer
          }
          else if (strcmp((char*)data,"getScanCfg") == 0) {
            Serial.println("[WS] getScanCfg");
            settings.get_csv_cfg_scanner(buf);
          }
          else if (strncmp((char*)data,"setScanCfg", 10) == 0) {
            Serial.println("[WS] setScanCfg");
            settings.set_csv_cfg_scanner((char *) data);
            sprintf(buf, WEBMANAGER_WS_OK);               // Websocket Answer
            control.update_max_slides();
          }
          else if (strcmp((char*)data,"getCableRelNames") == 0) {
            Serial.println("[WS] getCableRelNames");
            settings.get_csv_cable_release_names(buf, WEBMANAGER_WS_BUFFER_SIZE);
            Serial.println("[WS] AFTER getCableRelNames");
          }
          else if (strcmp((char*)data,"getIrRelNames") == 0) {
            Serial.println("[WS] getIrRelNames");
            settings.get_csv_ir_release_names(buf, WEBMANAGER_WS_BUFFER_SIZE);
          }
          else if (strcmp((char*)data,"findNetworks") == 0) {
            Serial.println("[WS] findNetworks");
            wifimanager.scan_networks();
            sprintf(buf, WEBMANAGER_WS_OK);               // Websocket Answer 
          }
          else if (strcmp((char*)data,"restartSystem") == 0) {
            Serial.println("[WS] restartSystem");
            M5.Power.reset();
          } 
          else if (strcmp((char*)data,"powerOff") == 0) {
            Serial.println("[WS] powerOff");
            M5.Power.powerOFF();
          } 
          else if (strcmp((char*)data,"deleteConfig") == 0) {
            Serial.println("[WS] deleteConfig");
            settings.remove();
            sprintf(buf, WEBMANAGER_WS_OK);               // Websocket Answer
          }
          else if (strcmp((char*)data,"saveConfig") == 0) {
            Serial.println("[WS] saveConfig");            
            settings.write();
            sprintf(buf, WEBMANAGER_WS_OK);               // Websocket Answer
          }
          else {
            sprintf(buf, WEBMANAGER_WS_ERROR);               // Websocket Answer
          }
        } else {
          for(size_t i=0; i < info->len; i++){
            Serial.printf("%02x ", data[i]);
          }
          Serial.printf("\n");
        }
        if(info->opcode == WS_TEXT) {
          client->text(buf);
        }
      } else {
        // Multiple frames 
        Serial.printf("WebSocket Multiple Frames data len=%u\n", len); //arg, data, len);
      }
      break;

    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;

  }
  free(buf);
}

// NOT FOUND
void Webmanager::notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void Webmanager::send_control_event(control_evt_t evt) {
  xQueueSend(control_evt_queue, (void *) &evt, 10);
}

bool Webmanager::isWSConnected() {
  return ws_connected;
}

void Webmanager::ws_async_send(char *payload) {
  if (ws_connected) {
    ws.text(ws_client_id, payload);
  }
}
