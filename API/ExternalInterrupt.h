#ifndef _EXTERNALINTERRUPT_H_
#define _EXTERNALINTERRUPT_H_


typedef void (*InterruptService) ();

void InitExternalInterrupt(InterruptService interruptService);


#endif /* _EXTERNALINTERRUPT_H_ */
