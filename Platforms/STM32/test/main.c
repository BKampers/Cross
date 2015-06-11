#include <stdio.h>
#include <string.h>

#include "ApiStatus.h"
#include "Communication.h"
#include "Lcd.h"
#include "Pins.h"


char message[255];
uint8_t leds = 0;


int main(void)
{
    InitOutputPins();
    InitLcd();
    SetOutputPins(0x8000);

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
                int pin, w = 0;
                SetOutputPins(0x0100);
                ResetOutputPins(0x8000);
                PutLcdLine(0, message);
                status = WriteString(message);
                SetOutputPins((status == OK) ? 0x200 : 0x800);
                status = WriteString("\r\n");
                SetOutputPins((status == OK) ? 0x400 : 0x800);
                for (pin = 0x0100; pin <= 0x8000; pin = pin << 1)
                {
                    if (IsOutputPinSet(pin))
                    {
                        w |= pin;
                    }
                }
                sprintf(message, "{\"w\":\"0x%04x\"}", w);
                PutLcdLine(1, message);
                UpdateLcd();
            }
            status = ReadChannel(1, message);
            if ((status == OK) && (strlen(message) > 0))
            {
                SetOutputPins(0x0200);
                PutLcdLine(1, message);
                UpdateLcd();
                status = WriteChannel(1, message);
                SetOutputPins((status == OK) ? 0x200 : 0x800);
                status = WriteChannel(1, "\r\n");
                SetOutputPins((status == OK) ? 0x400 : 0x800);
            }
        }
    }
}
