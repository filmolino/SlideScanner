#include <Arduino.h>
#include <M5Stack.h>
#include "SPIFFS.h"
#include "cable_release.h"
#include "ir_release.h"
#include "Config.h"

// Classes
extern IrRelease ir_release;
extern CableRelease cable_release;

Config::Config(String filename) {
  this->filename = filename;
}


void Config::begin() {
  if (!read()) {
    Serial.println("config file not found...");
    load_defaults();
    if (!write()) {
      Serial.println("Write Error...");
      return;
    }
    if (!read()) {
      Serial.println("Reread error...");
      return;
    }
  }
  Serial.println("Config init OK");
}

size_t Config::read() {
  File file = SPIFFS.open(filename, "rb");
  size_t size = 0;

  if(file){
    size = file.read((byte *) &settings, sizeof(config_scanner_t));
    file.close();
  }
  return size;
}

size_t Config::write() {
  File file = SPIFFS.open(filename, "w");
  size_t size = 0;

  if(file){
    size = file.write((uint8_t *) &settings, sizeof(config_scanner_t));
    file.close();
  }
  return size;
}

bool Config::remove() {
  return SPIFFS.remove(filename);
}

void Config::load_defaults() {
  uint8_t n;
  // WiFi settings
  settings.wifi.ap_ssid = CONFIG_SCANNER_AP_SSID;
  settings.wifi.ap_password = CONFIG_SCANNER_AP_PASSWORD;
  settings.wifi.sta_ssid = CONFIG_SCANNER_STA_SSID;
  settings.wifi.sta_password = CONFIG_SCANNER_STA_PASSWORD;
  settings.wifi.ap = CONFIG_SCANNER_AP_ENABLED;
  settings.wifi.sta = CONFIG_SCANNER_STA_ENABLED;
  // Control settings
  settings.control.delay_slide_change = CONFIG_DEFAULT_SLIDE_CHANGE_TIME;
  settings.control.time_slide_change_pulse = CONFIG_DEFAULT_SLIDE_CHANGE_PULSE_TIME;
  settings.control.delay_slide_scan = CONFIG_DEFAULT_SLIDE_SCAN_TIME;
  settings.control.slides_to_scan = CONFIG_DEFAULT_SLIDES_TO_SCAN;
  settings.control.ir = CONFIG_DEFAULT_IR_ENABLED;
  settings.control.ir_selected_code = CONFIG_DEFAULT_IR_SELECTED_CODE;
  settings.control.cable = CONFIG_DEFAULT_CABLE_ENABLED;
  settings.control.cable_selected_code = CONFIG_DEFAULT_CABLE_SELECTED_CODE;
  
  // IR Release settings
  for (n = 0; n < (uint8_t) ir_release_code_empty; n++) {
    settings.release.ir_codes[n] = ir_release.getCode((ir_release_codes_t) n);
  }
 
  for (; n < IR_RELEASE_CODES_SIZE; n++) {
    settings.release.ir_codes[n] = ir_release.getCode(ir_release_code_empty);
  }
 
  // Cable Release settings
  for (n = 0; n < (uint8_t) cable_release_code_empty; n++) {
    settings.release.cable_codes[n] = cable_release.getCode((cable_release_codes_t) n);
  }
 
  for (; n < CABLE_RELEASE_CODES_SIZE; n++) {
    settings.release.cable_codes[n] = cable_release.getCode(cable_release_code_empty);
  }

}

config_wifi_t Config::get_wifi() {
  return settings.wifi;
}

void Config::set_wifi(config_wifi_t data) {
  settings.wifi = data;
}

uint32_t Config::get_delay_slide_change() {
  return settings.control.delay_slide_change;
}

uint32_t Config::get_delay_slide_scan() {
  Serial.printf("config.control.delay_slide_scan=%u\n\r", settings.control.delay_slide_scan);
  return settings.control.delay_slide_scan;
}

uint8_t Config::get_slides_to_scan() {
  return settings.control.slides_to_scan;
}

uint32_t Config::get_time_slide_change_pulse() {
  return settings.control.time_slide_change_pulse;
}

bool Config::get_ir_active() {
  return settings.control.ir;
}

bool Config::get_cable_active() {
  return settings.control.cable;
}

uint8_t Config::get_cable_selected_code() {
  return settings.control.cable_selected_code;
}

String Config::get_cable_name(uint8_t code) {
  if ((code > 0) && (code <= IR_RELEASE_CODES_SIZE)) {
    code--;   // Array starts at [0]
  } else {
    code = 0;
  }
  return settings.release.cable_codes[code]->name;
}

uint8_t Config::get_ir_divider(uint8_t code) {
  if ((code > 0) && (code <= IR_RELEASE_CODES_SIZE)) {
    code--;   // Array starts at [0]
  } else {
    code = 0;
  }
  return settings.release.ir_codes[code]->divider;
}

uint32_t Config::get_ir_freq(uint8_t code) {
  if ((code > 0) && (code <= IR_RELEASE_CODES_SIZE)) {
    code--;   // Array starts at [0]
  } else {
    code = 0;
  }
  return settings.release.ir_codes[code]->freq;
}

uint8_t Config::get_ir_selected_code() {
  return settings.control.ir_selected_code;
}

String Config::get_ir_name(uint8_t code) {
  if ((code > 0) && (code <= IR_RELEASE_CODES_SIZE)) {
    code--;   // Array starts at [0]
  } else {
    code = 0;
  }
  return settings.release.ir_codes[code]->name;
}

rmt_item32_t *Config::get_ir_seq() {
  uint8_t code = settings.control.ir_selected_code;      // Attention code is in the range from 01...IR_RELEASE_CODES_SIZE

  if ((code > 0) && (code <= IR_RELEASE_CODES_SIZE)) {
    code--;   // Array starts at [0]
  } else {
    code = 0;
  }
  Serial.printf("IR send code=%u name=%s\n\r", code, settings.release.ir_codes[code]->name);
  return (rmt_item32_t *) (&settings.release.ir_codes[code]->ir_seq[0]);
}

void Config::set_cable_release() {
  uint8_t code = settings.control.cable_selected_code;      // Attention code is in the range from 01...CABLE_RELEASE_CODES_SIZE
  static uint8_t *testPtr;


  if ((code > 0) && (code < CABLE_RELEASE_CODES_SIZE)) {
    code--;   // Array starts at [0]
  } else {
    code = 0;
  }

  cable_release.set_seq((uint8_t *) &settings.release.cable_codes[code]->cable_seq[0]);
}

void Config::get_csv_cfg_wifi(char *csv) {
  sprintf(csv, "WiFiCfg,%s,%s,%s,%s,%d,%d", 
          settings.wifi.ap_ssid, 
          settings.wifi.ap_password,
          settings.wifi.sta_ssid, 
          settings.wifi.sta_password,
          settings.wifi.ap,
          settings.wifi.sta);
}

void Config::set_csv_cfg_wifi(char *csv) {
  char *data;

  data = strtok(csv, ",");    // CMD not used here
  data = strtok(NULL, ",");
  settings.wifi.ap_ssid = data;
  data = strtok(NULL, ",");
  settings.wifi.ap_password = data;
  data = strtok(NULL, ",");
  settings.wifi.sta_ssid = data;
  data = strtok(NULL, ",");
  settings.wifi.sta_password = data; 
  data = strtok(NULL, ",");
  settings.wifi.ap = (strcmp(data, "1") == 0) ? true : false;
  data = strtok(NULL, ",");
  settings.wifi.sta = (strcmp(data, "1") == 0) ? true : false;

  Serial.printf("ap_ssid = %s\n\r", settings.wifi.ap_ssid);
  Serial.printf("ap_ssid = %s\n\r", settings.wifi.ap_password);
  Serial.printf("sta_ssid = %s\n\r", settings.wifi.sta_ssid);
  Serial.printf("sta_password = %s\n\r", settings.wifi.sta_password);
  Serial.printf("ap = %u\n\r", settings.wifi.ap);
  Serial.printf("sta = %u\n\r", settings.wifi.sta);
}

void Config::get_csv_cfg_scanner(char *csv) {
  sprintf(csv, "ScannerCfg,%u,%u,%u,%u,%d,%u,%d,%u", 
          settings.control.delay_slide_change,
          settings.control.time_slide_change_pulse,
          settings.control.delay_slide_scan, 
          settings.control.slides_to_scan,
          settings.control.ir,
          settings.control.ir_selected_code,
          settings.control.cable,
          settings.control.cable_selected_code);
}

void Config::set_csv_cfg_scanner(char *csv) {
  char *data;

  data = strtok(csv, ",");    // CMD not used here
  data = strtok(NULL, ",");
  settings.control.delay_slide_change = strtoul(data, NULL, 10);
  data = strtok(NULL, ",");
  settings.control.time_slide_change_pulse = strtoul(data, NULL, 10);
  data = strtok(NULL, ",");
  settings.control.delay_slide_scan = strtoul(data, NULL, 10);
  data = strtok(NULL, ",");
  settings.control.slides_to_scan = strtoul(data, NULL, 10);
  data = strtok(NULL, ",");
  settings.control.ir = (strcmp(data, "1") == 0) ? true : false;
  data = strtok(NULL, ",");
  settings.control.ir_selected_code = strtoul(data, NULL, 10);
  data = strtok(NULL, ",");
  settings.control.cable = (strcmp(data, "1") == 0) ? true : false;
  data = strtok(NULL, ",");
  settings.control.cable_selected_code = strtoul(data, NULL, 10);

  Serial.printf("delay_slide_change = %u\n\r", settings.control.delay_slide_change);
  Serial.printf("time_slide_change_pulse = %u\n\r", settings.control.time_slide_change_pulse);
  Serial.printf("delay_slide_scan = %u\n\r", settings.control.delay_slide_scan);
  Serial.printf("slides_to_scan = %u\n\r", settings.control.slides_to_scan);
  Serial.printf("ir = %u\n\r", settings.control.ir);
  Serial.printf("ir_selected_code = %u\n\r", settings.control.ir_selected_code);
  Serial.printf("cable = %u\n\r", settings.control.cable);
  Serial.printf("cable_selected_code = %u\n\r", settings.control.cable_selected_code);
}

void Config::get_csv_cable_release_names(char *csv, uint32_t bufferSize) {
  String tempStr = "getCableRelNames";

  for (uint8_t n = 0; n < CABLE_RELEASE_CODES_SIZE ; n++) {
    tempStr += "," + settings.release.cable_codes[n]->name;
  }
  tempStr.toCharArray(csv, bufferSize, 0);
}

void Config::get_csv_ir_release_names(char *csv, uint32_t bufferSize) {
  String tempStr = "getIrRelNames";
  
  for (uint8_t n = 0; n < IR_RELEASE_CODES_SIZE ; n++) {
    tempStr += "," + settings.release.ir_codes[n]->name;
  }
  tempStr.toCharArray(csv, bufferSize, 0);
}


