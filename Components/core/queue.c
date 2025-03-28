#include "queue.h"
#include <assert.h>

void Queue_Init(queue_t* q, uint8_t* buf, uint16_t size)
{
	assert(q != NULL);

	q->first = buf;
	q->size = size;
	q->last = q->first + size;
	q->inPtr = q->first;
	q->outPtr = q->first;
}

void Queue_Reset(queue_t* q)
{
	assert(q != NULL);

	q->inPtr = q->first;
	q->outPtr = q->first;
}

uint16_t Queue_CheckNumOfFree(queue_t* q)
{
	assert(q != NULL);

	uint16_t ret = q->size + q->outPtr - q->inPtr;
	if (ret > q->size) ret -= q->size;
	return (ret - 1);
}

bool Queue_IsEmpty(queue_t* q)
{
	assert(q != NULL);

	return (q->inPtr == q->outPtr);
}

bool Queue_IsFull(queue_t* q)
{
	assert(q != NULL);

	return ((q->inPtr + 1 == q->outPtr) || (q->inPtr == q->last - 1 && q->outPtr == q->first));
}

bool Queue_Push(queue_t* q, uint8_t val)
{
	assert(q != NULL);

	uint8_t* next = q->inPtr + 1;
	if (next == q->last) next = q->first;
	if (next != q->outPtr)
	{
		*(q->inPtr) = val;
		q->inPtr = next;
		return true;
	}
	else return false;
}

bool Queue_Pop(queue_t* q, uint8_t* val)
{
	assert(q != NULL);
	assert(val != NULL);

	if (q->outPtr != q->inPtr)
	{
		*val = *(q->outPtr);
		q->outPtr++;
		if (q->outPtr == q->last) q->outPtr = q->first;
		return true;
	}
    return false;
}
