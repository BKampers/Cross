#include "ApiStatus.h"


char UNINITIALIZED[] = "Uninitialized";
char OK[] = "OK";
char NOT_IMPLEMENTED[] = "Not implemented";
char INVALID_PARAMETER[] = "Invalid parameter";
char INVALID_ID[] = "Invalid id";
char OUT_OF_MEMORY[] = "Out of memory";
char BUFFER_OVERFLOW[] = "Buffer overflow";
char WRITE_ERROR[] = "Write error";


#define MIN(a, b) a < b ? a : b
#define MAX(a, b) a > b ? a : b


int min(int a, int b)
{
	return MIN(a, b);
}


int max(int a, int b)
{
	return MAX(a, b);
}


int minf(float a, float b)
{
	return MIN(a, b);
}


int maxf(float a, float b)
{
	return MAX(a, b);
}
