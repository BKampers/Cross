#ifndef __TEXT_H__
#define	__TEXT_H__


typedef enum
{
    TEXT_OK,
    TEXT_OUT_OF_MEMORY
} TextStatus;


typedef struct 
{
    char* buffer;
    size_t capacity;
    size_t length;
} StringBuffer;


bool IsUnicodeControl(char character);

void InitializeStringBuffer(StringBuffer* stringBuffer);
TextStatus AppendString(StringBuffer* stringBuffer, const char* string);
TextStatus AppendSubstring(StringBuffer* stringBuffer, const char* string, size_t offset, size_t count);
TextStatus AppendCharacter(StringBuffer* stringBuffer, char character);
char* DetachString(StringBuffer* stringBuffer);


#endif	/* __TEXT_H__ */

