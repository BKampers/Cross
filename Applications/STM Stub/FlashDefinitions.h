#ifndef _FLASHDEFINITIONS_H_
#define _FLASHDEFINITIONS_H_

#include "stm32f10x_flash.h"
#include "core_cm3.h"

#define LOCK_FLASH_BANK() (void) 0
#define UNLOCK_FLASH_BANK() (void) 0
#define CLEAR_FLASH_FLAG(FLAG) (void) 0
#define ERASE_FLASH_PAGE(REFERENCE) FLASH_COMPLETE
#define READ_FLASH_WORD(REFERENCE) ReadFlashWord(REFERENCE)
#define WRITE_FLASH_WORD(REFERENCE, DATA) WriteFlashWord(REFERENCE, DATA)

#endif /* _FLASHDEFINITIONS_H_ */

