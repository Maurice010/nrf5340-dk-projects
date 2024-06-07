#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

#ifdef CONFIG_LEDCONTROL
#include "ledcontrol.h"
#endif

#define SLEEP_TIME_MS 10*60*1000

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

#define SW0_NODE DT_ALIAS(sw0)
#define SW1_NODE DT_ALIAS(sw1)
#define SW2_NODE DT_ALIAS(sw2)
#define SW3_NODE DT_ALIAS(sw3)

const struct gpio_dt_spec leds[] = {
    GPIO_DT_SPEC_GET(LED0_NODE, gpios),
    GPIO_DT_SPEC_GET(LED1_NODE, gpios),
    GPIO_DT_SPEC_GET(LED2_NODE, gpios),
    GPIO_DT_SPEC_GET(LED3_NODE, gpios)
};

static const struct gpio_dt_spec buttons[] = {
    GPIO_DT_SPEC_GET(SW0_NODE, gpios),
    GPIO_DT_SPEC_GET(SW1_NODE, gpios),
    GPIO_DT_SPEC_GET(SW2_NODE, gpios),
    GPIO_DT_SPEC_GET(SW3_NODE, gpios)
};

int main(void)
{
    int ret = leds_init(leds, 4);
    if (ret < 0) {
        return 0;
    }
    
    ret = buttons_init(buttons, 4);
    if (ret < 0) {
        return 0;
    }
    
    while (1) {
        
    }

    return 0;
}