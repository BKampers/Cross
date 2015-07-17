#include <pthread.h>


#include "ApiStatus.h"

#include "SocketHandler.h"
#include "JsonParser.h"

#include "Timers.h"


void SetExternalTicks(int ticks);

/*
** Private
*/

#define INPUT_BUFFER_SIZE 64

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
            
            Initialize(channel.inputBuffer, &node);
            status = GetInt(&node, "ExternalTicks", &ticks);
            if (status == JSON_OK)
            {
                SetExternalTicks(ticks);
            }
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
