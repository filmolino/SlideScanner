#ifndef __CONTROL_H
#define __CONTROL_H

#define ESP_TIMER_NO_AS_DELAY_TIMER      0       // Use Timer0 as delay timer
#define ESP_TIMER_NO_AS_PULSE_TIMER      1       // Use Timer0 as delay timer

// STATES
typedef enum {
  CONTROL_STATE_IDLE = 0,
  CONTROL_STATE_ERROR = 1,
  CONTROL_STATE_NEXT = 2,
  CONTROL_STATE_LAST = 3,
  CONTROL_STATE_SCAN = 4,
} control_states_t;

// EVENTS
typedef enum {
  CONTROL_EVT_NONE = 0,
  CONTROL_EVT_NEXT = 1,
  CONTROL_EVT_LAST = 2,
  CONTROL_EVT_START = 3,
  CONTROL_EVT_STOP = 4,
  CONTROL_EVT_SCAN = 5,
  CONTROL_EVT_BTN_INTERNAL_SHORT = 6,
  CONTROL_EVT_BTN_INTERNAL_LONG = 7,
  CONTROL_EVT_TIMER_DELAY = 8,
  CONTROL_EVT_TIMER_PULSE = 9,
  CONTROL_EVT_RESET_CURRENT_SLIDES = 10,
  CONTROL_EVT_ERROR = 11,
} control_evt_t;

class Control {
  public:
    void begin(gpio_num_t last, gpio_num_t next);
    void sm(control_evt_t argEvt);
    void update_current_slide(uint8_t val);
    void update_max_slides();
    bool is_scanning();
    void set_current_slide(char *payload);
    void get_current_slide(char *payload);
    
  private:
    control_states_t _current_state;
    bool autoScan = false;
    uint8_t current_slide = 0;
    gpio_num_t pin_last;
    gpio_num_t pin_next;
    hw_timer_t * delay_timer = NULL;
    hw_timer_t * pulse_timer = NULL;

    void state_idle_entry(void);
    void state_idle(control_evt_t argEvt);
    void state_error_entry(void);
    void state_error(control_evt_t argEvt);
    void state_next_entry(void);
    void state_next(control_evt_t argEvt);
    void state_last_entry(void);
    void state_last(control_evt_t argEvt);
    void state_scan_entry(void); 
    void state_scan(control_evt_t argEvt);
    void start_delay_timer(uint32_t delay_ms);
    void stop_delay_timer();
    void start_pulse_timer(uint32_t delay_ms);
    void stop_pulse_timer();
    void update_auto_scan_button(bool state);
    QueueHandle_t get_queue();
    void set_cable_release_queue();
};

#endif
