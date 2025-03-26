#include "machine.h"
#include <stddef.h>

static void Machine_NullState(machine_t* m){}

void Machine_Init(machine_t *m)
{
	m->nextState = &Machine_NullState;
	m->currentState = &Machine_NullState;
	m->nextEvent = 0;

	Event_Init(&m->executeEvent, sizeof(machine_event_t), (void*)&Machine_Execute);
}

inline void Machine_Execute(void* msg)
{
	machine_event_t* data = (machine_event_t*)msg;
	data->mPtr->nextState = NULL;
	data->mPtr->nextEvent = data->event;
	(*data->mPtr->currentState)(data->mPtr);

    if (data->mPtr->nextState != NULL && data->mPtr->nextState != data->mPtr->currentState)
    {
    	data->mPtr->nextEvent = EXIT_CURRENT_STATE; (*data->mPtr->currentState)(data->mPtr);
    	data->mPtr->currentState = data->mPtr->nextState;
        data->mPtr->nextEvent = ENTER_NEW_STATE; (*data->mPtr->currentState)(data->mPtr);
    }
}

void Machine_PostEvent(machine_t* m, uint8_t event)
{
	machine_event_t machineEvent;
	machineEvent.mPtr = m;
	machineEvent.event = event;
	Event_Post(m->executeEvent.index, &machineEvent);
}

void Machine_Start(machine_t* m, machineState s)
{
    m->currentState = s;
    m->nextEvent = ENTER_NEW_STATE;
    (*m->currentState)(m);
}

bool Machine_Check(machine_t* m, uint8_t input, machineState state)
{
    if (m->nextEvent != input) return false;
    m->nextState = state;
    return true;
}
