#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__

#include <stdint.h>
#include <stdbool.h>
#include "fsm_events.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EVENT_QUEUE_SIZE
#define EVENT_QUEUE_SIZE 16
#endif

typedef struct{
    PWM_Event_t queue[EVENT_QUEUE_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
}EventQueue_t;

static inline void EventQueue_Clear(EventQueue_t* q){
	q->head=0; 
	q->tail=0;
}
bool EnqueueEvent_ctx(EventQueue_t* q, PWM_Event_t evt);
bool DequeueEvent_ctx(EventQueue_t* q, PWM_Event_t* evt);

#ifdef __cplusplus
}
#endif
#endif //__EVENT_QUEUE_H__