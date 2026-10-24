#ifndef __BSP_LED_H
#define __BSP_LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_config.h"

typedef enum
{
  LED_ID_1 = 0,
  LED_ID_2,
  LED_ID_NUM
} led_id_t;

void    LED_Init(void);
void    LED_On(led_id_t id);
void    LED_Off(led_id_t id);
void    LED_Toggle(led_id_t id);
void    LED_Set(led_id_t id, uint8_t on);
void    LED_AllOff(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_LED_H */
