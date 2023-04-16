#ifndef __CABLE_RELEASE_H
#define __CABLE_RELEASE_H 

#define CABLE_RELEASE_SEQ_SIZE      32
#define CABLE_RELEASE_CODES_SIZE    32

// Sequence Commands
#define CABLE_RELEASE_END           0
#define CABLE_RELEASE_FOCUS_ON      1
#define CABLE_RELEASE_FOCUS_OFF     2
#define CABLE_RELEASE_SHUTTER_ON    3
#define CABLE_RELEASE_SHUTTER_OFF   4
#define CABLE_RELEASE_DELAY         5


typedef struct {
  String name;
  uint8_t cable_seq[CABLE_RELEASE_SEQ_SIZE];
} cable_release_seq_t;

// EVENTS
typedef enum {
  CABLE_RELEASE_EVT_NONE = 0,
  CABLE_RELEASE_EVT_START = 1,
} cable_release_evt_t;

typedef enum {
  cable_release_code_basic,
  cable_release_code_empty        // empty must be the last in the enum list
} cable_release_codes_t;


class CableRelease {
  public:
    void begin(gpio_num_t shutter, gpio_num_t focus);
    const cable_release_seq_t * getCode(cable_release_codes_t code);
    void set_seq(uint8_t *seq);
    void sm(cable_release_evt_t evt);

  private:
    gpio_num_t pin_shutter;
    gpio_num_t pin_focus;
    uint8_t current_cmd = 0;
    uint8_t *current_seq = NULL;
    bool seq_active = false;
};

#endif
