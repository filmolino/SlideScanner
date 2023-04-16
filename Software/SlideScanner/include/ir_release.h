#ifndef __IR_RELEASE_H
#define __IR_RELEASE_H 

#include "driver/rmt.h"

#define IR_RELEASE_SEQ_SIZE       40
#define IR_RELEASE_CODES_SIZE     32

typedef struct {
  String name;
  rmt_data_t ir_seq[IR_RELEASE_SEQ_SIZE];
  uint32_t freq;
  uint8_t divider;
} ir_release_seq_t;

typedef enum {
  ir_release_code_canon,
  ir_release_code_minolta,
  ir_release_code_nikon,
  ir_release_code_olympus,
  ir_release_code_pentax,
  ir_release_code_sony,
  ir_release_code_empty       // empty must be the last in the enum list
} ir_release_codes_t;

class IrRelease {
  public:
    void begin(gpio_num_t pin, rmt_channel_t channel, uint8_t divider, uint32_t freq);
    const ir_release_seq_t * getCode(ir_release_codes_t code);
    void send(rmt_item32_t *item);
    
  private:
    rmt_channel_t channel;
  
};

#endif