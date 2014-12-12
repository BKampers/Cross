#include "Timers.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h> 

#include "Types.h"


/*
** Private
*/


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
            /*printf("* HandlePulse(%d)\r\n", timerValue * 0x200);*/
            HandlePulse(timerValue * 0x200);
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


void InitExternalPulseTimer(void (*InterruptService) (int capture))
{
    HandlePulse = InterruptService;
    StartTimerThread();
}


Status StartPeriodTimer(int ticks)
{
    //TODO
    return OK;
}
