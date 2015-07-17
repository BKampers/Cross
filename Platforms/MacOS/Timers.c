#include "Timers.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h> 

#include "Types.h"


void InitControl();


/*
** Private
*/

int externalTicks = 0x20000;


void (*HandlePulse) (int capture) = NULL;

int timerValue = 0;
bool timerTaskRunning = FALSE;


void* TimerTask(void* threadArgs)
{
    while (timerTaskRunning)
    {
        sleep(1);
        timerValue = (timerValue + 1) % 0x80;
        int cog = timerValue % 60;
        if ((0 < cog) && (cog < 59)) 
        {
            HandlePulse(timerValue * externalTicks);
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


void SetExternalTicks(int ticks)
{
    externalTicks = ticks;
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


void InitExternalPulseTimer(void (*InterruptService) (int capture))
{
    InitControl();
    HandlePulse = InterruptService;
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
