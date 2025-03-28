#include "engine.h"
#include "system.h"

engine_t engine;

#ifndef NDEBUG
monitor_cpu_t monitor = {0, 0, 0, 0.0f};
#endif

//Function Prototypes
static void Engine_ExecuteTask();

void Engine_Init(uint8_t* buf, uint16_t size, uint8_t* dataBuf, uint8_t maxEvSize, event_t** evPool, uint8_t evPoolSize)
{
	engine.eventQueue.size = size;
	engine.eventQueue.minFree = size;
	engine.eventQueue.first = buf;
	engine.eventQueue.last = engine.eventQueue.first + size;
	engine.eventQueue.inPtr = engine.eventQueue.first;
	engine.eventQueue.outPtr = engine.eventQueue.first;

	engine.eventQueue.dataBuf = dataBuf;
	engine.eventQueue.maxEvSize = maxEvSize;
	engine.eventQueue.events = evPool;
	engine.eventQueue.maxPoolSize = evPoolSize;
	engine.eventQueue.poolSize = 1;

	Event_Init(&engine.checkTask, 0, &Engine_ExecuteTask);

	engine.tickCount = 0;
	engine.nextTick  = LAST_TICK;
}

void Engine_Run()
{
#ifndef NDEBUG
	DWT_Init();
	monitor.last_cycle = DWT->CYCCNT;
	while(1)
	{
		if(Event_Loop())continue;

		uint32_t wfi_start = DWT->CYCCNT;
		WAIT_FOR_INTERUPT;
		monitor.wfi_cycles += (DWT->CYCCNT - wfi_start);

        uint32_t current_cycle = DWT->CYCCNT;
        monitor.total_cycles += (current_cycle - monitor.last_cycle);
        monitor.last_cycle = current_cycle;

        if (monitor.total_cycles >= HAL_RCC_GetSysClockFreq()) // 1 second
        {
            monitor.cpu_usage = 100.0f * (1.0f - ((float)monitor.wfi_cycles / monitor.total_cycles));
            monitor.total_cycles = 0;
            monitor.wfi_cycles = 0;
        }
	}
#else
	while(1)
	{
		if(Event_Loop())continue;
		WAIT_FOR_INTERUPT;
	}
#endif
}

void Engine_Delay(uint32_t t)
{
	uint64_t timeout = engine.tickCount + t;
	while (engine.tickCount < timeout){NO_OPERATION;}
}

void Engine_RegisterTask(task_t *task)
{
	assert(task != NULL);
	task->nextTick = 0;
	task->interval = 0;
	task->loop = -1;

	task->next = engine.taskLists;
	engine.taskLists = task;
}

void Engine_StartTask(task_t *task)
{
	assert(task != NULL);
	task_t* prev = NULL;
	task_t* it;
	for (it = engine.taskLists; it!= NULL; it = it->next)
	{
		if (it == task)
		{
			if (prev == NULL) engine.taskLists = it->next;
			else prev->next = it->next;
			task->next = engine.activeTasks;
			engine.activeTasks = task;
			break;
		}
		prev = it;
	}
	if(engine.nextTick > task->nextTick)engine.nextTick = task->nextTick;
}

void Engine_StopTask(task_t *task)
{
	assert(task != NULL);
	task_t* prev = NULL;
	task_t* it;
	for (it = engine.activeTasks; it!=NULL; it = it->next)
	{
		if (it == task)
		{
			if (prev == NULL) engine.activeTasks = it->next;
			else prev->next = it->next;
			task->next = engine.taskLists;
			engine.taskLists = task;
			break;
		}
		prev = it;
	}
}

static void Engine_ExecuteTask()	// Event Handler
{
	uint64_t min = LAST_TICK;
	task_t* it = engine.activeTasks;
	task_t* next;
	while (it != NULL)
	{
		next = it->next;
		if (engine.tickCount >= it->nextTick)
		{
			Task_Run(it);
		}
		if (min > it->nextTick) min = it->nextTick;
		it = next;
	}
	engine.nextTick = min;
}

inline void Engine_CheckTask()
{
	if(++engine.tickCount >= engine.nextTick)
	{
		Event_Post(engine.checkTask.index, NULL);
	}
}
