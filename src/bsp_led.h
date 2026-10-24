/**
  ******************************************************************************
  * @file    bsp_led.h
  * @brief   LED 驱动（两路，PB12 / PB13）
  ******************************************************************************
  */

#ifndef __BSP_LED_H
#define __BSP_LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_config.h"

/** LED 编号。代码里永远用 LED_ID_1 / LED_ID_2，不要直接写 0 和 1。 */
typedef enum
{
  LED_ID_1 = 0,   /* PB12，串口 / 状态指示常用这颗 */
  LED_ID_2,       /* PB13 */
  LED_ID_NUM
} led_id_t;

void    LED_Init(void);                          /* 上电全部熄灭 */
void    LED_On(led_id_t id);
void    LED_Off(led_id_t id);
void    LED_Toggle(led_id_t id);
void    LED_Set(led_id_t id, uint8_t on);        /* on != 0 点亮 */
void    LED_AllOff(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_LED_H */
