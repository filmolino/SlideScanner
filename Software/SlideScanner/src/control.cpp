#include <Arduino.h>
#include <M5Stack.h>
#include "project.h"
#include "cable_release.h"
#include "ir_release.h"
#include "config.h"
#include "control.h"

// Classes
extern Config settings;
extern Display display;
extern IrRelease ir_release;
extern Webmanager webmanager;
extern QueueHandle_t control_evt_queue;
extern QueueHandle_t cable_release_evt_queue;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void ARDUINO_ISR_ATTR delay_timer_callback() {
  portENTER_CRITICAL(&mux);
  control_evt_t evt = CONTROL_EVT_TIMER_DELAY;
  xQueueSend(control_evt_queue, (void *) &evt, 10);
  portEXIT_CRITICAL(&mux);
}

void ARDUINO_ISR_ATTR pulse_timer_callback()
{
  portENTER_CRITICAL(&mux);
  control_evt_t evt = CONTROL_EVT_TIMER_PULSE;
  xQueueSend(control_evt_queue, (void *) &evt, 10);
  portEXIT_CRITICAL(&mux);
}

void Control::begin(gpio_num_t last, gpio_num_t next) {
  _current_state = CONTROL_STATE_IDLE;

  // GPIO
  pin_last = last;
  pin_next = next;
  gpio_config_t config = {};
  config.mode = GPIO_MODE_OUTPUT;
  config.pull_up_en = GPIO_PULLUP_DISABLE;
  config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  config.intr_type = GPIO_INTR_DISABLE;
  
  config.pin_bit_mask = 1LL << (uint64_t) pin_last;
  gpio_config(&config);
  gpio_set_level(pin_last, 0);

  config.pin_bit_mask = 1LL << (uint64_t) pin_next;
  gpio_config(&config);
  gpio_set_level(pin_next, 0);

  // Timer
  delay_timer = timerBegin(ESP_TIMER_NO_AS_DELAY_TIMER, 80, true);
  pulse_timer = timerBegin(ESP_TIMER_NO_AS_PULSE_TIMER, 80, true);
  timerStop(delay_timer);   // Timer was started by timerBegin but we just want to configure it here
  timerStop(pulse_timer);
  timerAttachInterrupt(delay_timer, &delay_timer_callback, true);
  timerAttachInterrupt(pulse_timer, &pulse_timer_callback, true);
}


// --------------------------------------------------------------------------------------------------
// State: IDLE
// --------------------------------------------------------------------------------------------------
void Control::state_idle_entry(void) {
  Serial.println("entry: CONTROL_STATE_IDLE");
  display.update_footer_scan(DISPLAY_ACTIVE_NONE);
  update_auto_scan_button(false);
  _current_state = CONTROL_STATE_IDLE;
  gpio_set_level(pin_last, 0);
  gpio_set_level(pin_next, 0);
  stop_delay_timer();
  stop_pulse_timer();
  autoScan = false;
}

void Control::state_idle(control_evt_t argEvt) {
  switch (argEvt) {
    case CONTROL_EVT_NONE:
      break;

    case CONTROL_EVT_NEXT:
      autoScan = false;
      state_next_entry();
      break;

    case CONTROL_EVT_LAST:
      autoScan = false;
      state_last_entry();
      break;

    case CONTROL_EVT_START:
      autoScan = true; 
      display.update_footer_scan(DISPLAY_ACTIVE_AUTO);
      update_auto_scan_button(true);
      state_next_entry();
      break;

    case CONTROL_EVT_STOP:
      break;

    case CONTROL_EVT_SCAN:
      autoScan = false;
      state_scan_entry();
      break;

    case CONTROL_EVT_TIMER_DELAY:
      break;

    case CONTROL_EVT_TIMER_PULSE:
      break;

    case CONTROL_EVT_RESET_CURRENT_SLIDES:
      update_current_slide(0);
      break;

    case CONTROL_EVT_ERROR:
      state_error_entry();
      break;

    default:
      state_idle_entry();   // Should not happen, stay in IDLE state
      break;
  }
}


// --------------------------------------------------------------------------------------------------
// State: ERROR
// --------------------------------------------------------------------------------------------------
void Control::state_error_entry(void) {
  Serial.println("entry: CONTROL_STATE_ERROR");
  _current_state = CONTROL_STATE_ERROR;
}

void Control::state_error(control_evt_t argEvt) {
  switch (argEvt) {
    case CONTROL_EVT_NONE:
      break;

    case CONTROL_EVT_NEXT:
      break;

    case CONTROL_EVT_LAST:
      break;

    case CONTROL_EVT_START:
      break;

    case CONTROL_EVT_STOP:
      break;

    case CONTROL_EVT_SCAN:
      break;

    case CONTROL_EVT_TIMER_DELAY:
      break;

    case CONTROL_EVT_TIMER_PULSE:
      break;

    case CONTROL_EVT_ERROR:
      state_error_entry();
      break;

    default:
      state_idle_entry();   // Should not happen, stay in IDLE state
      break;
  }
}


// --------------------------------------------------------------------------------------------------
// State: CONTROL_STATE_NEXT
// --------------------------------------------------------------------------------------------------
void Control::state_next_entry(void) {
  Serial.println("entry: CONTROL_STATE_NEXT");
  if (!autoScan) {
    display.update_footer_scan(DISPLAY_ACTIVE_NEXT);
  }
  if (current_slide < settings.get_slides_to_scan()) {
    update_current_slide(current_slide + 1);    
    _current_state = CONTROL_STATE_NEXT;
    gpio_set_level(pin_next, 1);
    start_delay_timer(settings.get_delay_slide_change());
    start_pulse_timer(settings.get_time_slide_change_pulse());
  }
  else {
    // all slides are scanned, nothing to do
    state_idle_entry();
  }
}

void Control::state_next(control_evt_t argEvt) {
  switch (argEvt) {
    case CONTROL_EVT_NONE:
      break;

    case CONTROL_EVT_NEXT:
      break;

    case CONTROL_EVT_LAST:
      break;

    case CONTROL_EVT_START:
      break;

    case CONTROL_EVT_STOP:
      state_idle_entry();
      break;

    case CONTROL_EVT_SCAN:
      break;

    case CONTROL_EVT_TIMER_PULSE:
      stop_pulse_timer();
      gpio_set_level(pin_next, 0);
      break;

    case CONTROL_EVT_TIMER_DELAY:
      autoScan ? state_scan_entry() : state_idle_entry();
      break;

    case CONTROL_EVT_ERROR:
      state_error_entry();
      break;

    default:
      state_idle_entry();   // Should not happen, go to IDLE state
      break;
  }
}


// --------------------------------------------------------------------------------------------------
// State: CONTROL_STATE_LAST
// --------------------------------------------------------------------------------------------------
void Control::state_last_entry(void) {
  Serial.println("entry: CONTROL_STATE_LAST");
  if (!autoScan) {
    display.update_footer_scan(DISPLAY_ACTIVE_LAST);
  }
  if (current_slide > 0) {
    update_current_slide(current_slide - 1);
    _current_state = CONTROL_STATE_LAST;
    gpio_set_level(pin_last, 1);
    start_delay_timer(settings.get_delay_slide_change());
    start_pulse_timer(settings.get_time_slide_change_pulse());  
  }
  else {
    // First slide reached, nothing to scan
    state_idle_entry();
  }
}

void Control::state_last(control_evt_t argEvt) {
  switch (argEvt) {
    case CONTROL_EVT_NONE:
      break;

    case CONTROL_EVT_NEXT:
      break;

    case CONTROL_EVT_LAST:
      break;

    case CONTROL_EVT_START:
      break;

    case CONTROL_EVT_STOP:
      state_idle_entry();
      break;

    case CONTROL_EVT_SCAN:
      break;

    case CONTROL_EVT_TIMER_PULSE:
      stop_pulse_timer();
      gpio_set_level(pin_last, 0);
      break;

    case CONTROL_EVT_TIMER_DELAY:
      state_idle_entry();
      break;

    case CONTROL_EVT_ERROR:
      state_error_entry();
      break;

    default:
      state_idle_entry();   // Should not happen, go to IDLE state
      break;
  }
}


// --------------------------------------------------------------------------------------------------
// State: SCAN
// --------------------------------------------------------------------------------------------------
void Control::state_scan_entry(void) {
  Serial.println("entry: CONTROL_STATE_SCAN");
  if (!autoScan) {
    display.update_footer_scan(DISPLAY_ACTIVE_SCAN);
  }
  _current_state = CONTROL_STATE_SCAN;

  if (settings.get_ir_active()) {
    Serial.println("IR send");
    ir_release.send(settings.get_ir_seq());
  }
  
  if (settings.get_cable_active()) {
    Serial.println("CABLE send");
    cable_release_evt_t evt = CABLE_RELEASE_EVT_START;
    xQueueSend(cable_release_evt_queue, (void *) &evt, 10);
  }
  start_delay_timer(settings.get_delay_slide_scan());
}

void Control::state_scan(control_evt_t argEvt) {
  switch (argEvt) {
    case CONTROL_EVT_NONE:
      break;

    case CONTROL_EVT_NEXT:
      break;

    case CONTROL_EVT_LAST:
      break;

    case CONTROL_EVT_START:
      break;

    case CONTROL_EVT_STOP:
      state_idle_entry();
      break;

    case CONTROL_EVT_SCAN:
      break;

    case CONTROL_EVT_TIMER_PULSE:
      stop_pulse_timer();
      break;

    case CONTROL_EVT_TIMER_DELAY:
      stop_delay_timer();
      if ((current_slide < settings.get_slides_to_scan()) && autoScan) {
        state_next_entry();
      } else {
        state_idle_entry();
      }
      break;

    case CONTROL_EVT_ERROR:
      state_error_entry();
      break;

    default:
      state_idle_entry();   // Should not happen, go to IDLE state
      break;
  }
}


// Statemachine
void Control::sm(control_evt_t argEvt) 
{
  Serial.printf("control_sm current_state=%u  |  argEvt=%u\n\r", _current_state, argEvt);

  switch (_current_state) {
    case CONTROL_STATE_IDLE:        state_idle(argEvt); break;
    case CONTROL_STATE_ERROR:       state_error(argEvt); break;
    case CONTROL_STATE_NEXT:        state_next(argEvt); break;
    case CONTROL_STATE_LAST:        state_last(argEvt); break;
    case CONTROL_STATE_SCAN:        state_scan(argEvt); break;
    default:                        state_idle_entry(); break;
  }
}

// TIMER delay
void Control::start_delay_timer(uint32_t delay_ms) {
  Serial.printf("Starting delay_timer delay=%u\n\r", delay_ms * 1000);
  timerRestart(delay_timer);
  timerStart(delay_timer);
  timerAlarmWrite(delay_timer, delay_ms * 1000, true);
  timerAlarmEnable(delay_timer);
}

void Control::stop_delay_timer() {
  timerStop(delay_timer);
}


// TIMER pulse
void Control::start_pulse_timer(uint32_t delay_ms) {
  Serial.printf("Starting pulse_timer delay=%u\n\r", delay_ms * 1000);
  timerRestart(pulse_timer);
  timerStart(pulse_timer);
  timerAlarmWrite(pulse_timer, delay_ms * 1000, true);
  timerAlarmEnable(pulse_timer);
}

void Control::stop_pulse_timer() {
  timerStop(pulse_timer);
}


void Control::set_current_slide(char *payload) {
  char *data;
  uint8_t slide;

  data = strtok(payload, ",");    // CMD not used here
  data = strtok(NULL, ",");
  slide = strtoul(data, NULL, 10);
  update_current_slide(slide);
}


void Control::get_current_slide(char *payload) {
  sprintf(payload, "CurrentSlide,%u", current_slide);
}


void Control::update_current_slide(uint8_t val) {
  char data[32];

  current_slide = val;
  sprintf(data, "CurrentSlide,%u", current_slide);

  // Update Display
  display.update_main_scan(current_slide, settings.get_slides_to_scan());

  // Update Web
  webmanager.ws_async_send(data);
}


void Control::update_max_slides() {
  // Update Display
  display.update_main_scan(current_slide, settings.get_slides_to_scan());

  // Update Web
  // Web updates itself in js code
}

// Changes button color to inactive
void Control::update_auto_scan_button(bool state) {
  char data[32];

  sprintf(data, "AutoScan,%d", state);
  webmanager.ws_async_send(data);
}

// Get ptr to event queue
QueueHandle_t Control::get_queue() {
  return control_evt_queue;
}

// Set cable release queue
void Control::set_cable_release_queue() {

// wieder aktivieren
// cable_release_evt_queue = cable_release_get_queue_ptr();
  Serial.printf("cable_release_evt_queue %u\n\r", cable_release_evt_queue);
}

// Get scan state
bool Control::is_scanning() {
  return autoScan;
}

