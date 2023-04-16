#include <Arduino.h>
#include <M5Stack.h>
#include "project.h"
#include "cable_release.h"

// Basic (Nikon getestet)
const cable_release_seq_t code_basic = {
  .name = "Basic",
  .cable_seq = {
    CABLE_RELEASE_FOCUS_ON,
    CABLE_RELEASE_DELAY,              /* Delay 500 ms */
    0xF4,
    0x01,
    CABLE_RELEASE_SHUTTER_ON,
    CABLE_RELEASE_DELAY,              /* Delay 200 ms */
    200,                            
    0,
    CABLE_RELEASE_FOCUS_OFF,
    CABLE_RELEASE_SHUTTER_OFF,
    CABLE_RELEASE_END
  }
};

const cable_release_seq_t code_empty = {
  .name = "Empty",
  .cable_seq = {
    CABLE_RELEASE_END
  }
};


void CableRelease::begin(gpio_num_t shutter, gpio_num_t focus) {
  current_cmd = 0;
  seq_active = false;

  // GPIO
  pin_shutter = shutter;
  pin_focus = focus;
  gpio_config_t config = {};
  config.mode = GPIO_MODE_OUTPUT;
  config.pull_up_en = GPIO_PULLUP_DISABLE;
  config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  config.intr_type = GPIO_INTR_DISABLE;
  config.pin_bit_mask = 1LL << (uint64_t) pin_shutter;
  gpio_config(&config);
  config.pin_bit_mask = 1LL << (uint64_t) pin_focus;
  gpio_config(&config);
  gpio_set_level(pin_shutter, 0);
  gpio_set_level(pin_focus, 0);
}

const cable_release_seq_t * CableRelease::getCode(cable_release_codes_t code) {
  switch (code) {
    case cable_release_code_empty:    return &code_empty;     break;
    case cable_release_code_basic:    return &code_basic;     break;
    default:                          return NULL;            break;
  }
}

// Sets pointer to current sequence
void CableRelease::set_seq(uint8_t *seq) {
  current_seq = seq;
  Serial.printf("current_seq = %d\n\r", *current_seq);
}

void CableRelease::sm(cable_release_evt_t evt) {
  uint32_t delay;

  if (evt == CABLE_RELEASE_EVT_START) {
    current_cmd = 0;
    seq_active = true;

    Serial.printf("starting sequence ptr=%u  |  val=%u\n\r", current_seq, *current_seq);

    if (!current_seq) {
      Serial.println("ERROR no valid buffer");          
      seq_active = false;       /* no valid buffer */
    }

    while ((current_cmd < CABLE_RELEASE_SEQ_SIZE) && (seq_active))
    {
      switch(*(current_seq + current_cmd++) ) {
        case CABLE_RELEASE_FOCUS_ON:
          Serial.println("CMD Focus ON");
          gpio_set_level(pin_focus, 1);
          break;

        case CABLE_RELEASE_FOCUS_OFF:
          Serial.println("CMD Focus OFF");            
          gpio_set_level(pin_focus, 0);
          break;

        case CABLE_RELEASE_SHUTTER_ON:
          Serial.println("CMD Shutter ON");            
          gpio_set_level(pin_shutter, 1);
          break;

        case CABLE_RELEASE_SHUTTER_OFF:
          Serial.println("CMD Shutter OFF");            
          gpio_set_level(pin_shutter, 0);
          break;

        case CABLE_RELEASE_DELAY:
          delay = *(current_seq + current_cmd++);
          delay |= *(current_seq + current_cmd++) << 8;
          vTaskDelay(delay / portTICK_PERIOD_MS);
          break;

        case CABLE_RELEASE_END:
          seq_active = false;
          break;
        
        default:
          seq_active = false;
          break;
      }
    }
  }
}

