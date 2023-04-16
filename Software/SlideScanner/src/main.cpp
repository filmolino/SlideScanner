#include <Arduino.h>
#include <M5Stack.h>
#include "SPIFFS.h"
#include "ir_release.h"
#include "control.h"
#include "config.h"
#include "project.h"

// Display
Display display;

// Config
Config settings(CONFIG_FILE_NAME);

// Camera releases
IrRelease ir_release;
CableRelease cable_release;

// Control
Control control;
QueueHandle_t control_evt_queue = NULL;

// Cable Release
QueueHandle_t cable_release_evt_queue = NULL;

// Wifi
WiFiManager wifimanager;

// Web
Webmanager webmanager;

void control_task(void *param) {
  control_evt_t evt;

  Serial.println("control_task created");

  while (1) {
    if (xQueueReceive(control_evt_queue, (void*) &evt, 10)) {      
      control.sm(evt);
    }
  }
}


void cable_release_task(void *param) {
  cable_release_evt_t evt;

  Serial.println("cable_release_task created");

  while (1) {
    if (xQueueReceive(cable_release_evt_queue, (void*) &evt, 10)) {
      Serial.println("cable_release_task QUEUE event"); 
      cable_release.sm(evt);
    }
  }
}



void display_task(void *param) {
  Serial.println("display_task created");

  while (1) {
    display.update_header();
    vTaskDelay(2000/portTICK_PERIOD_MS);
  }
}


void setup() {
  Serial.begin(115200);

  // M5Stack 
  M5.begin(); 
  M5.Power.begin();
  if (M5.Power.canControl()) {
    Serial.println("Power canControl OK");
  } else {
    Serial.println("Power canControl ERROR");
  }

  // Display
  display.begin();
  xTaskCreate(display_task, "display_task", 2048, NULL, 10, NULL);

  // SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  else {
    Serial.println("SPIFFS init ok");
  }

  // Get config data
  settings.begin();
  display.update_main_scan(0, settings.get_slides_to_scan());
  
  // IR Release
  uint8_t code = settings.get_ir_selected_code();
  uint8_t divider = settings.get_ir_divider(code);
  uint32_t freq = settings.get_ir_freq(code);
  ir_release.begin(GPIO_IR_RELEASE, RMT_CHANNEL_0, divider, freq);

  // Cable Release
  cable_release.begin(GPIO_CABLE_RELEASE_SHUTTER, GPIO_CABLE_RELEASE_FOCUS);
  settings.set_cable_release();
  cable_release_evt_queue = xQueueCreate(10, sizeof(cable_release_evt_t));
  xTaskCreate(cable_release_task, "cable_release_task", 2048, NULL, 10, NULL);
  
  // Control State Machine
  control.begin(GPIO_RELAY_LAST, GPIO_RELAY_NEXT);
  
  // Control Task
  control_evt_queue = xQueueCreate(10, sizeof(control_evt_t));
  xTaskCreate(control_task, "control_task", 2048, NULL, 10, NULL);   

  // WiFi Router
  wifimanager.begin();

  // WebServer
  webmanager.begin();
}

void loop() {
  static long lastBtnMillis = 0;
  long currentMillis = millis();

  // Buttons
  if (currentMillis - lastBtnMillis > BTN_PERIOD) {
    lastBtnMillis = millis();
    display.buttons();
  }

  // WiFiManager
  wifimanager.loop();
}
