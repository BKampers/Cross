/*
** Implementation of the UART driver recognizing JSON objects
** Copyright 2014, Bart Kampers
*/


#include "Communication.h"

#include "Types.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <pthread.h> 
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <errno.h>


/*
** private 
*/

#define USER_PORT 10007
#define MAXPENDING 5 /* Maximum outstanding connection requests */
#define RCVBUFSIZE 256


typedef struct
{
    char* inputBuffer;
    uint8_t bufferSize;
    uint8_t inputIndex;
    uint8_t bracketCount;
    bool inputAvailable;
}  Channel;


char receiveBuffer[RCVBUFSIZE];
char statusMessage[32];

Channel* channels[] = { NULL, NULL };
#define CHANNEL_COUNT (sizeof(channels) / sizeof(Channel*))

Status threadStatus = UNINITIALIZED;
int clientSocketId = -1; /* any negative value means uninitialized */
int bytesReceived = 0;
bool running = FALSE;


void HandleReceivedCharacter(char ch, Channel* channel)
{
    if (channel != NULL)
    {
        if (ch == '{')
        {
            channel->bracketCount++;
        }
        if (channel->bracketCount > 0)
        {
            channel->inputBuffer[channel->inputIndex] = ch;
            channel->inputIndex++;
            if (ch == '}')
            {
                channel->bracketCount--;
                if (channel->bracketCount == 0)
                {
                    channel->inputAvailable = TRUE;
                }
            }
        }
    }
}


void* ThreadMain(void* threadArgs)
{    
    int serverSocketId; /* Socket descriptor for server */
    struct sockaddr_in serverAddress; /* Local address */
    struct sockaddr_in clientAddress; /* Client address */
    socklen_t clientAddressLength = sizeof(clientAddress); /* Length of client address data structure */
    uint64_t count = 0;
    printf("=== ThreadMain %d ===\n", ((Channel*) threadArgs)->bufferSize);

    /* Create socket for incoming connections */
    serverSocketId = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocketId >= 0)
    {      
        /* Construct local address structure */
        memset(&serverAddress, 0, sizeof(serverAddress)); /* Zero out structure */
        serverAddress.sin_family = AF_INET; /* Internet address family */
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
        serverAddress.sin_port = htons(USER_PORT); /* Local port */

        /* Bind to the local address */
        int result = bind(serverSocketId, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
        if (result == 0)
        {
            /* Mark the socket so it will listen for incoming connections */
            result = listen(serverSocketId, MAXPENDING);
            if (result == 0)
            {
                threadStatus = OK;
                for (;;) /* Run forever */
                {
                    printf("=== Forever...\n");
                    /* Wait for a client to connect */
                    clientSocketId = accept(serverSocketId, (struct sockaddr *) &clientAddress, &clientAddressLength);
                    printf("=== clientSocketId = %d\n", clientSocketId);
                    if (clientSocketId >= 0)
                    {
                        /* client connected */
                        while (running)
                        {
                            char ch;
                            printf("=== running...\n");                    
                            int received = recv(clientSocketId, &ch, 1, 0);
                            HandleReceivedCharacter(ch, (Channel*) threadArgs);
                            count++;
                        }
                    }
                }
            }
            else
            {
                threadStatus = "LISTEN_FAILED";
            }
        }
        else
        {
            int e = errno;
            threadStatus = "BIND_ADDRESS_FAILED";
        }
    }
    else 
    {
        threadStatus = "CREATE_SOCKET_FAILED";
    }
    
    printf("=== %s ===\n", threadStatus);
    return NULL;
}



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
    Channel* channel = channels[channelId];
    if (! running)
    {
        running = TRUE;
        pthread_t threadID;
        int error = pthread_create(&threadID, NULL, ThreadMain, (void*) channel);
        if (error == 0)
        {
            printf("=== Server thread created ===\n");
        }
        else
        {
            printf("=== Thread error %d ===\n", error);
        }
    }
}


void DeinitChannel(int channelId)
{
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
        channels[channelId]->inputBuffer = malloc(bufferSize);
        channels[channelId]->bufferSize = bufferSize;
        channels[channelId]->inputIndex = 0;
        channels[channelId]->bracketCount = 0;
        channels[channelId]->inputAvailable = FALSE;
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
        free(channels[channelId]->inputBuffer);
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
        Channel* channel = channels[channelId];
        if (channel->inputAvailable)
        {
            strncpy(string, channel->inputBuffer, channel->inputIndex);
            string[channel->inputIndex] = '\0';
            channel->inputAvailable = FALSE;
            channel->inputIndex = 0;
        }
        else
        {
            string[0] = '\0';
        }
    }
    return status;
}


Status ReadString(char* string)
{
    return ReadChannel(DEFAULT_CHANNEL, string);
}


Status WriteChannel(int channelId, char* string)
{
    Status status = "SocketNotInitialized";
    if (clientSocketId >= 0)
    {
        int length = strlen(string);
        /* Send the string to the client */
        int sent = send(clientSocketId, string, length, 0);
        if (sent == length)
        {
            status = OK;
        }
        else
        {
            status = "SendFailure";
        }
    }
    return status;
}


Status WriteString(char* string)
{
    return WriteChannel(DEFAULT_CHANNEL, string);
}
