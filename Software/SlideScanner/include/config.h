#ifndef __CONFIG_H
#define __CONFIG_H 

#include "Arduino.h"
#include "cable_release.h"
#include "ir_release.h"

#define CONFIG_FILE_NAME "/config.cfg"

#define CONFIG_SCANNER_NAME                       "filmolino"

#define CONFIG_SCANNER_AP_SSID                    "scanner"
#define CONFIG_SCANNER_AP_PASSWORD                "filmolino"
#define CONFIG_SCANNER_AP_ENABLED                 true
#define CONFIG_SCANNER_AP_PARA_MAX_LENGTH         64

#define CONFIG_SCANNER_STA_SSID                   "Your SSID"
#define CONFIG_SCANNER_STA_PASSWORD               "Your password"
#define CONFIG_SCANNER_STA_ENABLED                false
#define CONFIG_SCANNER_STA_PARA_MAX_LENGTH        64

#define CONFIG_DEFAULT_SLIDE_CHANGE_TIME          2000      /* 2s time to change the slide */
#define CONFIG_DEFAULT_SLIDE_CHANGE_PULSE_TIME    250       /* 0.25s pulse time for slide change */
#define CONFIG_DEFAULT_SLIDE_SCAN_TIME            1500      /* 1.5s time to scan/capture the slide */
#define CONFIG_DEFAULT_SLIDES_TO_SCAN             50        /* Slides to scan (slides in magazine) */

#define CONFIG_DEFAULT_IR_ENABLED                 true      /* IR release active */
#define CONFIG_DEFAULT_IR_SELECTED_CODE           5         /* Selected IR release code 3 = NIKON [1..32] */
#define CONFIG_DEFAULT_CABLE_ENABLED              true      /* Cable release active */
#define CONFIG_DEFAULT_CABLE_SELECTED_CODE        1         /* Selected cable release code 1 = BASIC [1..32] */



typedef struct {
  String ap_ssid;                   /* Access Point Settings */
  String ap_password;
  String sta_ssid;                  /* Station Settings */
  String sta_password;
  bool ap;                            /* true if access point is active */
  bool sta;                           /* true if station mode is active */
} config_wifi_t; 


typedef struct {
  uint32_t delay_slide_change;        /* Delay in ms */
  uint32_t time_slide_change_pulse;   /* pulse time in ms */
  uint32_t delay_slide_scan;          /* Delay in ms */
  uint8_t slides_to_scan;             /* Slides to scan */
  bool ir;                            /* true if active */
  uint8_t ir_selected_code;
  bool cable;                         /* true if active */
  uint8_t cable_selected_code;
} config_control_t;

typedef struct {
  const ir_release_seq_t * ir_codes[IR_RELEASE_CODES_SIZE];
  const cable_release_seq_t * cable_codes[CABLE_RELEASE_CODES_SIZE];
} config_release_t;

typedef struct {
  config_wifi_t     wifi;
  config_control_t  control;
  config_release_t  release;
} config_scanner_t;



class Config {
  public:
    // Constructor
    Config(String filename);

    // 
    void begin();
    size_t read();
    size_t write();
    bool remove();
    void load_defaults();
    config_wifi_t get_wifi();
    void set_wifi(config_wifi_t data);
    uint32_t get_delay_slide_change();
    uint32_t get_delay_slide_scan();
    uint8_t get_slides_to_scan();
    uint32_t get_time_slide_change_pulse();
    bool get_cable_active();
    uint8_t get_cable_selected_code();
    String get_cable_name(uint8_t code);
    bool get_ir_active();
    uint8_t get_ir_divider(uint8_t code);
    uint32_t get_ir_freq(uint8_t code);
    uint8_t get_ir_selected_code();
    String get_ir_name(uint8_t code);
    rmt_item32_t *get_ir_seq();
    void set_cable_release();

    void get_csv_cfg_wifi(char *csv);
    void set_csv_cfg_wifi(char *csv);

    void get_csv_cfg_scanner(char *csv);
    void set_csv_cfg_scanner(char *csv);

    void get_csv_cable_release_names(char *csv, uint32_t bufferSize);
    void get_csv_ir_release_names(char *csv , uint32_t bufferSize);

  private:
    String filename = "";
    config_scanner_t settings;

  };

#endif