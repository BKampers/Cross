#ifndef _APISTATUS_H_
#define _APISTATUS_H_


#define RETURN_WHEN_INVALID Status _status;
#define VALIDATE(EXPRESSION) _status = (EXPRESSION); if (_status != OK) { return _status; }


typedef char* Status;

extern char OK[];
extern char DISABLED[];

extern char BUFFER_OVERFLOW[];
extern char INVALID_ID[];
extern char INVALID_PARAMETER[];
extern char NOT_IMPLEMENTED[];
extern char OUT_OF_MEMORY[];
extern char UNINITIALIZED[];
extern char WRITE_ERROR[];

int min(int a, int b);
int max(int a, int b);

float minf(float a, float b);
float maxf(float a, float b);

#endif /* _APISTATUS_H_ */
