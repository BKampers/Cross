/*
** Implementation of the UART driver recognizing JSON objects
** Copyright 2014, Bart Kampers
*/


#include "Communication.h"

#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

#include "Types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
** private 
*/

#define CHANNEL_COUNT 2

typedef struct
{
    char* inputBuffer;
    uint8_t bufferSize;
    uint8_t inputIndex;
    uint8_t bracketCount;
    bool inputAvailable;
}  Channel;


char statusMessage[32];

Channel* channels[] = { NULL, NULL };


Status ValidateChannelId(int channelId)
{
    if ((0 <= channelId) && (channelId < CHANNEL_COUNT))
    {
        return OK;
    }
    else
    {
        sprintf(statusMessage, "InvalidChannelId %d", channelId);
        return statusMessage;
    }
}


Status ValidateChannelAvailable(int channelId)
{
    Status status = ValidateChannelId(channelId);
    if (status == OK)
    {
        if (channels[channelId] != NULL)
        {
            sprintf(statusMessage, "AlreadyOpen %d", channelId);
            status = statusMessage;
        }
    }
    return status;
}


Status ValidateChannelOpen(int channelId)
{
    Status status = ValidateChannelId(channelId);
    if (status == OK)
    {
        if (channels[channelId] == NULL)
        {
            sprintf(statusMessage, "ChannelNotOpened %d", channelId);
            status = statusMessage;
        }
    }
    return status;
}


/*
** Configure USART's nested vectored interrupt controller.
*/
void NVIC_Configuration(int channelId)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = (channelId == 0) ?  USART1_IRQn : USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


/*
** Configure the USART's GPIO ports.
*/
void GPIO_Configuration(int channelId)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure USARTx Tx pin as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = (channelId == 0) ? GPIO_Pin_9 : GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USARTx Rx pin as input floating */
  GPIO_InitStructure.GPIO_Pin = (channelId == 0) ? GPIO_Pin_10 : GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}


/*
** Enable USART
*/
void InitChannel(int channelId)
{
    if (channelId == 0)
    {
        /* Enable USART1 and GPIOA clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    }
    else
    {
        /* Enable USART2 clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    }

    NVIC_Configuration(channelId);
    GPIO_Configuration(channelId);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_TypeDef* usart = (channelId == 0) ? USART1 : USART2;
    USART_Init(usart, &USART_InitStructure);
    USART_Cmd(usart, ENABLE);
    /* Enable the usart Receive interrupt: this interrupt is generated when the USARTx receive data register is not empty */
    USART_ITConfig(usart, USART_IT_RXNE, ENABLE);
    WriteChannel(channelId, "\r\n"); /* First character might get lost */
}


void DeinitChannel(int channelId)
{
    USART_TypeDef* usart = (channelId == 0) ? USART1 : USART2;
    USART_Cmd(usart, DISABLE);
    USART_ITConfig(usart, USART_IT_RXNE, DISABLE);
    USART_DeInit(usart);
}


/*
** Interface
*/

Status OpenCommunicationChannel(int channelId, uint8_t bufferSize)
{
    Status status = ValidateChannelAvailable(channelId);
    if (status == OK)
    {
        channels[channelId] = malloc(sizeof(Channel));
        channels[channelId]->inputBuffer = malloc(bufferSize);
        channels[channelId]->bufferSize = bufferSize;
        channels[channelId]->inputIndex = 0;
        channels[channelId]->bracketCount = 0;
        channels[channelId]->inputAvailable = FALSE;
        InitChannel(channelId);
    }
    return status;
}


Status CloseCommunicationChannel(int channelId)
{
    Status status = ValidateChannelOpen(channelId);
    if (status == OK)
    {
        DeinitChannel(channelId);
        free(channels[channelId]->inputBuffer);
        free(channels[channelId]);
        channels[channelId] = NULL;
    }
    return status;
}


Status ReadChannel(int channelId, char* string)
{
    Status status = ValidateChannelOpen(channelId);
    if (status == OK)
    {
        Channel* channel = channels[channelId];
        if (channel->inputAvailable)
        {
            strncpy(string, channel->inputBuffer, channel->inputIndex);
            string[channel->inputIndex] = '\0';
            channel->inputAvailable = FALSE;
            channel->inputIndex = 0;
        }
        else
        {
            string[0] = '\0';
        }
    }
    return status;
}


Status ReadString(char* string)
{
    return ReadChannel(DEFAULT_CHANNEL, string);
}


Status WriteChannel(int channelId, char* string)
{
    Status status = ValidateChannelOpen(channelId);
    if (status == OK)
    {
        USART_TypeDef* usart = (channelId == 0) ? USART1 : USART2;
        while (*string != '\0')
        {
            USART_SendData(usart, (uint16_t) *string);
            ++string;
            while (USART_GetFlagStatus(usart, USART_FLAG_TC) == RESET); /* Loop until the end of transmission */
        }
    }
    return status;
}


Status WriteString(char* string)
{
    return WriteChannel(DEFAULT_CHANNEL, string);
}


/*
** Interrupt handling
*/

void HandleIrq(USART_TypeDef* usart, Channel* channel)
{
    if ((channel != NULL) && ((usart->SR & USART_FLAG_RXNE) != (uint16_t) RESET) && (channel->inputIndex < channel->bufferSize))
    {
        char ch = (char) USART_ReceiveData(usart);
        if (ch == '{')
        {
            channel->bracketCount++;
        }
        if (channel->bracketCount > 0)
        {
            channel->inputBuffer[channel->inputIndex] = ch;
            channel->inputIndex++;
            if (ch == '}')
            {
                channel->bracketCount--;
                if (channel->bracketCount == 0)
                {
                    channel->inputAvailable = TRUE;
                }
            }
        }
    }
}


void USART1_IRQHandler(void)
{
    HandleIrq(USART1, channels[0]);
}


void USART2_IRQHandler(void)
{
    HandleIrq(USART2, channels[1]);
}
