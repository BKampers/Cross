#ifndef _STM32F10XFLASH_H_
#define _STM32F10XFLASH_H_

#include <stdint.h>
#include "Types.h"


#define FLASH_FLAG_BSY      ((uint32_t) 0x00000001) /*!< FLASH Busy flag */
#define FLASH_FLAG_EOP      ((uint32_t) 0x00000020) /*!< FLASH End of Operation flag */
#define FLASH_FLAG_PGERR    ((uint32_t) 0x00000004) /*!< FLASH Program error flag */
#define FLASH_FLAG_WRPRTERR ((uint32_t) 0x00000010) /*!< FLASH Write protected error flag */
#define FLASH_FLAG_OPTERR   ((uint32_t) 0x00000001) /*!< FLASH Option Byte error flag */

typedef enum
{ 
  FLASH_BUSY = 1,
  FLASH_ERROR_PG,
  FLASH_ERROR_WRP,
  FLASH_COMPLETE,
  FLASH_TIMEOUT
}FLASH_Status;


extern byte flashMemory[];

void ClearFlashMemory();

uint32_t ReadFlashWord(uint32_t reference);
FLASH_Status WriteFlashWord(uint32_t, uint32_t);

#endif /* _STM32F10XFLASH_H_ */

