#ifndef __WEB_MANAGER_H
#define __WEB_MANAGER_H

#include "ESPAsyncWebServer.h"

#define WEBMANAGER_WS_OK                    "OK,"
#define WEBMANAGER_WS_ERROR                 "ERROR,"

#define WEBMANAGER_WS_BUFFER_SIZE           1024

class Webmanager {
  public:
    void begin();
    bool isWSConnected();
    void ws_async_send(char *payload);
    
  private:
    void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
    void notFound(AsyncWebServerRequest *request);
    void send_control_event(control_evt_t evt);

    bool ws_connected = false;
    uint32_t ws_client_id = 0;
};

#endif
