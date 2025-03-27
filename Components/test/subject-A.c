#include <test/subject-A.h>

obs_subject_t SubjectA_Subject;

void SubjectA_Init(void) {
    Observer_InitSubject(&SubjectA_Subject, sizeof(int16_t));
}

void SubjectA_SendData(int16_t value)
{
    Observer_Notify(&SubjectA_Subject, &value);
}
