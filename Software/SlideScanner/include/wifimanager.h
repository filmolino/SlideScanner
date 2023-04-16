#ifndef __WIFI_MANAGER_H
#define __WIFI_MANAGER_H

#define WIFI_MANAGER_STA_MAXIMUM_RETRY          5 
#define WIFI_MANAGER_SCAN_MAX_AP                15
#define WIFI_MANAGER_STA_CONNECTION_TIMEOUT_MS  5000

typedef enum {
  WIFIMANAGER_MODE_NONE,
  WIFIMANAGER_MODE_AP,
  WIFIMANAGER_MODE_STA,
} wifimanager_mode_t;

class WiFiManager {
  public:
    void begin();
    String getOwnIP();
    wifimanager_mode_t getMode();
    void scan_networks();
    void start_AP();
    bool start_STA();
    void loop();

  private:
   config_wifi_t wset;
   bool scan_active = false;
   wifimanager_mode_t wifi_mode = WIFIMANAGER_MODE_NONE;
};

#endif
