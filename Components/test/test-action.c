#include <test/test.h>
#include <console/log.h>
#include <test/subject-A.h>
#include <test/subject-B.h>

extern obs_node_t obsNode;

U_INTEGER_HANDLER(data)
{
	LOG_PRINTF("[U_INT] Data: %d", value);
}

U_ACTION_HANDLER(start)
{
	LOG_PRINT("[U_ACT] Start");
}

U_ACTION_HANDLER(a)
{
	LOG_PRINT("Register Subject A");
	Observer_AttachNode(&SubjectA_Subject, &obsNode);
}

U_ACTION_HANDLER(b)
{
	LOG_PRINT("Register Subject B");
	Observer_AttachNode(&SubjectB_Subject, &obsNode);
}

U_ACTION_HANDLER(stop)
{
	LOG_PRINT("[U_ACT] Stop");
	SM_EXECUTE(test, STOP);
}

U_ACTION_HANDLER(sayHello)
{
	LOG_PRINT("[U_ACT] HELLO WORLD");
}

U_TEXT_HANDLER(name)
{
	uint8_t buf[64];
	uint8_t i = 0;
	for(i = 0; i < length; i++)
	{
		buf[i] = data[i];
	}
	buf[i] = 0;

	LOG_PRINTF("[U_TXT]: %s", buf);
}

