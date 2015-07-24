#ifndef _SOCKETHANDLER_H_
#define	_SOCKETHANDLER_H_

#include <sys/socket.h>

#include "ApiStatus.h"
#include "Types.h"

#define DEFAULT_PORT 44252


typedef struct
{
    Status status;
    char* inputBuffer;
    int clientSocketId;
    uint16_t port;
    bool inputAvailable;
    uint8_t bufferSize;
    uint8_t inputIndex;
}  Channel;


Status InitSocketChannel(Channel* channel);
void DeinitSocketChannel(Channel* channel);
void ReadSocketChannel(Channel* channel, char* string);


#endif	/* _SOCKETHANDLER_H_ */

