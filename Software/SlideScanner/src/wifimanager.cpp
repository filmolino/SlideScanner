#include <Arduino.h>
#include <M5Stack.h>
#include <WiFi.h>
#include "project.h"

extern Config settings;
extern Webmanager webmanager;

static char *buf;
static char *ap_entry;

void WiFiManager::begin()
{
  // WiFi
  wset = settings.get_wifi();

  WiFi.setHostname(CONFIG_SCANNER_NAME);

  if (wset.sta) {
    WiFi.mode(WIFI_MODE_APSTA);
    if (!start_STA()) {
      start_AP();     // if not connected to an AP start own AP
    }
  }
  else
  {
    WiFi.mode(WIFI_MODE_AP);
    start_AP();
  }
}

String WiFiManager::getOwnIP()
{ 
  switch (wifi_mode){
    case WIFIMANAGER_MODE_AP:
      return WiFi.softAPIP().toString();
      break;

    case WIFIMANAGER_MODE_STA:
      return WiFi.localIP().toString();
      break;

    case WIFIMANAGER_MODE_NONE:
    default:
      return "Not connected";
      break;
  }

  return WiFi.softAPIP().toString();
}

wifimanager_mode_t WiFiManager::getMode() {
  return wifi_mode;
}

void WiFiManager::start_AP() {
  char ap_ssid[CONFIG_SCANNER_AP_PARA_MAX_LENGTH];
  char ap_password[CONFIG_SCANNER_AP_PARA_MAX_LENGTH];

  Serial.print("Starting AP...");

  wset.ap_ssid.toCharArray((char *)&ap_ssid, CONFIG_SCANNER_AP_PARA_MAX_LENGTH, 0);
  wset.ap_password.toCharArray((char *)&ap_password, CONFIG_SCANNER_AP_PARA_MAX_LENGTH, 0);

  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.softAP(ap_ssid, ap_password);

  wifi_mode = WIFIMANAGER_MODE_AP;

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  Serial.print("ssid ");
  Serial.println(ap_ssid);
  Serial.print("password ");
  Serial.println(ap_password);
}

bool WiFiManager::start_STA()
{
  char sta_ssid[CONFIG_SCANNER_STA_PARA_MAX_LENGTH];
  char sta_password[CONFIG_SCANNER_STA_PARA_MAX_LENGTH];

  Serial.print("Starting STA...");

  wset.sta_ssid.toCharArray((char *)&sta_ssid, CONFIG_SCANNER_AP_PARA_MAX_LENGTH, 0);
  wset.sta_password.toCharArray((char *)&sta_password, CONFIG_SCANNER_AP_PARA_MAX_LENGTH, 0);

  WiFi.begin(sta_ssid, sta_password);

  long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if ((millis() - startTime) > WIFI_MANAGER_STA_CONNECTION_TIMEOUT_MS) {
      WiFi.disconnect();      // Connection to AP failed => start in AP mode
      return false;
    }
  }
  wifi_mode = WIFIMANAGER_MODE_STA;
  return true;
}

void WiFiManager::scan_networks()
{
  Serial.println("Disconnecting...");
  //WiFi.disconnect();
  delay(100);
  Serial.println("Start WiFi scan...");
  WiFi.scanNetworks(true, false, true);    // Start async network scan
  scan_active = true;
}

void WiFiManager::loop() {
  if (scan_active) {
    int16_t nets = WiFi.scanComplete();
    if (nets >= 0) {
      Serial.println("scanNetworks OK");
      ap_entry = (char *) calloc(64, 1);
      buf = (char *) calloc(WEBMANAGER_WS_BUFFER_SIZE, 1);
      scan_active = false;
      if (nets > WIFI_MANAGER_SCAN_MAX_AP) {
        nets = WIFI_MANAGER_SCAN_MAX_AP;
      }

      sprintf((char *) buf, "NetworksFound,%u", nets);
      for (uint8_t n = 0; n < nets; n++) {
        Serial.printf("AP ssid:%s  |  rssi:%d\n\r", WiFi.SSID(n).c_str(), WiFi.RSSI(n));
        sprintf(ap_entry, ",%s,%d", WiFi.SSID(n).c_str(), WiFi.RSSI(n));
        strcat((char *) buf, ap_entry);   
      }
      webmanager.ws_async_send(buf);
    }
  }
}
