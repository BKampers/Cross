#ifndef _HARDWARESETTINGS_H_
#define _HARDWARESETTINGS_H_

#include "stm32f10x_gpio.h"

#define OUTPUT_REGISTER_MM RCC_APB2Periph_GPIOE
#define OUTPUT_PORT_MM GPIOE

#define IGNITION_1_PIN GPIO_Pin_0
#define IGNITION_2_PIN GPIO_Pin_1
#define IGNITION_3_PIN GPIO_Pin_2
#define IGNITION_4_PIN GPIO_Pin_3
#define IGNITION_5_PIN GPIO_Pin_4
#define IGNITION_6_PIN GPIO_Pin_5
#define IGNITION_7_PIN GPIO_Pin_6
#define IGNITION_8_PIN GPIO_Pin_7

#define INJECTION_1_PIN GPIO_Pin_8
#define INJECTION_2_PIN GPIO_Pin_9
#define INJECTION_3_PIN GPIO_Pin_10
#define INJECTION_4_PIN GPIO_Pin_11

#define GLOBAL_IGNITION_PIN GPIO_Pin_12
#define TEMP_PHASE_PIN GPIO_Pin_13
#define IGNITION_PWM_PIN GPIO_Pin_14

#define OUTPUT_REGISTER_RPM RCC_APB2Periph_GPIOB
#define OUTPUT_PORT_RPM GPIOB

#define RPM_INDICATOR_1 GPIO_Pin_2
#define RPM_INDICATOR_2 GPIO_Pin_4
#define RPM_INDICATOR_3 GPIO_Pin_5
#define RPM_INDICATOR_4 GPIO_Pin_10




#endif /* _HARDWARESETTINGS_H_ */
