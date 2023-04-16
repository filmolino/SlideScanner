#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <M5Stack.h>

#define ESP_TIMER_NO_AS_DELAY_TIMER      0       // Use Timer0 as delay timer
#define ESP_TIMER_NO_AS_PULSE_TIMER      1       // Use Timer0 as delay timer

typedef enum {
  DISPLAY_ACTIVE_NONE,
  DISPLAY_ACTIVE_LAST,
  DISPLAY_ACTIVE_NEXT,
  DISPLAY_ACTIVE_SCAN,
  DISPLAY_ACTIVE_MENU,
  DISPLAY_ACTIVE_AUTO,
} display_header_active_t;

typedef enum {
  DISPLAY_BATTERY_NIL,
  DISPLAY_BATTERY_100,
  DISPLAY_BATTERY_75,
  DISPLAY_BATTERY_50,
  DISPLAY_BATTERY_25,
  DISPLAY_BATTERY_0,
  DISPLAY_BATTERY_CHARGING,
  DISPLAY_BATTERY_ERROR,
} display_battery_state_t;

typedef enum {
  DISPLAY_BUTTON_NONE,
  DISPLAY_BUTTON_SHORT,
  DISPLAY_BUTTON_LONG,
} display_btn_t;

typedef enum {
  DISPLAY_MENU_SCAN,
  DISPLAY_MENU_CONFIG,
} display_menu_t;

class Display {
  public:
    void begin();
    void update_header();
    void update_main_scan(uint8_t current_slide, uint8_t slides_to_scan);
    void update_footer_scan(display_header_active_t active);
    void update_main_config();
    void update_footer_config();
    void update();
    void buttons();
    
  private:
    void buttonA(display_btn_t btn);
    void buttonB(display_btn_t btn);
    void buttonC(display_btn_t btn);

    display_menu_t menu = DISPLAY_MENU_SCAN;
    TFT_eSprite sprHeader = TFT_eSprite(&M5.Lcd);
    TFT_eSprite sprMain = TFT_eSprite(&M5.Lcd);
    TFT_eSprite sprFooter = TFT_eSprite(&M5.Lcd);
};

#endif
