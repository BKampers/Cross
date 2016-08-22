#ifndef _STM32F10XFLASH_H_
#define _STM32F10XFLASH_H_

#include <stdint.h>


#define FLASH_COMPLETE 1

#define FLASH_FLAG_EOP 0x1
#define FLASH_FLAG_PGERR 0x2
#define FLASH_FLAG_WRPRTERR 0x4

typedef int FLASH_Status;

extern void* flashMemory;

void initFlashMemory();

void FLASH_LockBank1();
void FLASH_UnlockBank1();
void FLASH_ClearFlag(int);
FLASH_Status FLASH_ErasePage(uint32_t);
FLASH_Status FLASH_ProgramWord(uint32_t, uint32_t);

#endif /* _STM32F10XFLASH_H_ */

