#include "event_queue.h"

bool EnqueueEvent_ctx(EventQueue_t* q, PWM_Event_t evt){
    uint8_t next = (uint8_t)(q->head + 1) % EVENT_QUEUE_SIZE;
    if(next == q->tail) return false;
    q->queue[q->head] = evt;
    q->head = next;
    return true;
}
bool DequeueEvent_ctx(EventQueue_t* q, PWM_Event_t* evt){
    if(q->head == q->tail) return false;
    *evt = q->queue[q->tail];
    q->tail = (q->tail + 1) % EVENT_QUEUE_SIZE;
    return true;
}
