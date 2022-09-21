#include "lightSleep.h"


void lightSleepInit() {
    gpio_pad_select_gpio(WAKE_UP_BUTTON);
    gpio_set_direction(WAKE_UP_BUTTON, GPIO_MODE_INPUT);
    gpio_wakeup_enable(WAKE_UP_BUTTON, GPIO_INTR_LOW_LEVEL);
}

// void setSleepMode() {
//   while(true) {
//     if (rtc_gpio_get_level(WAKE_UP_BUTTON) == 0) {
//         printf("Aguardando soltar o botão ... \n");
//         do {
//             vTaskDelay(pdMS_TO_TICKS(10));
//         } while (rtc_gpio_get_level(WAKE_UP_BUTTON) == 0);
//     }

//     printf("Entrando em modo Light Sleep\n");

//     // Configura o modo sleep somente após completar a escrita na UART para finalizar o printf
//     uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);

//     int64_t tempo_antes_de_dormir = esp_timer_get_time();

//     // Entra em modo Light Sleep
//     esp_light_sleep_start();

//     int64_t tempo_apos_acordar = esp_timer_get_time();
//   }

// }