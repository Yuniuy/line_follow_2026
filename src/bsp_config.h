/**
  ******************************************************************************
  * @file    bsp_config.h
  * @brief   全工程唯一的硬件引脚映射表 + 可调参数（改接线只改这一个文件）
  *
  * 为什么单独抽一个文件：
  *   CubeMX 生成时没填 User Label，所以 main.h 里的 Private defines 是空的，
  *   代码里到处写 GPIO_PIN_12 过两周自己都不认识。这里集中定义一次，
  *   别的地方一律用 LED1_PIN / KEY1_PIN 这种名字。
  *
  * 【接线表】如果实物接线和这里不一样，只改本文件即可，不用动任何 .c
  *
  *   模块          信号        引脚
  *   ------------  ----------  ---------------------------
  *   LED1          LED1        PB12   （推挽输出）
  *   LED2          LED2        PB13   （推挽输出）
  *   蜂鸣器        BUZZER      PB14   （推挽输出，高电平响）
  *   按键 1        KEY1        PA0    （输入，内部上拉）
  *   按键 2        KEY2        PA1    （输入，内部上拉）
  *   按键 3        KEY3        PA2    （输入，内部上拉）
  *   按键 4        KEY4        PA3    （输入，内部上拉）
  *   巡线 1        S1          PA4    （数字输入，待硬件到货后核对）
  *   巡线 2        S2          PA5    （数字输入）
  *   巡线 3        S3          PB0    （数字输入）
  *   巡线 4        S4          PB1    （数字输入）
  *   巡线 5        S5          PB15   （数字输入）
  *   TB6612 PWMA   PWMA        PA6    （TIM3_CH1，硬件 PWM）
  *   TB6612 PWMB   PWMB        PA7    （TIM3_CH2，硬件 PWM）
  *   TB6612 AIN1   AIN1        PB8    （推挽输出）
  *   TB6612 AIN2   AIN2        PB9    （推挽输出）
  *   TB6612 BIN1   BIN1        PA11   （推挽输出）
  *   TB6612 BIN2   BIN2        PA12   （推挽输出）
  *   OLED          SCL/SDA     PB6 / PB7  （I2C1）
  *   串口          TX/RX       PA9 / PA10 （USART1，115200 8N1）
  ******************************************************************************
  */

#ifndef __BSP_CONFIG_H
#define __BSP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

/* ==========================================================================
 * 0. 通用小工具
 * ========================================================================== */

/** 按端口打开 GPIO 时钟。BSP 里重新配置引脚时用，
 *  不依赖 CubeMX 具体开了哪几个口，换引脚后也不会忘记开时钟。 */
#define BSP_GPIO_CLK_ENABLE(port)                       \
  do {                                                  \
    if ((port) == GPIOA) { __HAL_RCC_GPIOA_CLK_ENABLE(); } \
    else if ((port) == GPIOB) { __HAL_RCC_GPIOB_CLK_ENABLE(); } \
    else if ((port) == GPIOC) { __HAL_RCC_GPIOC_CLK_ENABLE(); } \
    else if ((port) == GPIOD) { __HAL_RCC_GPIOD_CLK_ENABLE(); } \
  } while (0)

/* ==========================================================================
 * 1. LED
 * ========================================================================== */

#define LED1_PORT               GPIOB
#define LED1_PIN                GPIO_PIN_12

#define LED2_PORT               GPIOB
#define LED2_PIN                GPIO_PIN_13

/** LED 极性（点亮时的电平）
 *  1 = 高电平点亮：GPIO 经限流电阻接 LED 正极，LED 负极接 GND
 *  0 = 低电平点亮：LED 正极接 VCC，负极接 GPIO（很多开发板是这种）
 *  接反了不会烧，只是「常亮/常灭」反过来，改这一个宏即可。 */
#define LED_ACTIVE_HIGH         1

/* ==========================================================================
 * 2. 蜂鸣器
 * ========================================================================== */

#define BUZZER_PORT             GPIOB
#define BUZZER_PIN              GPIO_PIN_14
#define BUZZER_ACTIVE_HIGH      1   /* 1 = 高电平响（有源蜂鸣器 + 三极管） */

/* ==========================================================================
 * 3. 按键
 * ========================================================================== */

#define KEY_NUM                 4

#define KEY1_PORT               GPIOA
#define KEY1_PIN                GPIO_PIN_0

#define KEY2_PORT               GPIOA
#define KEY2_PIN                GPIO_PIN_1

#define KEY3_PORT               GPIOA
#define KEY3_PIN                GPIO_PIN_2

#define KEY4_PORT               GPIOA
#define KEY4_PIN                GPIO_PIN_3

/** 按下时读到什么电平
 *  GPIO_PIN_RESET = 按下为低电平（按键一端接 GND，最常见）
 *  GPIO_PIN_SET   = 按下为高电平（按键一端接 VCC） */
#define KEY_PRESSED_LEVEL       GPIO_PIN_RESET

/** 是否给按键开内部上拉（CubeMX 里配的是 NOPULL，这里按实际接线补上）
 *  按键接 GND  -> 1（内部上拉，松开时是高电平）
 *  按键接 VCC  -> 0（必须外部下拉，或改成 GPIO_PULLDOWN，见 bsp_key.c） */
#define KEY_USE_INTERNAL_PULLUP 1

#define KEY_DEBOUNCE_MS         20u   /* 消抖时间，轻触按键 10~30ms 都行 */
#define KEY_LONG_MS             800u  /* 按住超过这个时间算长按 */

/* ==========================================================================
 * 4. 五路灰度巡线传感器（TCRT5000 数字输出）
 *    硬件还没到，先占位。等买到后：把探头照白纸 / 黑线各测一次，
 *    确认「压到黑线」时读出来是 0 还是 1，再改 SENSOR_LINE_LEVEL。
 * ========================================================================== */

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

/** 探头压在黑线上时的电平（多数模块是 0，指示灯亮） */
#define SENSOR_LINE_LEVEL       GPIO_PIN_RESET

/* ==========================================================================
 * 5. 电机驱动 TB6612FNG
 *    PWMA / PWMB 走 TIM3 硬件 PWM（PA6/PA7），已经在 CubeMX 里配好；
 *    下面 4 个方向脚是普通推挽输出。
 *    STBY 建议直接接 3.3V（常使能），省一个 IO。
 * ========================================================================== */

#define AIN1_PORT               GPIOB
#define AIN1_PIN                GPIO_PIN_8

#define AIN2_PORT               GPIOB
#define AIN2_PIN                GPIO_PIN_9

#define BIN1_PORT               GPIOA
#define BIN1_PIN                GPIO_PIN_11

#define BIN2_PORT               GPIOA
#define BIN2_PIN                GPIO_PIN_12

/* ==========================================================================
 * 6. 可调参数（LED 演示用，对应赛题 2a / 2b / 2c）
 * ========================================================================== */

#define ALT_HALF_MS             250u     /* 交替闪烁：每 250ms 换一次 */
#define ALT_ROUND_MS            30000u   /* 1 轮 = 30 秒 */
#define ALT_ROUNDS              3u       /* 共 3 轮（合计 90 秒） */

#define SELFTEST_BLINK_TIMES    3u       /* 上电自检：两灯同闪几次 */
#define SELFTEST_HALF_MS        120u     /* 自检半周期 */

#ifdef __cplusplus
}
#endif

#endif /* __BSP_CONFIG_H */
