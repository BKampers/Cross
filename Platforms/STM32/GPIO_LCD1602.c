#include "Lcd.h"

#include "ApiStatus.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include <stdint.h>
#include <stdio.h>

#include "string.h"


#define SOFTWARE_DELAY 5000
#undef READ_LCD_ENABLED

/***************************************************************************//**
 * Declare function prototypes
 ******************************************************************************/
void RCC_Configuration(void);
void LCD_DATA_DIR_OUT(void);
void LCD_DATA_DIR_IN(void);
void LCD_ALL_DIR_OUT(void);
unsigned int LCD_DATA_IN(void);
static unsigned char Wait_While_Busy(void);
void Lcd_Write_4bits(uc8 byte);
void Delay(vu32 nCount);
void Lcd_Write_Command(uc8 command);
void Lcd_Write_Data(uc8 data);
void Lcd_Init(void);
void Lcd_Write_Line1(void);
void Lcd_Write_Line2(void);
void set_cursor(int, int);
void lcd_print (char *string);
void lcd_clear (void);

#ifdef READ_LCD_ENABLED
static unsigned char Lcd_Read_Status (void);
#endif


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
    RCC_Configuration();
    Lcd_Init();
    lcd_clear();
    Delay(1000);
}


void PutLcd(int column, int row, char* text)
{
    char* line = buffer[row];
    int count = min(LCD_LINE_LENGTH - column, strlen(text));
    strncpy(line + column, text, count);
}


void UpdateLcd()
{
    int line;
    for (line = 0; line < LCD_LINE_COUNT; ++line)
    {
        set_cursor(0, line);
        lcd_print(buffer[line]);
    }
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


/***************************************************************************//**
 * Global variables, private define and typedef
 ******************************************************************************/
#define EN  GPIO_Pin_10
#define RS  GPIO_Pin_12
#define RW  GPIO_Pin_11

const unsigned int SWAP_DATA[16] = { 0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE,
                                     0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF};


const char UserFont[8][8] = {  /* 8 user defined characters to be loaded into CGRAM (used for bargraph)*/
    { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
    { 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10 },
    { 0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18 },
    { 0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C },
    { 0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E },
    { 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F },
    { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
    { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }
};


/***************************************************************************//**
 * @brief System clocks configuration
 ******************************************************************************/
void RCC_Configuration(void)
{
    RCC_DeInit ();                        /* RCC system reset(for debug purpose)*/
    RCC_HSEConfig (RCC_HSE_ON);           /* Enable HSE                         */

    /* Wait till HSE is ready                                                   */
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

    RCC_HCLKConfig   (RCC_SYSCLK_Div1);   /* HCLK   = SYSCLK                    */
    RCC_PCLK2Config  (RCC_HCLK_Div1);     /* PCLK2  = HCLK                      */
    RCC_PCLK1Config  (RCC_HCLK_Div2);     /* PCLK1  = HCLK/2                    */
    RCC_ADCCLKConfig (RCC_PCLK2_Div4);    /* ADCCLK = PCLK2/4                   */

    *(vu32 *)0x40022000 = 0x01;           /* Flash 2 wait state                 */

    /* PLLCLK = 8MHz * 9 = 72 MHz                                               */
    RCC_PLLConfig (0x00010000, RCC_PLLMul_9);

    RCC_PLLCmd (ENABLE);                  /* Enable PLL                         */

    /* Wait till PLL is ready                                                   */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

    /* Select PLL as system clock source                                        */
    RCC_SYSCLKConfig (RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source                             */
    while (RCC_GetSYSCLKSource() != 0x08);
}

/***************************************************************************//**
 * @brief Delay some time
 ******************************************************************************/
void Delay(vu32 nCount)
{
    for(; nCount != 0; nCount--);
}

/***************************************************************************//**
 * @brief  Setting all pins to output mode
 ******************************************************************************/
void LCD_ALL_DIR_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/***************************************************************************//**
 * @brief  Setting DATA pins to input mode
 ******************************************************************************/
void LCD_DATA_DIR_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);  
}

/***************************************************************************//**
 * @brief  Setting DATA pins to output mode
 ******************************************************************************/
void LCD_DATA_DIR_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/***************************************************************************//**
 * @brief  Reading DATA pins
 * @return the data value.
 ******************************************************************************/
unsigned int LCD_DATA_IN(void)
{
    uint16_t u16Temp=0;
    u16Temp = GPIO_ReadInputData(GPIOC)&0x000F;
    return SWAP_DATA[u16Temp];
}

#ifdef READ_LCD_ENABLED
/***************************************************************************//**
 * @brief  Read status of LCD controller
 * @return status : Status of LCD controller
 ******************************************************************************/
static unsigned char Lcd_Read_Status (void)
{
    unsigned char status;

    LCD_DATA_DIR_IN();
    GPIO_WriteBit(GPIOC, RS, Bit_RESET);
    GPIO_WriteBit(GPIOC, RW, Bit_SET);
    Delay(10);
    GPIO_WriteBit(GPIOC, EN, Bit_SET);
    Delay(10);
    status  = LCD_DATA_IN() << 4;
    GPIO_WriteBit(GPIOC, EN, Bit_RESET);
    Delay(10);
    GPIO_WriteBit(GPIOC, EN, Bit_SET);
    Delay(10);
    status |= LCD_DATA_IN();
    GPIO_WriteBit(GPIOC, EN, Bit_RESET);
    LCD_DATA_DIR_OUT();
    return (status);
}
#endif


/***************************************************************************//**
 * @brief Wait while LCD is busy
 * @return status : Status of LCD controller
 ******************************************************************************/
static unsigned char Wait_While_Busy()
{  
#ifdef READ_LCD_ENABLED
    unsigned char status;
    do
    {
    	status = Lcd_Read_Status();
    } while ((status & 0x80) != 0);
    return status;
#else
    Delay(SOFTWARE_DELAY);
    return 0;
#endif
}
/***************************************************************************//**
 * @brief  Write 4-bits to LCD controller
 ******************************************************************************/
void Lcd_Write_4bits(uc8 byte)
{
    uint16_t u16Temp=0;
    GPIO_WriteBit(GPIOC, RW, Bit_RESET);
    GPIO_WriteBit(GPIOC, EN, Bit_SET);
    u16Temp = GPIO_ReadOutputData(GPIOC)&0xFFF0;
    u16Temp |=  SWAP_DATA[byte&0x0F];
    GPIO_Write(GPIOC, u16Temp);
    Delay(10);
    GPIO_WriteBit(GPIOC, EN, Bit_RESET);
    Delay(10);
}

/***************************************************************************//**
 * @brief:    Write command to LCD controller
 * @param[in] command :  Command to be written
 ******************************************************************************/
void Lcd_Write_Command(uc8 command)
{
    Wait_While_Busy();
    GPIO_WriteBit(GPIOC, RS, Bit_RESET);
    Lcd_Write_4bits(command>>4);
    Lcd_Write_4bits(command);
}

/***************************************************************************//**
 * @brief:     Write data to LCD controller
  * @param[in] data :  Data to be written
 ******************************************************************************/
void Lcd_Write_Data(uc8 data)
{
    Wait_While_Busy();
    GPIO_WriteBit(GPIOC, RS, Bit_SET);
    Lcd_Write_4bits(data>>4);
    Lcd_Write_4bits(data);
}

/*******************************************************************************
* @brief : Set cursor position on LCD display
* @param[in] column : Column position
* @param[in] line   : Line position
*******************************************************************************/
void set_cursor(int column, int line)
{
    unsigned char address;

    address = (line * 0x40) + column;
    address = 0x80 + (address & 0x7F);
    Lcd_Write_Command(address);               /* Set DDRAM address counter to 0     */
}

/***************************************************************************//**
 * @brief  Initial the LCD1602
 ******************************************************************************/
void Lcd_Init(void)
{
    char const *p;
    int i;

    /* Enable clock for peripheral        */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    /* Set all pins for LCD as outputs    */
    LCD_ALL_DIR_OUT();
    Delay(15000);
    GPIO_WriteBit(GPIOC, RS, Bit_RESET);
    Lcd_Write_4bits(0x3);  /* Select 4-bit interface  */
    Delay(4100);
    Lcd_Write_4bits(0x3);
    Delay(100);
    Lcd_Write_4bits(0x3);
    Lcd_Write_4bits(0x2);
    
    Lcd_Write_Command(0x28); /* 2 lines, 5x8 character matrix      */
    Lcd_Write_Command(0x0C); /* Display ctrl:Disp=ON,Curs/Blnk=OFF */
    Lcd_Write_Command(0x06); /* Entry mode: Move right, no shift   */

    /* Load user-specific characters into CGRAM                                 */
    Lcd_Write_Command(0x40);                  /* Set CGRAM address counter to 0     */
    p = &UserFont[0][0];
    for (i = 0; i < sizeof(UserFont); i++, p++)
    {
        lcd_print((char*) (int) *p);
    }
    Lcd_Write_Command(0x80);                 /* Set DDRAM address counter to 0     */
}

/***************************************************************************//**
 * @brief   print a string on LCD1602.
 * @param[in] *string : point to the string which will be printed on LCD.
 ******************************************************************************/
void lcd_print (char *string)
{
    while (*string) 
    {
        Lcd_Write_Data (*string++);
    }
}

/*******************************************************************************
 * @brief  Clear the LCD display                                                        *
*******************************************************************************/
void lcd_clear (void)
{
    Lcd_Write_Command(0x01);                  /* Display clear                      */
    set_cursor (0, 0);
}
