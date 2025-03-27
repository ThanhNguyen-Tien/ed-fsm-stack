#ifndef TEST_SUBJECT_A_H_
#define TEST_SUBJECT_A_H_

#include <core/observer.h>

extern obs_subject_t SubjectA_Subject;

void SubjectA_Init(void);

void SubjectA_SendData(int16_t value);

#endif /* TEST_SUBJECT_A_H_ */
