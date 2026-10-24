#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_config.h"

typedef enum
{
  KEY_ID_1 = 0,
  KEY_ID_2,
  KEY_ID_3,
  KEY_ID_4,
  KEY_ID_NUM
} key_id_t;

typedef enum
{
  KEY_EVENT_NONE  = 0,
  KEY_EVENT_DOWN  = 1u << 0,
  KEY_EVENT_UP    = 1u << 1,
  KEY_EVENT_CLICK = 1u << 2,
  KEY_EVENT_LONG  = 1u << 3
} key_event_t;

void      KEY_Init(void);
void      KEY_Scan(void);

uint8_t   KEY_TakeEvent(key_id_t id, key_event_t ev);
uint16_t  KEY_TakeAllEvents(key_id_t id);
uint8_t   KEY_IsPressed(key_id_t id);
void      KEY_ClearEvents(key_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_KEY_H */
