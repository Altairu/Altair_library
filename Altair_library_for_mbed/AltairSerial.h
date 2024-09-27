#ifndef ALTAIR_SERIAL_H
#define ALTAIR_SERIAL_H

#include "mbed.h"

enum USBMode {
    USB_A,
    USB_B,
    USB_MiniB
};

class AltairSerial {
public:
    AltairSerial(USBMode mode, int baudRate = 9600) {
        switch (mode) {
            case USB_A:
                serial = new BufferedSerial(PA_9, PA_10, baudRate);
                break;
            case USB_B:
                serial = new BufferedSerial(PC_11, PC_10, baudRate);
                break;
            case USB_MiniB:
                serial = new BufferedSerial(USBTX, USBRX, baudRate);
                break;
        }
    }

    void sendFloatArrayWithHeader(float* data, int length) {
        uint8_t header = 0xA5;
        serial->write(&header, 1);
        serial->write(data, length * sizeof(float));
    }

    void receiveFloatArrayWithHeader(float* buffer, int& length) {
        uint8_t header;
        do {
            serial->read(&header, 1);
        } while (header != 0xA5);

        serial->read(buffer, length * sizeof(float));
    }

private:
    BufferedSerial* serial;
};

#endif

