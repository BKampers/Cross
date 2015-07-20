#include <pthread.h>
#include <unistd.h>

#include "ApiStatus.h"

#include "SocketHandler.h"
#include "JsonParser.h"

#include "Timers.h"


int GetExternalTicks();
void SetExternalTicks(int ticks);

/*
** Private
*/

#define INPUT_BUFFER_SIZE 64

char buffer[INPUT_BUFFER_SIZE];
Channel channel;


void* ControlTask(void* threadArgs)
{
    for (;;)
    {
        if (channel.inputAvailable)
        {
            JsonNode node;
            JsonStatus status;
            int ticks;
            
            ReadSocketChannel(&channel, buffer);
            Initialize(buffer, &node);
            status = GetInt(&node, "ExternalTicks", &ticks);
            if (status == JSON_OK)
            {
                SetExternalTicks(ticks);
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
