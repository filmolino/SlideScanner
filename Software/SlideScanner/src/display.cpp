#include <Arduino.h>
#include <M5Stack.h>
#include "project.h"

using namespace std::placeholders;

// Classes
extern Config settings;
extern Control control;
extern WiFiManager wifimanager;
extern Webmanager webmanager;

// Queues
extern QueueHandle_t control_evt_queue;

void Display::begin() {
  sprHeader.createSprite(SPRITE_HEADER_WIDTH, SPRITE_HEADER_HEIGHT);
  sprMain.createSprite(SPRITE_MAIN_WIDTH, SPRITE_MAIN_HEIGHT);
  sprFooter.createSprite(SPRITE_FOOTER_WIDTH, SPRITE_FOOTER_HEIGHT);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextDatum(MC_DATUM);
  M5.Lcd.setSwapBytes(true);
  M5.Lcd.pushImage(35, 34, 250, 172, img_filmolino);
  delay(DISPLAY_SHOW_LOGO_TIME_MS);
  M5.Lcd.fillScreen(BLACK);
  update_footer_scan(DISPLAY_ACTIVE_NONE);
}

void Display::update_header(void) {
  static display_battery_state_t batt_state = DISPLAY_BATTERY_NIL;
  display_battery_state_t batt_state_next = DISPLAY_BATTERY_NIL;
  static bool ws_state = false;
  bool ws_state_next = webmanager.isWSConnected();

  if (M5.Power.isCharging()) {
    batt_state_next = DISPLAY_BATTERY_CHARGING;
  } else {
    switch (M5.Power.getBatteryLevel()) {
      case 100: batt_state_next = DISPLAY_BATTERY_100; break;
      case 75: batt_state_next = DISPLAY_BATTERY_75; break;
      case 50: batt_state_next = DISPLAY_BATTERY_50; break;
      case 25: batt_state_next = DISPLAY_BATTERY_25; break;
      case 0: batt_state_next = DISPLAY_BATTERY_100; break;
      default: batt_state_next = DISPLAY_BATTERY_ERROR; break;
    }
  } 

  if ((batt_state != batt_state_next) || 
      (batt_state == batt_state_next == DISPLAY_BATTERY_NIL) ||
      (ws_state != ws_state_next)) {
    batt_state = batt_state_next;

    sprHeader.fillSprite(BLACK);
    sprHeader.setTextColor(WHITE);
    sprHeader.setTextDatum(MC_DATUM);
    sprHeader.pushImage(18, 0, 180, 40, img_slide_scanner_small);

    switch (batt_state) {
      case DISPLAY_BATTERY_CHARGING:  sprHeader.pushImage(264, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_battery_charging); break;
      case DISPLAY_BATTERY_100:       sprHeader.pushImage(264, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_battery_full); break;
      case DISPLAY_BATTERY_75:        sprHeader.pushImage(264, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_battery_full); break;
      case DISPLAY_BATTERY_50:        sprHeader.pushImage(264, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_battery_half); break;
      case DISPLAY_BATTERY_25:        sprHeader.pushImage(264, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_battery_half); break;
      case DISPLAY_BATTERY_0:         sprHeader.pushImage(264, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_battery); break;
      case DISPLAY_BATTERY_NIL:
      default: break;
    }

    if (ws_state_next) {
      sprHeader.pushImage(216, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_link_45deg);
    }
    ws_state = ws_state_next;
    sprHeader.pushSprite(0,0);
  }
}

void Display::update_main_scan(uint8_t current_slide, uint8_t slides_to_scan) {
  String str = String(current_slide) + "/" + String(slides_to_scan);

  sprMain.fillSprite(BLACK);
  sprMain.setTextColor(WHITE);
  sprMain.setTextDatum(MC_DATUM);  
  sprMain.setFreeFont(&FreeMono24pt7b);
  sprMain.drawString(str, 160, 80);
  sprMain.pushSprite(0, 40);
}

void Display::update_footer_scan(display_header_active_t active) {
  sprFooter.fillSprite(BLACK);
  sprFooter.setTextColor(WHITE);
  sprFooter.setTextDatum(TL_DATUM);

  if (active == DISPLAY_ACTIVE_LAST) {
    sprFooter.pushImage( 24, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_arrow_left_circle_fill_active);
  } else {
    sprFooter.pushImage( 24, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_arrow_left_circle_fill);
  }

  if (active == DISPLAY_ACTIVE_MENU) {
    sprFooter.pushImage( 72, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_list_active);
  } else {
    sprFooter.pushImage( 72, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_list);
  }
  
  if (active == DISPLAY_ACTIVE_NEXT) {
    sprFooter.pushImage(120, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_arrow_right_circle_fill_active);
  } else {
    sprFooter.pushImage(120, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_arrow_right_circle_fill);
  } 

  sprFooter.pushImage(168, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_x_circle_fill);

  if (active == DISPLAY_ACTIVE_SCAN) {
    sprFooter.pushImage(216, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_camera_fill_active);
  } else {
    sprFooter.pushImage(216, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_camera_fill);
  } 

  if (active == DISPLAY_ACTIVE_AUTO) {
    sprFooter.pushImage(264, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_play_circle_fill_active);
  } else {
    sprFooter.pushImage(264, LCD_POS_HEADER_FOOTER_ICON_Y, 32, 32, img_play_circle_fill);
  } 

  sprFooter.pushSprite(0, 200);
}

void Display::update_main_config() {
  String str = "";
  uint8_t posY = 10;
  sprMain.fillSprite(BLACK);
  sprMain.setTextColor(WHITE);
  sprMain.setTextDatum(TL_DATUM);

  switch (wifimanager.getMode()) {
    case WIFIMANAGER_MODE_AP:
      str = "WiFi : Access Point";
      sprMain.drawString(str, 0, posY, 2);
      posY += LCD_MENU_LINE_SPACING;
      str = "SSID : " + settings.get_wifi().ap_ssid;
      sprMain.drawString(str, 0, posY, 2);
      posY += LCD_MENU_LINE_SPACING;
      str = "Password : " + settings.get_wifi().ap_password;
      sprMain.drawString(str, 0, posY, 2);
      posY += LCD_MENU_LINE_SPACING;
      break;

    case WIFIMANAGER_MODE_STA:
      str = "WiFi : Station";
      sprMain.drawString(str, 0, posY, 2);
      posY += LCD_MENU_LINE_SPACING;  
      break;

    case WIFIMANAGER_MODE_NONE:
    default:
      str = "WiFi : UNDEFINED";
      sprMain.drawString(str, 0, posY, 2);
      posY += LCD_MENU_LINE_SPACING;
      break;
  }

  str = "IP : " + wifimanager.getOwnIP();
  sprMain.drawString(str, 0, posY, 2);
  posY += LCD_MENU_LINE_SPACING;

  str = "IR : " + settings.get_ir_name(settings.get_ir_selected_code());
  sprMain.drawString(str, 0, posY, 2);
  posY += LCD_MENU_LINE_SPACING;
  
  str = "Cable : " + settings.get_cable_name(settings.get_cable_selected_code());
  sprMain.drawString(str, 0, posY, 2);
  posY += LCD_MENU_LINE_SPACING;
  
  str = "Slides to scan : " + String(settings.get_slides_to_scan());
  sprMain.drawString(str, 0, posY, 2);

  sprMain.pushSprite(0, 40);
}

void Display::update_footer_config() {
  sprFooter.fillSprite(BLACK);
  sprFooter.setTextColor(RED);
  sprFooter.setTextDatum(TC_DATUM);
  sprFooter.drawString("Defaults", 68, LCD_POS_HEADER_FOOTER_TEXT_Y, 2);
  sprFooter.drawString("Reboot", 160, LCD_POS_HEADER_FOOTER_TEXT_Y, 2);
  sprFooter.setTextColor(WHITE);
  sprFooter.drawString("Exit", 252, LCD_POS_HEADER_FOOTER_TEXT_Y, 2);
  sprFooter.pushSprite(0, 200);
}

void Display::buttons() {
  static bool buttonA_short = false;
  static bool buttonA_long = false;  
  static bool buttonB_short = false;
  static bool buttonB_long = false;
  static bool buttonC_short = false;
  static bool buttonC_long = false;
  
  M5.update();
  if (M5.BtnA.pressedFor(BTN_SHORT_PRESS)) { 
    buttonA_short = true;
  }
  if (M5.BtnA.pressedFor(BTN_LONG_PRESS)) { 
    buttonA_long = true;
  }
  if (M5.BtnA.wasReleased()) { 
    if (buttonA_long) {
      Serial.println("Button A LONG");
      buttonA(DISPLAY_BUTTON_LONG);
    } else if (buttonA_short) {
      Serial.println("Button A SHORT");
      buttonA(DISPLAY_BUTTON_SHORT);
    }
    buttonA_short = false;    
    buttonA_long = false;
  }

  if (M5.BtnB.pressedFor(BTN_SHORT_PRESS)) { 
    buttonB_short = true;
  }
  if (M5.BtnB.pressedFor(BTN_LONG_PRESS)) { 
    buttonB_long = true;
  }
  if (M5.BtnB.wasReleased()) { 
    if (buttonB_long) {
      Serial.println("Button B LONG");
      buttonB(DISPLAY_BUTTON_LONG);
    } else if (buttonB_short) {
      Serial.println("Button B SHORT");
      buttonB(DISPLAY_BUTTON_SHORT);
    }
    buttonB_short = false;    
    buttonB_long = false;
  }

  if (M5.BtnC.pressedFor(BTN_SHORT_PRESS)) { 
    buttonC_short = true;
  }
  if (M5.BtnC.pressedFor(BTN_LONG_PRESS)) { 
    buttonC_long = true;
  }
  if (M5.BtnC.wasReleased()) { 
    if (buttonC_long) {
      Serial.println("Button C LONG");
      buttonC(DISPLAY_BUTTON_LONG);
    } else if (buttonC_short) {
      Serial.println("Button C SHORT");
      buttonC(DISPLAY_BUTTON_SHORT);
    }
    buttonC_short = false;    
    buttonC_long = false;
  }
}


void Display::buttonA(display_btn_t btn) {
  control_evt_t control_evt = CONTROL_EVT_NONE;

  switch (menu) {
    case DISPLAY_MENU_SCAN:
      if (btn == DISPLAY_BUTTON_SHORT) {
        control_evt = CONTROL_EVT_LAST;
        xQueueSend(control_evt_queue, (void *) &control_evt, 10);
      } else if (btn == DISPLAY_BUTTON_LONG) {
        menu = DISPLAY_MENU_CONFIG;
        update_main_config();
        update_footer_config();
      }
      break;

    case DISPLAY_MENU_CONFIG:
      if (btn == DISPLAY_BUTTON_SHORT) {
        settings.remove();
      } else if (btn == DISPLAY_BUTTON_LONG) {

      }
      break;
  }
}

void Display::buttonB(display_btn_t btn) {
  control_evt_t control_evt = CONTROL_EVT_NONE;

  switch (menu) {
    case DISPLAY_MENU_SCAN:
      if (btn == DISPLAY_BUTTON_SHORT) {
        control_evt = CONTROL_EVT_NEXT;
        xQueueSend(control_evt_queue, (void *) &control_evt, 10);
      } else if (btn == DISPLAY_BUTTON_LONG) {
        control.update_current_slide(0);
      }
      break;

    case DISPLAY_MENU_CONFIG:
      if (btn == DISPLAY_BUTTON_SHORT) {
        M5.Power.reset();
      } else if (btn == DISPLAY_BUTTON_LONG) {

      }
      break;
  }
}

void Display::buttonC(display_btn_t btn) {
  control_evt_t control_evt = CONTROL_EVT_NONE;

  switch (menu) {
    case DISPLAY_MENU_SCAN:
      if (btn == DISPLAY_BUTTON_SHORT) {
        if (control.is_scanning()) {
          control_evt = CONTROL_EVT_STOP;
        } else {
          control_evt = CONTROL_EVT_SCAN;
        }
        xQueueSend(control_evt_queue, (void *) &control_evt, 10);
      } else if (btn == DISPLAY_BUTTON_LONG) {
        control_evt = CONTROL_EVT_START;
        xQueueSend(control_evt_queue, (void *) &control_evt, 10);
      }
      break;

    case DISPLAY_MENU_CONFIG:
      if (btn == DISPLAY_BUTTON_SHORT) {
        menu = DISPLAY_MENU_SCAN;
        update_main_scan(0, settings.get_slides_to_scan());
        update_footer_scan(DISPLAY_ACTIVE_NONE);
      } else if (btn == DISPLAY_BUTTON_LONG) {

      }
      break;
  }
}
