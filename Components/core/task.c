#include "engine.h"

void Task_Start(task_t* task, uint32_t interval, int32_t loop)
{
	assert(task != NULL);

	task->interval = interval;
	task->nextTick = engine.tickCount + interval;
	task->loop = loop;
	Engine_StartTask(task);
}

void Task_Stop(task_t* task)
{
	assert(task != NULL);

	task->loop = 0;
	Engine_StopTask(task);
	task->nextTick = LAST_TICK - 1;
}

bool Task_IsRunning(task_t* task)
{
	assert(task != NULL);

	return (task->loop!=0);
}

void Task_Run(task_t* task)
{
	assert(task != NULL);

	if (--task->loop == 0)
	{
		Engine_StopTask(task);
		task->nextTick = LAST_TICK - 1;
	}
	else
	{
		task->nextTick += task->interval;
		if (task->loop < 0) task->loop = -1;
	}

	task->handler();
}
