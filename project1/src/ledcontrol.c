#include "ledcontrol.h"

LOG_MODULE_REGISTER(ledcontrol, LOG_LEVEL_DBG);

typedef enum {
    STATE_NORMAL,
    STATE_CONFIG,
    STATE_TOGGLE_LED,
    STATE_TIME_CONTROL,
    STATE_RESET
} State;

static State current_state = STATE_NORMAL;
static int current_led = 0;

int leds_init(const struct gpio_dt_spec* leds, int leds_size) {
    int ret;
    for (int i = 0; i < leds_size; i++) {
        if (!gpio_is_ready_dt(&leds[i])) {
            LOG_ERR("LED %d not ready", i);
            return -1;
        }

        ret = gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_INACTIVE);
        if (ret < 0) {
            LOG_ERR("Failed to configure LED %d", i);
            return -1;
        }
    }

    LOG_INF("LEDs initialized");
    return 1;
}

int leds_default(const struct gpio_dt_spec* leds, int leds_size) {
    int ret;
    for (int i = 0; i < leds_size; i++) {
        ret = gpio_pin_toggle_dt(&leds[i]);
        if (ret < 0) {
            LOG_ERR("Failed to toggle LED %d", i);
            return -1;
        }

        k_msleep(SLEEP_TIME_MS);
    }

    return 1;
}

int leds_config_dif(const struct gpio_dt_spec* leds, int leds_size) {
    int ret;
    for (int i = 0; i < leds_size; i++) {
        if (i == 0) {
            ret = gpio_pin_set_dt(&leds[i], 1);
            if (ret < 0) {
                LOG_ERR("Failed to turn off LED %d", i);
                return -1;
            }
        }

        ret = gpio_pin_set_dt(&leds[i], 0);
        if (ret < 0) {
            LOG_ERR("Failed to turn off LED %d", i);
            return -1;
        }
    }

    return 1;
}

int leds_pattern(const struct gpio_dt_spec* leds, int leds_size) {
    switch (current_state) {
        case STATE_NORMAL:
            break;
        case STATE_CONFIG:
            break;
        case STATE_TOGGLE_LED:
            break;
        case STATE_TIME_CONTROL:
            break;
        case STATE_RESET:
            break;
    }
}

/*
Led config mode that allows to change loadingLED() pattern
    - hold button0 to enter/exit config mode (all leds will blink)
    - use button0 to toggle between leds (if led is chosen it will blink)
    - use button1 to turn on/off chosen led
    - use button2 to turn timer on/off - its like configuring one step with button0 and button1 and then choosing how long this step will take
    - press button3 to reset to default
    - print a message if something has changed
*/

/* Callback functions for button0 */
static struct gpio_callback button0_press_cb;
static struct gpio_callback button0_release_cb;
static uint64_t time_stamp;

void button0_pressed(const struct device* dev, struct gpio_callback* cb, uint32_t pins) {
    LOG_INF("Button0 pressed");
    time_stamp = k_uptime_get();
}

void button0_relased(const struct device* dev, struct gpio_callback* cb, uint32_t pins) {
    LOG_INF("Button0 released");
    int64_t delta_time = k_uptime_delta(&time_stamp);
    if (delta_time > 3000) {
        LOG_INF("Time elapsed: %lld ms, start: %llu", delta_time, time_stamp);
        if (current_state == STATE_CONFIG) {
            current_state = STATE_NORMAL;
        }
        else if (current_state == STATE_NORMAL) {
            current_state = STATE_CONFIG;
            leds_config_dif(leds, 4);
        }
    }
    else {
        if (current_state == STATE_CONFIG) {
            current_led = (current_led + 1) % 4;

        }
    }
}

/* Callback functions for button1 */
static struct gpio_callback button1_press_cb;

void button1_pressed(const struct device* dev, struct gpio_callback* cb, uint32_t pins) {
    LOG_INF("Button1 pressed");
    if (current_state == STATE_TIME_CONTROL) {
        current_state = STATE_TOGGLE_LED;
    }
}

/* Callback functions for button2 */
static struct gpio_callback button2_press_cb;

void button2_pressed(const struct device* dev, struct gpio_callback* cb, uint32_t pins) {
    LOG_INF("Button2 pressed");
    if (current_state == STATE_CONFIG) {
        current_state = STATE_TIME_CONTROL;
    }
    else if (current_state == STATE_TIME_CONTROL) {
        current_state = STATE_CONFIG;
    }
}
/* Callback functions for button3 */
static struct gpio_callback button3_press_cb;

void button3_pressed(const struct device* dev, struct gpio_callback* cb, uint32_t pins) {
    LOG_INF("Button3 pressed");
    if (current_state == STATE_CONFIG) {
        current_state = STATE_RESET;
    }
}

/* Buttons initialization and interrupt config */
int buttons_init(const struct gpio_dt_spec* buttons, int buttons_size) {
    int ret;
    for (int i = 0; i < buttons_size; i++) {
        if (!gpio_is_ready_dt(&buttons[i])) {
            LOG_ERR("Button %d not ready", i);
            return -1;
        }

        ret = gpio_pin_configure_dt(&buttons[i], GPIO_INPUT);
        if (ret < 0) {
            LOG_ERR("Button %d not ready", i);
            return -1;
        }

        if (i == 0) {
            ret = gpio_pin_interrupt_configure_dt(&buttons[i], GPIO_INT_EDGE_TO_ACTIVE | GPIO_INT_EDGE_TO_INACTIVE);
            if (ret < 0) {
                LOG_ERR("Failed to configure interrupt for button %d", i);
                return -1;
            }

            gpio_init_callback(&button0_press_cb, button0_pressed, BIT(buttons[i].pin));
            gpio_add_callback(buttons[i].port, &button0_press_cb);

            gpio_init_callback(&button0_release_cb, button0_relased, BIT(buttons[i].pin));
            gpio_add_callback(buttons[i].port, &button0_release_cb);

            LOG_INF("Button %d configured", i);
        }
        else if (i == 1) {
            ret = gpio_pin_interrupt_configure_dt(&buttons[i], GPIO_INT_EDGE_TO_ACTIVE);
            if (ret < 0) {
                LOG_ERR("Failed to configure interrupt for button %d", i);
                return -1;
            }

            gpio_init_callback(&button1_press_cb, button1_pressed, BIT(buttons[i].pin));
            gpio_add_callback(buttons[i].port, &button1_press_cb);

            LOG_INF("Button %d configured", i);
        }
        else if (i == 2) {
            ret = gpio_pin_interrupt_configure_dt(&buttons[i], GPIO_INT_EDGE_TO_ACTIVE);
            if (ret < 0) {
                LOG_ERR("Failed to configure interrupt for button %d", i);
                return -1;
            }

            gpio_init_callback(&button2_press_cb, button2_pressed, BIT(buttons[i].pin));
            gpio_add_callback(buttons[i].port, &button2_press_cb);

            LOG_INF("Button %d configured", i);
        }
        else if (i == 3) {
            ret = gpio_pin_interrupt_configure_dt(&buttons[i], GPIO_INT_EDGE_TO_ACTIVE);
            if (ret < 0) {
                LOG_ERR("Failed to configure interrupt for button %d", i);
                return -1;
            }

            gpio_init_callback(&button3_press_cb, button3_pressed, BIT(buttons[i].pin));
            gpio_add_callback(buttons[i].port, &button3_press_cb);

            LOG_INF("Button %d configured", i);
        }
    }

    return 1;
}

/* event detection logic */

/* handlers for different states */