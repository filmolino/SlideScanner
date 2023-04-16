#include <Arduino.h>
#include <M5Stack.h>
#include "driver/rmt.h"
#include "ir_release.h"

// IR codes
static const ir_release_seq_t code_canon = {
  .name = "Canon",
  .ir_seq = {
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1, 23491, 0 },  // Ton = 0.011ms,   Toff 7.341ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    {   35, 1,   35, 0 },   // Ton = 0.011ms,   Toff 0.011ms
    { 0, 1, 0, 0 }          // RMT end marker
  },
  .freq = 33000,
  .divider = 25,                // t = 1/(80 MHz / 25) = 0.0000003125  
};

static const ir_release_seq_t code_minolta = {
  .name = "Minolta",
  .ir_seq = {
    { 1176, 0,  593, 1 },   // Ton = 3.75ms,   Toff 1.89ms
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  449, 0 },   // Ton = 0.456ms,   Toff 1.430ms (1)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  449, 0 },   // Ton = 0.456ms,   Toff 1.430ms (1)
    {  143, 1,  449, 0 },   // Ton = 0.456ms,   Toff 1.430ms (1)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  449, 0 },   // Ton = 0.456ms,   Toff 1.430ms (1)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  449, 0 },   // Ton = 0.456ms,   Toff 1.430ms (1)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  449, 0 },   // Ton = 0.456ms,   Toff 1.430ms (1)
    {  143, 1,  449, 0 },   // Ton = 0.456ms,   Toff 1.430ms (1)
    {  143, 1,  449, 0 },   // Ton = 0.456ms,   Toff 1.430ms (1)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  449, 0 },   // Ton = 0.456ms,   Toff 1.430ms (1)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  449, 0 },   // Ton = 0.456ms,   Toff 1.430ms (1)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  153, 0 },   // Ton = 0.456ms,   Toff 0.487ms (0)
    {  143, 1,  449, 0 },   // Ton = 0.456ms,   Toff 1.430ms (1)
    { 0, 1, 0, 0 }          // RMT end marker
  },
  .freq = 38000,
  .divider = 255,       // t = 1/(80 MHz / 255) = 0.0000031875
};

static const ir_release_seq_t code_nikon = {
  .name = "Nikon",
  .ir_seq = {
    { 627, 1,  8730, 0 }, 
    { 125, 1,   496, 0 },
    { 125, 1,  1123, 0 },   // Ton = 400us, Toff  3.58ms
    { 125, 1, 19827, 0 },   // Ton = 400us, Toff 63.20ms
    { 627, 1,  8730, 0 },   // Ton =   2ms, Toff 27.83ms  PERIOD2
    { 125, 1,   496, 0 },   // Ton = 400us, Toff  1.58ms
    { 125, 1,  1123, 0 },   // Ton = 400us, Toff  3.58ms
    { 125, 1, 19827, 0 },   // Ton = 400us, Toff 63.20ms
    { 627, 1,  8730, 0 },   // Ton =   2ms, Toff 27.83ms  PERIOD3
    { 125, 1,   496, 0 },   // Ton = 400us, Toff  1.58ms
    { 125, 1,  1123, 0 },   // Ton = 400us, Toff  3.58ms
    { 125, 1, 19827, 0 },   // Ton = 400us, Toff 63.20ms
    { 0, 1, 0, 0 }          // RMT end marker
  },
  .freq = 38000,
  .divider = 255,           // t = 1/(80 MHz / 255) = 0.0000031875
};

static const ir_release_seq_t code_olympus = {
  .name = "Olympus",
  .ir_seq = {
    { 2815, 1, 1375, 0 },   // Ton = 8.972ms, Toff 4.384ms
    {  196, 1,    0, 0 },   // Ton = 0.624ms, Toff 0.000ms
    {  153, 0,  188, 1 },   // Ton = 0.488ms, Toff 0.600ms (0)
    {  502, 0,  188, 1 },   // Ton = 1.6ms,   Toff 0.600ms (1)
    {  502, 0,  188, 1 },   // Ton = 1.6ms,   Toff 0.600ms (1)
    {  153, 0,  188, 1 },   // Ton = 0.488ms, Toff 0.600ms (0)
    {  153, 0,  188, 1 },   // Ton = 0.488ms, Toff 0.600ms (0)
    {  153, 0,  188, 1 },   // Ton = 0.488ms, Toff 0.600ms (0)
    {  153, 0,  188, 1 },   // Ton = 0.488ms, Toff 0.600ms (0)
    {  502, 0,  188, 1 },   // Ton = 1.6ms,   Toff 0.600ms (1)
    {  502, 0,  188, 1 },   // Ton = 1.6ms,   Toff 0.600ms (1)
    {  502, 0,  188, 1 },   // Ton = 1.6ms,   Toff 0.600ms (1)
    {  153, 0,  188, 1 },   // Ton = 0.488ms, Toff 0.600ms (0)
    {  502, 0,  188, 1 },   // Ton = 1.6ms,   Toff 0.600ms (1)
    {  502, 0,  188, 1 },   // Ton = 1.6ms,   Toff 0.600ms (1)
    {  502, 0,  188, 1 },   // Ton = 1.6ms,   Toff 0.600ms (1)
    {  153, 0,  188, 1 },   // Ton = 0.488ms, Toff 0.600ms (0)
    {  153, 0,  188, 1 },   // Ton = 0.488ms, Toff 0.600ms (0)
    {  502, 0,  188, 1 },   // Ton = 1.6ms,   Toff 0.600ms (1)
    {  153, 0,  188, 1 },   // Ton = 0.488ms, Toff 0.600ms (0)
    {  153, 0,  188, 1 },   // Ton = 0.488ms, Toff 0.600ms (0)
    {  153, 0,  188, 1 },   // Ton = 0.488ms, Toff 0.600ms (0)
    {  153, 0,  188, 1 },   // Ton = 0.488ms, Toff 0.600ms (0)
    {  153, 0,  188, 1 },   // Ton = 0.488ms, Toff 0.600ms (0)
    {  153, 0,  188, 1 },   // Ton = 0.488ms, Toff 0.600ms (0)
    {  153, 0,  188, 1 },   // Ton = 0.488ms, Toff 0.600ms (0)
    {  153, 0,  188, 1 },   // Ton = 0.488ms, Toff 0.600ms (0)
    {  502, 0,  188, 1 },   // Ton = 1.6ms,   Toff 0.600ms (1)
    {  502, 0,  188, 1 },   // Ton = 1.6ms,   Toff 0.600ms (1)
    {  502, 0,  188, 1 },   // Ton = 1.6ms,   Toff 0.600ms (1)
    {  502, 0,  188, 1 },   // Ton = 1.6ms,   Toff 0.600ms (1)
    {  502, 0,  188, 1 },   // Ton = 1.6ms,   Toff 0.600ms (1)
    {  502, 0,  188, 1 },   // Ton = 1.6ms,   Toff 0.600ms (1)
    {  502, 0,  188, 1 },   // Ton = 1.6ms,   Toff 0.600ms (1)
    { 0, 1, 0, 0 }          // RMT end marker
  },
  .freq = 40000,
  .divider = 255,           // t = 1/(80 MHz / 255) = 0.0000031875
};

static const ir_release_seq_t code_pentax = {
  .name = "Pentax",
  .ir_seq = {
    { 4078, 1, 941, 0 },    // Ton = 13ms, Toff 3ms
    {  314, 1, 314, 0 },    // Ton =  1ms, Toff 1ms
    {  314, 1, 314, 0 },    // Ton =  1ms, Toff 1ms
    {  314, 1, 314, 0 },    // Ton =  1ms, Toff 1ms
    {  314, 1, 314, 0 },    // Ton =  1ms, Toff 1ms
    {  314, 1, 314, 0 },    // Ton =  1ms, Toff 1ms
    {  314, 1, 314, 0 },    // Ton =  1ms, Toff 1ms
    {  314, 1, 314, 0 },    // Ton =  1ms, Toff 1ms
    { 0, 1, 0, 0 }          // RMT end marker
  },
  .freq = 38000,
  .divider = 255,           // t = 1/(80 MHz / 255) = 0.0000031875
};

static const ir_release_seq_t code_sony = {
  .name = "Sony",
  .ir_seq = {
    {  728, 1,  204, 0 },   // Ton = 2.320ms, Toff 0.650ms
    {  728, 1,  204, 0 },   // Ton = 2.320ms, Toff 0.650ms
    {  728, 1,  204, 0 },   // Ton = 2.320ms, Toff 0.650ms
    {  728, 1,  204, 0 },   // Ton = 2.320ms, Toff 0.650ms
    {  367, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (1)
    {  180, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (0)
    {  367, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (1)
    {  367, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (1)
    {  180, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (0)
    {  367, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (1)
    {  180, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (0)
    {  180, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (0)
    {  367, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (1)
    {  180, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (0)
    {  367, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (1)
    {  367, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (1)
    {  367, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (1)
    {  180, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (0)
    {  180, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (0)
    {  180, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (0)
    {  367, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (1)
    {  367, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (1)
    {  367, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (1)
    {  367, 1,  204, 0 },   // Ton = 1.175ms, Toff 0.650ms (1)
    { 0, 1, 0, 0 }          // RMT end marker
  },
  .freq = 40000,
  .divider = 255,       // t = 1/(80 MHz / 255) = 0.0000031875
};

// empty
static const ir_release_seq_t code_empty = {
  .name = "Empty",
  .ir_seq = {
    { 0, 1, 0, 0 }              // RMT end marker
  },
  .freq = 38000,
  .divider = 255,               // t = 1/(80 MHz / 25) = 0.0000003125
};


void IrRelease::begin(gpio_num_t pin, rmt_channel_t ir_channel, uint8_t divider, uint32_t freq) {
  channel = ir_channel;
  rmt_config_t rmt = RMT_DEFAULT_CONFIG_TX(pin, channel);
  rmt.tx_config.carrier_en = true;
  rmt.tx_config.carrier_duty_percent = 50;
  rmt.tx_config.carrier_freq_hz = freq;
  rmt.clk_div = divider;     
  ESP_ERROR_CHECK(rmt_config(&rmt));
  ESP_ERROR_CHECK(rmt_driver_install(rmt.channel, 0, 0));
}

const ir_release_seq_t * IrRelease::getCode(ir_release_codes_t code) {
  switch (code) {
    case ir_release_code_canon:     return &code_canon;     break;
    case ir_release_code_minolta:   return &code_minolta;   break;
    case ir_release_code_nikon:     return &code_nikon;     break;
    case ir_release_code_olympus:   return &code_olympus;   break;
    case ir_release_code_pentax:    return &code_pentax;    break;
    case ir_release_code_sony:      return &code_sony;      break;
    case ir_release_code_empty:     return &code_empty;     break;
    default:                        return NULL;            break;
  }
}

void IrRelease::send(rmt_item32_t *item) {
  rmt_write_items(channel, item, IR_RELEASE_SEQ_SIZE, false);
}