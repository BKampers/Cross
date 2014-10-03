#ifndef _GPIO_LCD1602_H_
#define _GPIO_LCD1602_H_

#define LCD_LINE_COUNT   2
#define LCD_LINE_LENGTH 16


void InitLcd();
void PutLcd(int column, int row, char* text);
void PutLcdLine(int row, char* text);
void UpdateLcd();


#endif // _GPIO_LCD1602_H_
