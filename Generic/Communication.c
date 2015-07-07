#include "Communication.h"
        
        
Status FinishTransmission(int channelId)
{
    return WriteCharacter(channelId, TRANSMISSION_END);
}
