/**
  ******************************************************************************
  * @file    bsp_led.c
  * @brief   LED 驱动（两路，PB12 / PB13）
  ******************************************************************************
  */

#include "bsp_led.h"

/* 硬件表：以后加第三颗灯，只要在 bsp_config.h 加宏、这里加一行 */
typedef struct
{
  GPIO_TypeDef *port;
  uint16_t      pin;
} led_hw_t;

static const led_hw_t s_led_hw[LED_ID_NUM] =
{
  { LED1_PORT, LED1_PIN },
  { LED2_PORT, LED2_PIN },
};

/* 把「亮/灭」翻译成实际电平，极性由 LED_ACTIVE_HIGH 决定 */
static GPIO_PinState led_level(uint8_t on)
{
#if LED_ACTIVE_HIGH
  return on ? GPIO_PIN_SET : GPIO_PIN_RESET;
#else
  return on ? GPIO_PIN_RESET : GPIO_PIN_SET;
#endif
}

void LED_Init(void)
{
  /* 引脚方向已在 CubeMX 的 MX_GPIO_Init() 里配成推挽输出，
     这里只负责把初始状态设成「全灭」，保证上电不出现半亮的怪状态。 */
  LED_AllOff();
}

void LED_Set(led_id_t id, uint8_t on)
{
  if ((uint32_t)id >= (uint32_t)LED_ID_NUM)
  {
    return;   /* 防御：传了非法编号直接忽略，不要越界写数组 */
  }
  HAL_GPIO_WritePin(s_led_hw[id].port, s_led_hw[id].pin, led_level(on ? 1u : 0u));
}

void LED_On(led_id_t id)
{
  LED_Set(id, 1);
}

void LED_Off(led_id_t id)
{
  LED_Set(id, 0);
}

void LED_Toggle(led_id_t id)
{
  if ((uint32_t)id >= (uint32_t)LED_ID_NUM)
  {
    return;
  }
  /* 直接对输出寄存器取反，比「读电平再写电平」少一次读操作 */
  s_led_hw[id].port->ODR ^= s_led_hw[id].pin;
}

void LED_AllOff(void)
{
  for (uint32_t i = 0; i < (uint32_t)LED_ID_NUM; i++)
  {
    LED_Set((led_id_t)i, 0);
  }
}
