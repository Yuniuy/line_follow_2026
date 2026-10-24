/**
  ******************************************************************************
  * @file    bsp_key.c
  * @brief   4 路按键驱动：非阻塞扫描 + 软件消抖 + 短按/长按事件
  ******************************************************************************
  */

#include "bsp_key.h"

/* 单个按键的全部状态 */
typedef struct
{
  GPIO_TypeDef *port;
  uint16_t      pin;
  uint8_t       raw;          /* 上一次读到的原始状态（1 = 按下） */
  uint8_t       stable;       /* 消抖后的状态（1 = 按下） */
  uint8_t       long_fired;   /* 本次按住期间长按事件是否已经报过 */
  uint32_t      change_tick;  /* 原始状态最后一次变化的时刻 */
  uint32_t      press_tick;   /* 本次按下的时刻 */
  uint16_t      events;       /* 待处理事件位图 */
} key_obj_t;

/* 硬件表：换引脚只改 bsp_config.h，这里会自动跟着变。
   用「指定初始化」只写 port/pin，其余字段自动清零，
   编译时不会报 missing field initializer 警告。 */
static key_obj_t s_key[KEY_ID_NUM] =
{
  { .port = KEY1_PORT, .pin = KEY1_PIN },
  { .port = KEY2_PORT, .pin = KEY2_PIN },
  { .port = KEY3_PORT, .pin = KEY3_PIN },
  { .port = KEY4_PORT, .pin = KEY4_PIN },
};

/* 读引脚并归一化成「1 = 按下」 */
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

    /* CubeMX 里这些脚配的是「输入 + 无上下拉」。
       轻触按键另一端接 GND 时，没有上拉就会悬空乱跳，
       所以在这里按实际接线补上拉/下拉。 */
    gi.Pin  = k->pin;
    gi.Mode = GPIO_MODE_INPUT;
#if KEY_USE_INTERNAL_PULLUP
    gi.Pull = (KEY_PRESSED_LEVEL == GPIO_PIN_RESET) ? GPIO_PULLUP : GPIO_PULLDOWN;
#else
    gi.Pull = GPIO_NOPULL;
#endif
    HAL_GPIO_Init(k->port, &gi);

    /* 状态清零，时间戳取当前时刻，避免第一圈把 0 当成「很久没变化」 */
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

    /* 第一步：原始状态变了就重新计时（抖动期间会不断刷新时间戳） */
    if (raw != k->raw)
    {
      k->raw         = raw;
      k->change_tick = now;
    }

    /* 第二步：状态稳定超过消抖时间，才承认它真的变了 */
    if (((now - k->change_tick) >= KEY_DEBOUNCE_MS) && (k->stable != k->raw))
    {
      k->stable = k->raw;

      if (k->stable)                        /* 刚按下 */
      {
        k->press_tick = now;
        k->long_fired = 0;
        k->events    |= (uint16_t)KEY_EVENT_DOWN;
      }
      else                                  /* 刚松开 */
      {
        if (!k->long_fired)                 /* 没到长按时长的松开 = 短按 */
        {
          k->events |= (uint16_t)KEY_EVENT_CLICK;
        }
        k->events |= (uint16_t)KEY_EVENT_UP;
      }
    }

    /* 第三步：按住不放，到点补一个长按事件（只补一次） */
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
    s_key[id].events &= (uint16_t)(~(uint16_t)ev);   /* 读后即焚 */
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
