#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include "main.h"
#include "event.h"

#define DISABLE_INTERRUPT   __disable_irq()
#define ENABLE_INTERRUPT    __enable_irq()
#define WAIT_FOR_INTERUPT   __WFI()
#define NO_OPERATION		__NOP()

#define EVENT_POOL_SIZE     	32
#define EVENT_QUEUE_SIZE    	128
#define MAX_EVENT_SIZE			32
#define MAX_STRAND_EVENT_SIZE	32	// Optional

extern uint8_t evQueue[EVENT_QUEUE_SIZE];
extern uint8_t tempDataForHandler[MAX_EVENT_SIZE];
extern event_t* evPool[EVENT_POOL_SIZE];

extern void SystemClock_Config();
void System_Init();
void DWT_Init(void);

#endif // SYSTEM_H
