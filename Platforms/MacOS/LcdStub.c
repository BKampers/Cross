#include "Lcd.h"

#include "ApiStatus.h"

#include <stdint.h>
#include <stdio.h>

#include <string.h>



char buffer[LCD_LINE_COUNT][LCD_LINE_LENGTH + 1];

/*
** Interface
*/

void InitLcd()
{
    int line, column;
    for (line = 0; line < LCD_LINE_COUNT; ++line)
    {
        for (column = 0; column < LCD_LINE_LENGTH; ++column)
        {
            buffer[line][column] = ' ';
        }
        buffer[line][LCD_LINE_LENGTH] = '\0';
    }
}


void PutLcd(int column, int row, char* text)
{
    char* line = buffer[row];
    int count = min(LCD_LINE_LENGTH - column, strlen(text));
    strncpy(line + column, text, count);
}


void PutLcdLine(int row, char* text)
{
    char* line = buffer[row];
    int length = strlen(text);
    int i;
    for (i = 0; i < LCD_LINE_LENGTH; ++i)
    {
        line[i] = (i < length) ? text[i] : ' ';
    }
    line[LCD_LINE_LENGTH] = '\0';
}


void UpdateLcd()
{
    /*
    int line;
    for (line = 0; line < LCD_LINE_COUNT; ++line)
    {
        printf("LCD: %s\r\n", buffer[line]);
    }
    */
}
