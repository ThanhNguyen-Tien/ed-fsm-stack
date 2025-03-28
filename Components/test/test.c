#include "test.h"
#include <console/controller.h>
#include <console/log.h>
#include <core/machine.h>
#include <core/observer.h>
#include <stdbool.h>
#include <test/subject-A.h>
#include <test/subject-B.h>
#include "math.h"

#define NUM_OF_DATA	400

M_TASK(timeout)
M_TASK(plot)

M_EVENT(empty)
M_EVENT(fixed1)
M_EVENT(fixed2)

U_ACTION(start)
U_ACTION(stop)
U_ACTION(a)
U_ACTION(b)
U_ACTION(sayHello)
U_INTEGER(data)
U_TEXT(name)

MACHINE(test)

M_OBS_NODE(obs)

// Data to plot
int16_t sine[NUM_OF_DATA];
int16_t cosine[NUM_OF_DATA];

void Test_Init()
{
	ConsoleCtrl_Init();

	//Register Console Command
	CONSOLE_REGISTER_COMMAND(100, data);
	CONSOLE_REGISTER_COMMAND(101, start);
	CONSOLE_REGISTER_COMMAND(102, stop);
	CONSOLE_REGISTER_COMMAND(103, a);
	CONSOLE_REGISTER_COMMAND(104, b);
	CONSOLE_REGISTER_COMMAND(105, name);
	CONSOLE_REGISTER_COMMAND(106, sayHello);

	//Init for M_TASK
	M_TASK_INIT(timeout);
	M_TASK_INIT(plot);

	//Init for M_EVENT
	M_EVENT_INIT(empty);
	M_EVENT_INIT(fixed1, sizeof(fake_data_t));
	M_EVENT_INIT(fixed2, sizeof(uint32_t));

	//Init Observer Subject
	SubjectA_Init();
	SubjectB_Init();

	//Init Observer Node
	M_OBS_NODE_INIT(obs, int16_t, PUSH_TO_QUEUE)

	//Register Node with Subject
	Observer_AttachNode(&SubjectA_Subject, &obsNode);
	Observer_AttachNode(&SubjectA_Subject, &obsNode);

	//Start M_TASK
	M_TASK_START(plot, 2);

	for(int i = 0; i< NUM_OF_DATA; i++)
	{
		double v = ((double)i/(NUM_OF_DATA>>1)) * M_PI;
		int16_t sine_ = (int16_t)(sin(v) * 512);
		int16_t cosine_ = (int16_t)(cos(v) * 512);
		sine[i] = sine_ + 512;
		cosine[i] = cosine_ + 512;
	}

	//Start Machine
	SM_START(test, StartUp);
}

M_TASK_HANDLER(timeout)
{
	SM_POST(test, TIMEOUT);
	M_EVENT_POST(empty);
}

M_TASK_HANDLER(plot)
{
	static uint16_t angle;
	MC_PLOT(0,sine[angle]);
	MC_PLOT(1,cosine[angle]);
	if(++angle >= 400)angle = 0;
}

M_OBS_NODE_HANDLER(obs)
{
	int16_t* data_ = (int16_t*)data;
	LOG_PRINTF("[OBS_H] Data: %d", *data_);
}

M_EVENT_HANDLER(empty)
{
	static uint32_t count = 0;
	fake_data_t fake_;
	fake_.fake1 = count++;
	fake_.fake2 = (fake_.fake1 % 2 == 0)? 10 : -10;
	fake_.fake3 = -fake_.fake2;
	fake_.fake4 = sizeof(fake_);

	M_EVENT_POST(fixed1, fake_);
	M_EVENT_POST(fixed2, count);
}

M_EVENT_HANDLER(fixed1)
{
	fake_data_t* data_ = (fake_data_t*) data;
	LOG_PRINTF("[EV_H] Fixed1 Event: %d %d %d",
			(*data_).fake2,
			(*data_).fake3,
			(*data_).fake4);
	MC_PLOT(2, (*data_).fake2);
	MC_PLOT(3, (*data_).fake3);
}

M_EVENT_HANDLER(fixed2)
{
	uint32_t* data_ = (uint32_t*) data;
	LOG_PRINTF("[EV_H]: Fixed2 Event: %d", *data_);
}
