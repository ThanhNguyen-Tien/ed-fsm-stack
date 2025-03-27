#ifndef TEST_SUBJECT_B_H_
#define TEST_SUBJECT_B_H_

#include <core/observer.h>

extern obs_subject_t SubjectB_Subject;

void SubjectB_Init(void);

void SubjectB_SendData(int16_t value);

#endif /* TEST_SUBJECT_B_H_ */
