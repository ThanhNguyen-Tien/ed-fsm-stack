#include "observer.h"
#include <stddef.h>

void Observer_InitSubject(obs_subject_t* sub, uint8_t sizeOfData)
{
	sub->head->ev = NULL;
	sub->head->next = NULL;
	sub->head->type = PUSH_TO_QUEUE;
	sub->sizeOfData = sizeOfData;
}

void Observer_InitNode(obs_node_t* obs, event_t* ev, obs_handler_type_t type)
{
	obs->ev = ev;
	obs->type = type;
}

bool Observer_AttachNode(obs_subject_t* sub, obs_node_t* node)
{
	if (node->ev->size != sub->sizeOfData) return false;
    node->next = sub->head;
    sub->head = node;
    return true;
}

void Observer_DetachNode(obs_subject_t* sub, obs_node_t* node)
{
    obs_node_t** current = &(sub->head);
    while (*current != NULL)
    {
        if (*current == node)
        {
            *current = node->next;
            return;
        }
        current = &((*current)->next);
    }
}

void Observer_Notify(obs_subject_t* sub, void* data)
{
    obs_node_t* current = sub->head;

    while (current != NULL)
    {
        if (current->ev->handler != NULL)
        {
        	if(current->type == PUSH_TO_QUEUE)
        	{
        		Event_Post(current->ev->index, data);
        	}
        	else if(current->type == CALL_IMMEDIATLY)
        	{
        		current->ev->handler(data);
        	}
        	else
        	{
        		//TODO: Error_Handler
        	}
        }
        current = current->next;
    }
}
