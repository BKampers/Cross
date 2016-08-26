#ifndef _STM32F10XFLASH_H_
#define _STM32F10XFLASH_H_

#include <stdint.h>
#include "Types.h"


#define FLASH_COMPLETE 1
#define FLASH_ERROR -1

#define FLASH_FLAG_EOP 0x1
#define FLASH_FLAG_PGERR 0x2
#define FLASH_FLAG_WRPRTERR 0x4


typedef int FLASH_Status;

extern byte flashMemory[];

uint32_t ReadFlashWord(uint32_t reference);
FLASH_Status WriteFlashWord(uint32_t, uint32_t);

#endif /* _STM32F10XFLASH_H_ */

