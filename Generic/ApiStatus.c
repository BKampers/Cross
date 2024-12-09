#include "ApiStatus.h"


char DISABLED[] = "Disabled";
char OK[] = "OK";

char BUFFER_OVERFLOW[] = "BufferOverflow";
char INVALID_ID[] = "InvalidId";
char INVALID_PARAMETER[] = "InvalidParameter";
char NOT_IMPLEMENTED[] = "NotImplemented";
char OUT_OF_MEMORY[] = "OutOfMemory";
char UNINITIALIZED[] = "Uninitialized";
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
