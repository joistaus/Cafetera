#include <driver/gpio.h>
#include <ds18b20.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdbool.h>
#include <stdio.h>

#define LED_1 GPIO_NUM_2
#define MOTOR_1 GPIO_NUM_4
#define MOTOR_2 GPIO_NUM_16
#define CALENTADOR GPIO_NUM_5
#define TEMP_SENSOR GPIO_NUM_14
#define BUTTON_1 GPIO_NUM_22
#define BUTTON_2 GPIO_NUM_23

void setup(void) {
    gpio_config_t io_config;

    io_config.pin_bit_mask = (1 << LED_1) | (1 << MOTOR_1) | (1 << MOTOR_2) | (1 << CALENTADOR);
    io_config.mode = GPIO_MODE_OUTPUT;
    io_config.pull_up_en = GPIO_PULLUP_DISABLE;
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_config.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&io_config);

    io_config.pin_bit_mask = (1 << TEMP_SENSOR) | (1 << BUTTON_1) | (1 << BUTTON_2);
    io_config.mode = GPIO_MODE_INPUT;
    io_config.pull_up_en = GPIO_PULLUP_ENABLE;
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_config.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&io_config);

    ds18b20_init(TEMP_SENSOR);
}

enum State {
    NOT_DEFINED,
    OPTION_1,
    OPTION_2,
};

void app_main(void) {

    setup();

    enum State state;

    while (true) {
        float temperature = ds18b20_get_temp();

        printf("Temperature: %f\n", temperature);

        bool button_1 = !gpio_get_level(BUTTON_1);
        bool button_2 = !gpio_get_level(BUTTON_2);

        if (temperature < 42)
            gpio_set_level(CALENTADOR, 1);
        else if (temperature > 40)
            gpio_set_level(CALENTADOR, 0);

        state = NOT_DEFINED;

        if (button_1)
            state = OPTION_1;
        else if (button_2)
            state = OPTION_2;

        switch (state) {
        case OPTION_1:
            gpio_set_level(LED_1, 0);
            gpio_set_level(MOTOR_1, 1);
            vTaskDelay(2000 / portTICK_RATE_MS);

            gpio_set_level(MOTOR_1, 0);
            break;
        case OPTION_2:
            gpio_set_level(LED_1, 0);
            gpio_set_level(MOTOR_2, 1);
            vTaskDelay(1000 / portTICK_RATE_MS);

            gpio_set_level(MOTOR_2, 0);
            gpio_set_level(MOTOR_1, 1);
            vTaskDelay(1000 / portTICK_RATE_MS);

            gpio_set_level(MOTOR_1, 0);
            break;
        case NOT_DEFINED:
            gpio_set_level(LED_1, 1);
            break;
        }
    }
}