#include "Json.h"


char JSON_NULL_LITERAL[] = "null";
char JSON_FALSE_LITERAL[] = "false";
char JSON_TRUE_LITERAL[] = "true";


bool IsUnicodeControl(char character)
{
    return (character <= (char) 0x1F) || (((char) 0x7F <= character) && (character <= (char) 0x9F));
}
