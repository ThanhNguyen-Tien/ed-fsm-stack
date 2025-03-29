#include "strand.h"
#include "engine.h"
#include <string.h>

static inline void Strand_Next(strand_t* s)
{
	assert(s != NULL);
    if (s->busy || Queue_IsEmpty(s->command)) return;
    strand_wrapper_t sw = {NULL};
    sw.s = s;
    Event_Post(s->execute.index, &sw);
    s->busy = true;
}

static inline void Strand_Timeout(strand_t* s)
{
	assert(s != NULL);
	s->busy = false;
	Strand_Next(s);
}

static inline void Strand_Execute(void* msg)
{
	strand_wrapper_t* sw = (strand_wrapper_t*) msg;

	uint8_t type = 0;
	(void)Queue_Pop(sw->s->command, &type);
	if (type == DELAY)
	{
		uint32_t time = 0;
		uint8_t  temp = 0;
		for(uint8_t i = 0; i < 4; i++)
		{
			(void)Queue_Pop(sw->s->command, &temp);
			time <<= ((3-i)*8);
			time += temp;
		}
		Task_Start(&sw->s->timer, time, 1);
		sw->s->finished = NULL;
		return;
	}
	else if (type == VOID)	{sw->s->finished = NULL;}
	else if (type == CALLBACK)
	{
		uint8_t index = 0;
		(void)Queue_Pop(sw->s->command, &index);
		assert(index < engine.eventQueue.poolSize);
		sw->s->finished = sw->s->evQueue->events[index-1];
	}
	uint8_t index = 0;
	(void)Queue_Pop(sw->s->command, &index);

	assert(index < engine.eventQueue.poolSize);

	event_t* e = sw->s->evQueue->events[index - 1];
	if(e->size == 0)
	{
		e->handler(NULL);
	}
	else
	{
		assert(e->size <= MAX_STRAND_EVENT_SIZE);

	    for (uint8_t i = 0; i < e->size; i++)
	    {
	        (void)Queue_Pop(sw->s->command, &sw->s->payload[i]);
	    }

		e->handler(sw->s->payload);
		memset(sw->s->payload, 0, e->size);
	}
}

void Strand_Init(strand_t* s, queue_t* q, uint8_t* payload)
{
	assert(q != NULL);
	assert(s != NULL);

	Engine_RegisterTask(&s->timer);
	s->timer.handler = &Strand_Timeout;

	Event_Init(&s->execute, sizeof(strand_wrapper_t), &Strand_Execute);
	s->finished = NULL;

	s->evQueue = &engine.eventQueue;

	s->command = q;
	s->payload = payload;
	s->busy = false;

}

void Strand_Post(strand_t* s, event_t* ev, event_t* cb, void* data)
{
	assert(s != NULL);
	assert(ev != NULL);
#ifndef NDEBUG
	if(cb != NULL) assert(cb->size == 1);
#endif

    if (Queue_CheckNumOfFree(s->command) < ev->size + 3) return;

    DISABLE_INTERRUPT;

    if (cb != NULL)
    {
    	Queue_Push(s->command, CALLBACK);
    	Queue_Push(s->command, cb->index);
    }
    else
    {
    	Queue_Push(s->command, VOID);
    }

    Queue_Push(s->command, ev->index);
    if(ev->size > 0)
    {
        uint8_t* ptr = (uint8_t*)data;
        for (size_t i = 0; i < ev->size; i++)
        {
        	Queue_Push(s->command, ptr[i]);
        }
    }

    ENABLE_INTERRUPT;
    Strand_Next(s);
}

void Strand_Delay(strand_t* s, uint32_t ms)
{
	assert(s != NULL);
    if (Queue_CheckNumOfFree(s->command) < 5) return;
    DISABLE_INTERRUPT;
    Queue_Push(s->command, DELAY);
    Queue_Push(s->command, (ms >> 24) & 0xFF);
    Queue_Push(s->command, (ms >> 16) & 0xFF);
    Queue_Push(s->command, (ms >> 8) & 0xFF);
    Queue_Push(s->command, ms & 0xFF);
    ENABLE_INTERRUPT;
    Strand_Next(s);
}

void Strand_Done(strand_t* s)
{
	assert(s != NULL);
    s->busy = false;
    Strand_Next(s);
}

void Strand_Done_With_Err(strand_t* s, uint8_t err)
{
	assert(s != NULL);
	s->busy = false;
    if (s->finished != NULL)
    {
    	Event_Post(s->finished->index, &err);
    }
	Strand_Next(s);
}
