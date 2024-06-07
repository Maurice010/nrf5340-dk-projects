#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#define SLEEP_TIME_MS 1000

extern const struct gpio_dt_spec leds[];

int leds_init(const struct gpio_dt_spec* leds, int leds_size);
int leds_default(const struct gpio_dt_spec* leds, int leds_size);

void button0_relased(const struct device* dev, struct gpio_callback* cb, uint32_t pins);
void button0_pressed(const struct device* dev, struct gpio_callback* cb, uint32_t pins);
int buttons_init(const struct gpio_dt_spec* buttons, int buttons_size);

#endif