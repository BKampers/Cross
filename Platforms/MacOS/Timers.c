#include "Timers.h"

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h> 
#include <unistd.h>

#include "Types.h"
#include "Control.h"


/*
** Private
*/

void (*HandlePulse) (int capture) = NULL;
void (*HandleStop) () = NULL;

int timerValue = 0;
bool timerTaskRunning = FALSE;


void* TimerTask(void* threadArgs)
{
    while (timerTaskRunning)
    {
        struct timespec pulseWidth;
        GetPulseWidth(&pulseWidth);
        if (pulseWidth.tv_nsec <= MAX_PULSE_NANOS)
        {
            nanosleep(&pulseWidth, NULL);
            timerValue = (timerValue + pulseWidth.tv_nsec / 278) % 0x10000;
            HandlePulse(timerValue);
        }
        else
        {
            HandleStop();
            nanosleep(&pulseWidth, NULL);
        }
    }
    return NULL;
}


void StartTimerThread()
{
    if (! timerTaskRunning)
    {
        timerTaskRunning = TRUE;
        pthread_t threadID;
        int error = pthread_create(&threadID, NULL, TimerTask, NULL);
        if (error == 0)
        {
            printf("=== Timer thread created ===\n");
        }
        else
        {
            printf("=== Timer thread creation failed (error = %d) ===\n", error);
        }
    }
}


/*
** Interface
*/

void InitPeriodTimer(void (*InterruptService) ())
{
    //TODO
}


void InitCompareTimer(void (*InterruptService) (int channel))
{
    //TODO
}


void InitExternalPulseTimer(void (*InterruptService) (int capture), void (*OverflowService) ())
{
    InitControl();
    HandlePulse = InterruptService;
    HandleStop = OverflowService;
    StartTimerThread();
}


Status StartPeriodTimer(int ticks)
{
    //TODO
    return OK;
}


Status StartCompareTimer(int channel, int ticks)
{
    return OK;
}


int GetCompareTimerPeriod()
{
    return 0xFFFF;
}
