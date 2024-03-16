#include "AnalogInput.h"

#include <stm32f10x_rcc.h>
#include <stm32f10x_dma.h>
#include <stm32f10x_adc.h>
#include <stm32f10x_gpio.h>
#include <core_cm3.h>


#define ADC1_DR_Address ((uint32_t) 0x4001244C)

#define NUMBER_OF_ADC_CHANNELS 4


char* INVALID_ADC_INDEX = "Invalid ADC index";

__IO uint16_t ADCConvertedValues[NUMBER_OF_ADC_CHANNELS];


void InitAnalogInput()
{
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    /* ADCCLK = PCLK2/4 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div4);
    /* Enable peripheral clocks ------------------------------------------------*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

    /* DMA1 channel1 configuration ----------------------------------------------*/
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) ADCConvertedValues;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = NUMBER_OF_ADC_CHANNELS;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);

    /* ADC1 configuration ------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = NUMBER_OF_ADC_CHANNELS;
    ADC_Init(ADC1, &ADC_InitStructure);

    for (uint8_t channel = ADC_Channel_0; channel < NUMBER_OF_ADC_CHANNELS; ++channel)
    {
    	ADC_RegularChannelConfig(ADC1, channel, channel + 1, ADC_SampleTime_55Cycles5);
    }

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    /* Check the end of ADC1 reset calibration register */
    while(ADC_GetResetCalibrationStatus(ADC1));

    ADC_StartCalibration(ADC1);
    /* Check the end of ADC1 calibration */
    while(ADC_GetCalibrationStatus(ADC1));

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


Status GetAnalogValue(int index, int* value)
{
    if ((0 <= index) && (index < NUMBER_OF_ADC_CHANNELS))
    {
        *value = (int) ADCConvertedValues[index];
        return OK;
    }
    else
    {
        return INVALID_ADC_INDEX;
    }
}
