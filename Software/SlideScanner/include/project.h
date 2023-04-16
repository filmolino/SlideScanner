#ifndef __PROJECT_H
#define ___PROJECT_H

// Includes
#include "config.h"
#include "display.h"
#include "control.h"
#include "wifimanager.h"
#include "Webmanager.h"

// Graphics
#include "img_filmolino.h"
#include "img_arrow_left_circle_fill.h"
#include "img_arrow_left_circle_fill_active.h"
#include "img_arrow_right_circle_fill.h"
#include "img_arrow_right_circle_fill_active.h"
#include "img_battery.h"
#include "img_battery_charging.h"
#include "img_battery_full.h"
#include "img_battery_half.h"
#include "img_camera_fill.h"
#include "img_camera_fill_active.h"
#include "img_link_45deg.h"
#include "img_list.h"
#include "img_list_Active.h"
#include "img_play_circle_fill.h"
#include "img_play_circle_fill_active.h"
#include "img_x_circle_fill.h"
#include "img_slide_scanner_small.h"

// Sprites width and height
#define SPRITE_HEADER_WIDTH                 320
#define SPRITE_HEADER_HEIGHT                40
#define SPRITE_MAIN_WIDTH                   320
#define SPRITE_MAIN_HEIGHT                  160
#define SPRITE_FOOTER_WIDTH                 320
#define SPRITE_FOOTER_HEIGHT                40

// LCD Header/Footer positions
#define LCD_POS_HEADER_FOOTER_ICON_Y        4
#define LCD_POS_HEADER_FOOTER_TEXT_Y        2

// Line spacing for info menu
#define LCD_MENU_LINE_SPACING               18

// Startup Logo
#define DISPLAY_SHOW_LOGO_TIME_MS           2000

// Button settings
#define BTN_SHORT_PRESS                     25      // 25 ms
#define BTN_LONG_PRESS                      500     // 500 ms
#define BTN_PERIOD                          20      // 20 ms

// GPIO
#define GPIO_RELAY_LAST                     GPIO_NUM_26
#define GPIO_RELAY_NEXT                     GPIO_NUM_2
#define GPIO_CABLE_RELEASE_FOCUS            GPIO_NUM_13
#define GPIO_CABLE_RELEASE_SHUTTER          GPIO_NUM_12
#define GPIO_IR_RELEASE                     GPIO_NUM_5
#define GPIO_OPTO_IN                        GPIO_NUM_36

// ADC
#define ADC_OPTO                            GPIO_NUM_35

// Typedefs
typedef enum {
  BUTTON_NONE,
  BUTTON_A_SHORT,
  BUTTON_A_LONG,
  BUTTON_B_SHORT,
  BUTTON_B_LONG,
  BUTTON_C_SHORT,
  BUTTON_C_LONG,  
} Button_pressed_t;


#endif