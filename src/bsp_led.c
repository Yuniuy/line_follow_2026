#include "bsp_led.h"

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
  LED_AllOff();
}

void LED_Set(led_id_t id, uint8_t on)
{
  if ((uint32_t)id >= (uint32_t)LED_ID_NUM)
  {
    return;
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
  s_led_hw[id].port->ODR ^= s_led_hw[id].pin;
}

void LED_AllOff(void)
{
  for (uint32_t i = 0; i < (uint32_t)LED_ID_NUM; i++)
  {
    LED_Set((led_id_t)i, 0);
  }
}
