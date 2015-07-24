#ifndef _CONTROL_H_
#define	_CONTROL_H_

#include <time.h>
#include "ApiStatus.h"


Status InitControl();

void GetPulseWidth(struct timespec* pulseWidth);


#endif	/* _CONTROL_H_ */

