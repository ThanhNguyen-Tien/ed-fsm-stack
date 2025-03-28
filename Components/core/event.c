#include "event.h"
#include "engine.h"
#include "system.h"
#include <string.h>

/// @brief STATIC FUNCTION
static inline void EventQueue_Push(uint8_t val)
{
	*(engine.eventQueue.inPtr) = val;
	engine.eventQueue.inPtr++;
	if (engine.eventQueue.inPtr == engine.eventQueue.last) {engine.eventQueue.inPtr = engine.eventQueue.first;}
}

static inline uint8_t EventQueue_Pop()
{
	uint8_t ret = *(engine.eventQueue.outPtr);
	engine.eventQueue.outPtr++;
	if (engine.eventQueue.outPtr == engine.eventQueue.last) {engine.eventQueue.outPtr = engine.eventQueue.first;}
	return ret;
}

static inline void EventQueue_PopData(uint8_t* data, uint8_t size)
{
    if (engine.eventQueue.outPtr + size <= engine.eventQueue.last)
    {
        memcpy(data, engine.eventQueue.outPtr, size);
        engine.eventQueue.outPtr += size;
        if (engine.eventQueue.outPtr == engine.eventQueue.last)
        {
            engine.eventQueue.outPtr = engine.eventQueue.first;
        }
    }
    else
    {
        uint16_t firstPartSize = engine.eventQueue.last - engine.eventQueue.outPtr;
        memcpy(data, engine.eventQueue.outPtr, firstPartSize);
        memcpy(data + firstPartSize, engine.eventQueue.first, size - firstPartSize);
        engine.eventQueue.outPtr = engine.eventQueue.first + (size - firstPartSize);
    }
}

static inline void EventQueue_ExecuteEvent(event_t* ev)
{
	if(ev->size == 0)
	{
		ev->handler(NULL);
	}
	else
	{
		assert(ev->size <= engine.eventQueue.maxEvSize);

		EventQueue_PopData(engine.eventQueue.dataBuf, ev->size);
		ev->handler(engine.eventQueue.dataBuf);
		memset(engine.eventQueue.dataBuf, 0, ev->size);
	}
}

/// @brief PUBLIC FUNCTION
static uint8_t Event_Register_(event_t* event)
{
	assert(event != NULL);
	assert(engine.eventQueue.poolSize - 1 < engine.eventQueue.maxPoolSize);
	engine.eventQueue.events[engine.eventQueue.poolSize - 1] = event;
	return engine.eventQueue.poolSize++;
}

inline bool Event_Loop()
{
	if(engine.eventQueue.inPtr == engine.eventQueue.outPtr) return false;
	uint8_t index = EventQueue_Pop();
	assert(index < engine.eventQueue.poolSize);

#ifndef NDEBUG
	event_t * e = engine.eventQueue.events[index - 1];

	uint32_t exec_start = DWT->CYCCNT;
	EventQueue_ExecuteEvent(e);
	e->time.last_exec_time = DWT->CYCCNT - exec_start ;

	if (e->time.last_exec_time > e->time.max_time)
		e->time.max_time = e->time.last_exec_time;
	if (e->time.last_exec_time < e->time.min_time)
		e->time.min_time = e->time.last_exec_time;
#else
	event_t * e = engine.eventQueue.events[index - 1];
	EventQueue_ExecuteEvent(e);
#endif

	return true;
}

void Event_Init(event_t* ev, uint8_t size, EventHandler handler)
{
	assert(ev != NULL);
	ev->size = size;
	ev->handler = handler;
	ev->index = Event_Register_(ev);
#ifndef NDEBUG
	ev->time.last_exec_time = 0;
	ev->time.max_time = 0;
	ev->time.min_time = UINT32_MAX;
#endif
}

inline bool Event_Post(uint8_t index, void* data)
{
	if(index == 0) return false;
	uint8_t* ptr = (uint8_t *)data;
	uint16_t avail = engine.eventQueue.size - 1 + engine.eventQueue.outPtr - engine.eventQueue.inPtr;
	if(avail > engine.eventQueue.size) {avail -= engine.eventQueue.size;}
	if(avail < engine.eventQueue.events[index - 1]->size + 1) return false;

#ifndef NDEBUG
	if(avail < engine.eventQueue.minFree) engine.eventQueue.minFree = avail;
#endif

	DISABLE_INTERRUPT;

	EventQueue_Push(index);
	if(engine.eventQueue.events[index - 1]->size != 0)
	{
	    if (engine.eventQueue.inPtr + engine.eventQueue.events[index - 1]->size <= engine.eventQueue.last)
	    {
	        memcpy(engine.eventQueue.inPtr, ptr, engine.eventQueue.events[index - 1]->size);
	        engine.eventQueue.inPtr += engine.eventQueue.events[index - 1]->size;
	        if (engine.eventQueue.inPtr == engine.eventQueue.last)
	        {
	            engine.eventQueue.inPtr = engine.eventQueue.first;
	        }
	    }
	    else
	    {
	        uint16_t firstPartSize = engine.eventQueue.last - engine.eventQueue.inPtr;
	        memcpy(engine.eventQueue.inPtr, ptr, firstPartSize);
	        memcpy(engine.eventQueue.first, ptr + firstPartSize, engine.eventQueue.events[index - 1]->size - firstPartSize);
	        engine.eventQueue.inPtr = engine.eventQueue.first + (engine.eventQueue.events[index - 1]->size - firstPartSize);
	    }
	}

	ENABLE_INTERRUPT;
	return true;
}
