#include "observer.h"

void Observer_InitSubject(obs_subject_t* sub, uint8_t sizeOfData)
{
	sub->head = NULL;
	sub->sizeOfData = sizeOfData;
}

void Observer_InitNode(obs_node_t* obs, event_t* ev, obs_handler_type_t type)
{
	obs->ev = ev;
	obs->type = type;
	obs->next = NULL;
}

void Observer_AttachNode(obs_subject_t* sub, obs_node_t* node)
{
    assert(sub != NULL);
    assert(node != NULL);
    assert(node->ev != NULL);
    assert(node->ev->size == sub->sizeOfData);

    if(node->registeredSubject == sub) return;
	if (node->registeredSubject != NULL)
	{
	   Observer_DetachNode(node->registeredSubject, node);
	}

	node->registeredSubject = sub;
    node->next = sub->head;
    sub->head = node;
}

void Observer_DetachNode(obs_subject_t* sub, obs_node_t* node)
{
	assert(sub != NULL);
	assert(node != NULL);

    obs_node_t** current = &(sub->head);

    while (*current != NULL)
    {
        if (*current == node)
        {
            *current = node->next;
            node->registeredSubject = NULL;
            return;
        }
        current = &((*current)->next);
    }
}

void Observer_Notify(obs_subject_t* sub, void* data)
{
    assert(sub != NULL);
    assert(data != NULL);

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
        }
        current = current->next;
    }
}
