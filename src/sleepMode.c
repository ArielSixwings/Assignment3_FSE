#include "sleepMode.h"

void lightSleepInit() {
    if (CONFIG_LOW_POWER) {
        gpio_pad_select_gpio(WAKE_UP_BUTTON);
        gpio_set_direction(WAKE_UP_BUTTON, GPIO_MODE_INPUT);
        gpio_wakeup_enable(WAKE_UP_BUTTON, GPIO_INTR_LOW_LEVEL);
        esp_sleep_enable_gpio_wakeup();
        setSleepMode();
    }
}

void setSleepMode() {
    printf("Entrando em modo Light Sleep\n");

    // Configura o modo sleep somente após completar a escrita na UART para finalizar o printf
    uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);

    // Entra em modo Light Sleep
    esp_light_sleep_start();

    while (true) {
        if (rtc_gpio_get_level(WAKE_UP_BUTTON) == 0) break;
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    printf("Saindo do modo Light Sleep\n");
}