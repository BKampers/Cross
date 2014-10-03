#include <string.h>

#include "ApiStatus.h"
#include "Communication.h"
#include "Lcd.h"
#include "Leds.h"


char message[255];
uint8_t leds = 0;


int main(void)
{
    InitLeds();
    InitLcd();
    SetLeds(0x80);

    Status status = OpenCommunicationChannel(0, 0xFF);
    if (status == OK)
    {
        WriteString("{ \"Status\" : \"OK\" }\r\n");
    }
    PutLcdLine(0, status);
    UpdateLcd();
    status = OpenCommunicationChannel(1, 32);
    if (status == OK)
    {
        WriteString("{ \"Status\" : \"OK\" }\r\n");
    }
    PutLcdLine(1, status);
    UpdateLcd();

    status = OpenCommunicationChannel(1, 64);
    if (status == OK)
    {
        WriteString("{ \"Reopen1\" : \"FAIL\" }\r\n");
    }
    status = CloseCommunicationChannel(1);
    if (status != OK)
    {
        WriteString("{ \"Close1\" : \"");
        WriteString(status);
        WriteString("\" }\r\n");
    }
    status = OpenCommunicationChannel(1, 0xFF);
    if (status != OK)
    {
        WriteString("{ \"Open1\" : \"");
        WriteString(status);
        WriteString("\" }\r\n");
    }

    for (;;)
    {
        if (status == OK)
        {
            status = ReadString(message);
            if ((status == OK) && (strlen(message) > 0))
            {
                SetLeds(0x01);
                PutLcdLine(0, message);
                UpdateLcd();
                status = WriteString(message);
                SetLeds((status == OK) ? 0x05 : 0x11);
                status = WriteString("\r\n");
                SetLeds((status == OK) ? 0x05 : 0x21);
            }
            status = ReadChannel(1, message);
            if ((status == OK) && (strlen(message) > 0))
            {
                SetLeds(2);
                PutLcdLine(1, message);
                UpdateLcd();
                status = WriteChannel(1, message);
                SetLeds((status == OK) ? 0x06 : 0x12);
                status = WriteChannel(1, "\r\n");
                SetLeds((status == OK) ? 0x06 : 0x22);
            }
        }
    }
}
