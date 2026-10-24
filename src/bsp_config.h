#ifndef __BSP_CONFIG_H
#define __BSP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define BSP_GPIO_CLK_ENABLE(port)                       \
  do {                                                  \
    if ((port) == GPIOA) { __HAL_RCC_GPIOA_CLK_ENABLE(); } \
    else if ((port) == GPIOB) { __HAL_RCC_GPIOB_CLK_ENABLE(); } \
    else if ((port) == GPIOC) { __HAL_RCC_GPIOC_CLK_ENABLE(); } \
    else if ((port) == GPIOD) { __HAL_RCC_GPIOD_CLK_ENABLE(); } \
  } while (0)

#define LED1_PORT               GPIOB
#define LED1_PIN                GPIO_PIN_12

#define LED2_PORT               GPIOB
#define LED2_PIN                GPIO_PIN_13

#define LED_ACTIVE_HIGH         1

#define BUZZER_PORT             GPIOB
#define BUZZER_PIN              GPIO_PIN_14
#define BUZZER_ACTIVE_HIGH      1

#define KEY_NUM                 4

#define KEY1_PORT               GPIOA
#define KEY1_PIN                GPIO_PIN_0

#define KEY2_PORT               GPIOA
#define KEY2_PIN                GPIO_PIN_1

#define KEY3_PORT               GPIOA
#define KEY3_PIN                GPIO_PIN_2

#define KEY4_PORT               GPIOA
#define KEY4_PIN                GPIO_PIN_3

#define KEY_PRESSED_LEVEL       GPIO_PIN_RESET
#define KEY_USE_INTERNAL_PULLUP 1
#define KEY_DEBOUNCE_MS         20u
#define KEY_LONG_MS             800u

#define SENSOR_NUM              5

#define S1_PORT                 GPIOA
#define S1_PIN                  GPIO_PIN_4

#define S2_PORT                 GPIOA
#define S2_PIN                  GPIO_PIN_5

#define S3_PORT                 GPIOB
#define S3_PIN                  GPIO_PIN_0

#define S4_PORT                 GPIOB
#define S4_PIN                  GPIO_PIN_1

#define S5_PORT                 GPIOB
#define S5_PIN                  GPIO_PIN_15

#define SENSOR_LINE_LEVEL       GPIO_PIN_RESET

#define AIN1_PORT               GPIOB
#define AIN1_PIN                GPIO_PIN_8

#define AIN2_PORT               GPIOB
#define AIN2_PIN                GPIO_PIN_9

#define BIN1_PORT               GPIOA
#define BIN1_PIN                GPIO_PIN_11

#define BIN2_PORT               GPIOA
#define BIN2_PIN                GPIO_PIN_12

#define ALT_HALF_MS             250u
#define ALT_ROUND_MS            30000u
#define ALT_ROUNDS              3u

#define SELFTEST_BLINK_TIMES    3u
#define SELFTEST_HALF_MS        120u

#ifdef __cplusplus
}
#endif

#endif /* __BSP_CONFIG_H */
