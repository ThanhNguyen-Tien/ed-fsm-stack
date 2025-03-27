#include <test/subject-B.h>

obs_subject_t SubjectB_Subject;

void SubjectB_Init(void) {
    Observer_InitSubject(&SubjectB_Subject, sizeof(int16_t));
}

void SubjectB_SendData(int16_t value)
{
    Observer_Notify(&SubjectB_Subject, &value);
}
