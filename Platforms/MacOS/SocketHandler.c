#include "SocketHandler.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h> 


#include "Communication.h"


/*
** Private
*/

#define MAXPENDING 5 /* Maximum outstanding connection requests */

char* CONNECTED = "Connected";


void LogChannelStatus(Channel* channel)
{
    printf("%d %s: %d \r\n", (int) (channel->port), (char*) (channel->status), errno);
}


void HandleReceivedCharacter(char ch, Channel* channel)
{
    if ((channel != NULL) && ! channel->inputAvailable)
    {
        if (ch == TRANSMISSION_END)
        {
            channel->inputAvailable = TRUE;
        }
        else
        {
            channel->inputBuffer[channel->inputIndex] = ch;
            channel->inputIndex++;
        }
    }
}


void ListenToClient(Channel* channel)
{
    while (channel->status == CONNECTED)
    {
        char ch;
        int received = recv(channel->clientSocketId, &ch, sizeof(ch), 0);
        if (received > 0)
        {
            HandleReceivedCharacter(ch, channel);
        }
        else
        {
            if (received == 0)
            {
                channel->status = "Disconnected";
                LogChannelStatus(channel);
            }
            else
            {
                channel->status = "ReceiveFailed";
                LogChannelStatus(channel);
            }
        }
    }
}


void RunChannelThread(int serverSocketId, Channel* channel)
{
    for (;;) /* Run forever */
    {
        if (listen(serverSocketId, MAXPENDING) == 0)
        {
            struct sockaddr_in clientAddress;
            socklen_t clientAddressLength = sizeof(clientAddress);
            /* Wait for a client to connect */
            channel->status = "Listening";
            channel->clientSocketId = accept(serverSocketId, (struct sockaddr *) &clientAddress, &clientAddressLength);
            LogChannelStatus(channel);
            if (channel->clientSocketId >= 0)
            {
                channel->status = CONNECTED;
                ListenToClient(channel);
            }
            else
            {
                channel->status = "AcceptFailed";
                LogChannelStatus(channel);
            }
        }
        else
        {
            channel->status = "ListenFailed";
            LogChannelStatus(channel);
        }
    }
}


void* SocketTask(void* threadArgs)
{    
    struct sockaddr_in serverAddress; /* Local address */
    int serverSocketId; /* Socket descriptor for server */
    
    ((Channel*) threadArgs)->clientSocketId = -1; /* Any negative value means uninitialized */
    ((Channel*) threadArgs)->status = UNINITIALIZED;    
    printf("=== SocketTask: port %d ===\n", (int) (((Channel*) threadArgs)->port));

    /* Create socket for incoming connections */
    serverSocketId = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
    if (serverSocketId >= 0)
    {      
        /* Construct local address structure */
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET; /* Internet address family */
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
        serverAddress.sin_port = htons(((Channel*) threadArgs)->port);

        /* Bind to the local address */
        int result = bind(serverSocketId, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
        if (result == 0)
        {
            ((Channel*) threadArgs)->status = "Binded";
            LogChannelStatus((Channel*) threadArgs);
            RunChannelThread(serverSocketId, (Channel*) threadArgs);
        }
        else
        {
            ((Channel*) threadArgs)->status = "BindFailed";
            LogChannelStatus((Channel*) threadArgs);
        }
    }
    else 
    {
        ((Channel*) threadArgs)->status = "CreateSocketFailed";
        LogChannelStatus((Channel*) threadArgs);
    }
    
    printf("=== Unexpected thread end: %s ===\n", (char*) (((Channel*) threadArgs)->status));
    return NULL;
}


/*
** Interface
*/

Status InitSocketChannel(Channel* channel)
{
    channel->inputBuffer = malloc(channel->bufferSize);
    channel->inputIndex = 0;
    channel->inputAvailable = FALSE;
    pthread_t threadID;
    int error = pthread_create(&threadID, NULL, SocketTask, (void*) channel);
    if (error == 0)
    {
        printf("=== Socket thread created ===\n");
        return OK;
    }
    else
    {
        printf("=== Socket thread creation failed (error = %d) ===\n", error);
        return "SocketCreationFailed";
    }
}


void DeinitSocketChannel(Channel* channel)
{
    channel->clientSocketId = -1;
    channel->inputAvailable = FALSE;
    channel->status = "Closed";
    free(channel->inputBuffer);
}


void ReadSocketChannel(Channel* channel, char* string)
{
    if (channel->inputAvailable)
    {
        strncpy(string, channel->inputBuffer, channel->inputIndex);
        string[channel->inputIndex] = '\0';
        channel->inputIndex = 0;
        channel->inputAvailable = FALSE;
    }
    else
    {
        string[0] = '\0';
    }
}
