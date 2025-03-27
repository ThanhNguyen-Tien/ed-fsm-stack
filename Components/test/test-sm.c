#include <test/test.h>
#include <console/log.h>
#include <test/subject-A.h>
#include <test/subject-B.h>

STATE_BODY(StartUp)
{
	ENTER()
	{
		LOG_PRINT("[SM] StartUp");
		M_TASK_START(timeout, 2000, 1);
	}
	TRANSITION_(TIMEOUT)
	{
		SM_POST(test, START);
	}
	TRANSITION_(START, Running){}
}

STATE_BODY(Running)
{
	ENTER()
	{
		LOG_PRINT("[SM] Running");
		M_TASK_START(timeout, 2000, 5);
	}
	TRANSITION_(TIMEOUT)
	{
		static uint8_t count = 0;
		if(++count == 5)
		{
			SM_SWITCH(Stop);
			count = 0;
		}
		SubjectA_SendData(count);
		SubjectB_SendData(100 - count);
	}
	TRANSITION_(STOP, Stop){}
}

STATE_BODY(Stop)
{
	ENTER()
	{
		LOG_PRINT("[SM] Stop");
		M_TASK_START(timeout, 2000, 1);
	}
	TRANSITION_(TIMEOUT, StartUp){}
}
