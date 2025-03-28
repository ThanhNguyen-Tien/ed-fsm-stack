#include "system.h"
#include "engine.h"

uint8_t evQueue[EVENT_QUEUE_SIZE];
uint8_t tempDataForHandler[MAX_EVENT_SIZE];
event_t* evPool[EVENT_POOL_SIZE];

void SysTick_Handler(void)	//1ms
{
  Engine_CheckTask();
}

void System_Init()
{
	HAL_Init();
	SystemClock_Config();
}

void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Bật Debug Exception and Monitor Control
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;           // Bật Cycle Counter
    DWT->CYCCNT = 0;                               // Reset Counter
}

