#include "Control.h"

#include <pthread.h>
#include <unistd.h>

#include "SocketHandler.h"
#include "JsonParser.h"

#include "Timers.h"


/*
** Private
*/

#define INPUT_BUFFER_SIZE 64

char buffer[INPUT_BUFFER_SIZE];
Channel channel;

long pulseNanos = 1000000;
int cog = 1;


void* ControlTask(void* threadArgs)
{
    for (;;)
    {
        if (channel.inputAvailable)
        {
            JsonNode node;
            JsonStatus status;
            int nanos;
            
            ReadSocketChannel(&channel, buffer);
            Initialize(buffer, &node);
            status = GetInt(&node, "PulseNanos", &nanos);
            if (status == JSON_OK)
            {
                pulseNanos = nanos;
            }
        }
        else
        {
            sleep(1);
        }
    }
}


/*
** Interface
*/

Status InitControl()
{
    Status status;
    channel.bufferSize = INPUT_BUFFER_SIZE;
    channel.port = DEFAULT_PORT - 1;
    status = InitSocketChannel(&channel);
    if (status == OK)
    {
        pthread_t threadID;
        int error = pthread_create(&threadID, NULL, ControlTask, NULL);
        if (error != 0)
        {
            status = "ControlThreadNotStarted";
        }
    }
    return status;
}


void GetPulseWidth(struct timespec* pulseWidth)
{
    pulseWidth->tv_sec = 0;
    if (cog <= 58)
    {
        pulseWidth->tv_nsec = pulseNanos;
        cog++;
    }
    else
    {
        pulseWidth->tv_nsec = pulseNanos * 3;
        cog = 1;
    }
}
