#include "bsp_key.h"

typedef struct
{
  GPIO_TypeDef *port;
  uint16_t      pin;
  uint8_t       raw;
  uint8_t       stable;
  uint8_t       long_fired;
  uint32_t      change_tick;
  uint32_t      press_tick;
  uint16_t      events;
} key_obj_t;

static key_obj_t s_key[KEY_ID_NUM] =
{
  { .port = KEY1_PORT, .pin = KEY1_PIN },
  { .port = KEY2_PORT, .pin = KEY2_PIN },
  { .port = KEY3_PORT, .pin = KEY3_PIN },
  { .port = KEY4_PORT, .pin = KEY4_PIN },
};

static uint8_t key_read_raw(const key_obj_t *k)
{
  return (HAL_GPIO_ReadPin(k->port, k->pin) == KEY_PRESSED_LEVEL) ? 1u : 0u;
}

void KEY_Init(void)
{
  GPIO_InitTypeDef gi = {0};

  for (uint32_t i = 0; i < (uint32_t)KEY_ID_NUM; i++)
  {
    key_obj_t *k = &s_key[i];

    BSP_GPIO_CLK_ENABLE(k->port);

    gi.Pin  = k->pin;
    gi.Mode = GPIO_MODE_INPUT;
#if KEY_USE_INTERNAL_PULLUP
    gi.Pull = (KEY_PRESSED_LEVEL == GPIO_PIN_RESET) ? GPIO_PULLUP : GPIO_PULLDOWN;
#else
    gi.Pull = GPIO_NOPULL;
#endif
    HAL_GPIO_Init(k->port, &gi);

    k->raw         = key_read_raw(k);
    k->stable      = k->raw;
    k->long_fired  = 0;
    k->change_tick = HAL_GetTick();
    k->press_tick  = k->change_tick;
    k->events      = 0;
  }
}

void KEY_Scan(void)
{
  uint32_t now = HAL_GetTick();

  for (uint32_t i = 0; i < (uint32_t)KEY_ID_NUM; i++)
  {
    key_obj_t *k = &s_key[i];
    uint8_t    raw = key_read_raw(k);

    if (raw != k->raw)
    {
      k->raw         = raw;
      k->change_tick = now;
    }

    if (((now - k->change_tick) >= KEY_DEBOUNCE_MS) && (k->stable != k->raw))
    {
      k->stable = k->raw;

      if (k->stable)
      {
        k->press_tick = now;
        k->long_fired = 0;
        k->events    |= (uint16_t)KEY_EVENT_DOWN;
      }
      else
      {
        if (!k->long_fired)
        {
          k->events |= (uint16_t)KEY_EVENT_CLICK;
        }
        k->events |= (uint16_t)KEY_EVENT_UP;
      }
    }

    if (k->stable && !k->long_fired && ((now - k->press_tick) >= KEY_LONG_MS))
    {
      k->long_fired  = 1;
      k->events     |= (uint16_t)KEY_EVENT_LONG;
    }
  }
}

uint8_t KEY_TakeEvent(key_id_t id, key_event_t ev)
{
  if ((uint32_t)id >= (uint32_t)KEY_ID_NUM)
  {
    return 0;
  }

  if ((s_key[id].events & (uint16_t)ev) != 0u)
  {
    s_key[id].events &= (uint16_t)(~(uint16_t)ev);
    return 1;
  }
  return 0;
}

uint16_t KEY_TakeAllEvents(key_id_t id)
{
  uint16_t ev;

  if ((uint32_t)id >= (uint32_t)KEY_ID_NUM)
  {
    return 0;
  }

  ev = s_key[id].events;
  s_key[id].events = 0;
  return ev;
}

uint8_t KEY_IsPressed(key_id_t id)
{
  if ((uint32_t)id >= (uint32_t)KEY_ID_NUM)
  {
    return 0;
  }
  return s_key[id].stable;
}

void KEY_ClearEvents(key_id_t id)
{
  if ((uint32_t)id >= (uint32_t)KEY_ID_NUM)
  {
    return;
  }
  s_key[id].events = 0;
}
