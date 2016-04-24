#include "ApiStatus.h"


char UNINITIALIZED[] = "Uninitialized";
char OK[] = "OK";
char NOT_IMPLEMENTED[] = "NotImplemented";
char INVALID_PARAMETER[] = "InvalidParameter";
char INVALID_ID[] = "InvalidId";
char OUT_OF_MEMORY[] = "OutOfMemory";
char BUFFER_OVERFLOW[] = "BufferOverflow";
char WRITE_ERROR[] = "WriteError";


#define MIN(a, b) (a < b) ? a : b
#define MAX(a, b) (a > b) ? a : b


int min(int a, int b)
{
    return MIN(a, b);
}


int max(int a, int b)
{
    return MAX(a, b);
}


float minf(float a, float b)
{
    return MIN(a, b);
}


float maxf(float a, float b)
{
    return MAX(a, b);
}
