/**
  ******************************************************************************
  * @file    bsp_key.h
  * @brief   4 路按键驱动：非阻塞扫描 + 软件消抖 + 短按/长按事件
  *
  * 用法（三重保险，照着写不会错）：
  *   1) main() 里初始化一次      KEY_Init();
  *   2) 主循环里每圈都调用        KEY_Scan();      // 不要加 delay
  *   3) 想用的时候取事件          if (KEY_TakeEvent(KEY_ID_1, KEY_EVENT_CLICK)) { ... }
  *
  * 为什么不用 HAL_Delay 消抖：
  *   赛题要求循迹循环里绝对不能阻塞。这里用「时间戳比对」实现消抖，
  *   KEY_Scan() 执行时间几微秒，跟主循环其它任务互不影响。
  ******************************************************************************
  */

#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_config.h"

/** 按键编号 */
typedef enum
{
  KEY_ID_1 = 0,
  KEY_ID_2,
  KEY_ID_3,
  KEY_ID_4,
  KEY_ID_NUM
} key_id_t;

/** 按键事件。故意做成位标志，一个键可以同时攒多个事件不会丢。 */
typedef enum
{
  KEY_EVENT_NONE  = 0,
  KEY_EVENT_DOWN  = 1u << 0,   /* 刚按下（消抖后） */
  KEY_EVENT_UP    = 1u << 1,   /* 刚松开 */
  KEY_EVENT_CLICK = 1u << 2,   /* 短按：按下后在 KEY_LONG_MS 内松开 */
  KEY_EVENT_LONG  = 1u << 3    /* 长按：按住超过 KEY_LONG_MS，只报一次 */
} key_event_t;

void      KEY_Init(void);
void      KEY_Scan(void);

/** 取某个事件：有则返回 1 并把它清掉（读后即焚，不会重复触发） */
uint8_t   KEY_TakeEvent(key_id_t id, key_event_t ev);

/** 一次取走该键所有待处理事件（位图），同时清空 */
uint16_t  KEY_TakeAllEvents(key_id_t id);

/** 当前是否处于按下状态（消抖后），用于「按住不放」的场景 */
uint8_t   KEY_IsPressed(key_id_t id);

/** 丢弃该键所有未处理事件（切模式时用，避免残留事件误触发） */
void      KEY_ClearEvents(key_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_KEY_H */
