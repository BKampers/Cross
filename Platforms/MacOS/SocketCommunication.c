/*
** Implementation of the Communication interface driver recognizing JSON objects
*/


#include "Communication.h"

#include "Types.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SocketHandler.h"


/*
** private 
*/

char statusMessage[32];

Channel* channels[] = { NULL, NULL };
#define CHANNEL_COUNT (sizeof(channels) / sizeof(Channel*))

int bytesReceived = 0;
bool socketTaskRunning = FALSE;


Status ValidateChannelId(int channelId)
{
    if ((0 <= channelId) && (channelId < CHANNEL_COUNT))
    {
        return OK;
    }
    else
    {
        sprintf(statusMessage, "InvalidChannelId %d", channelId);
        return statusMessage;
    }
}


Status ValidateChannelAvailable(int channelId)
{
    Status status = ValidateChannelId(channelId);
    if (status == OK)
    {
        if (channels[channelId] != NULL)
        {
            sprintf(statusMessage, "AlreadyOpen %d", channelId);
            status = statusMessage;
        }
    }
    return status;
}


Status ValidateChannelOpen(int channelId)
{
    Status status = ValidateChannelId(channelId);
    if (status == OK)
    {
        if (channels[channelId] == NULL)
        {
            sprintf(statusMessage, "ChannelNotOpened %d", channelId);
            status = statusMessage;
        }
    }
    return status;
}


void InitChannel(int channelId)
{
    if (channelId == DEFAULT_CHANNEL)
    {
        Channel* channel = channels[channelId];
        if (! socketTaskRunning)
        {
            socketTaskRunning = TRUE;
            InitSocketChannel(channel);
        }
    }
}


void DeinitChannel(int channelId)
{
    if (channels[channelId] != NULL)
    {
        DeinitSocketChannel(channels[channelId]);
    }
}


/*
** Interface
*/

Status OpenCommunicationChannel(int channelId, uint8_t bufferSize)
{
    Status status = ValidateChannelAvailable(channelId);
    if (status == OK)
    {
        channels[channelId] = malloc(sizeof(Channel));
        channels[channelId]->bufferSize = bufferSize;
        channels[channelId]->port = DEFAULT_PORT + channelId;
        InitChannel(channelId);
    }
    return status;
}


Status CloseCommunicationChannel(int channelId)
{
    Status status = ValidateChannelOpen(channelId);
    if (status == OK)
    {
        DeinitChannel(channelId);
        free(channels[channelId]);
        channels[channelId] = NULL;
    }
    return status;
}


Status ReadChannel(int channelId, char* string)
{
    Status status = ValidateChannelOpen(channelId);
    if (status == OK)
    {
        ReadSocketChannel(channels[channelId], string);
    }
    return status;
}


Status ReadString(char* string)
{
    return ReadChannel(DEFAULT_CHANNEL, string);
}


Status WriteCharacter(int channelId, char character)
{
    char string[2] = {character, '\0'};
    return WriteString(channelId, string);
}


Status WriteString(int channelId, const char* string)
{
    Status status = ValidateChannelOpen(channelId);
    if (status == OK)
    {
        if (channelId == DEFAULT_CHANNEL)
        {
            Channel* channel = channels[channelId];
            if (channel->clientSocketId >= 0)
            {
                size_t length = strlen(string);
                size_t sent = send(channel->clientSocketId, string, length, 0);
                if (sent != length)
                {
                    status = "SendFailure";
                }
            }
            else
            {
                status = "SocketNotInitialized";
            }
        }
        else
        {
            printf("Channel %d: %s", channelId, string);
        }
    }
    return status;
}
