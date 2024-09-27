#ifndef ALTAIR_SERIAL_H
#define ALTAIR_SERIAL_H

#include "mbed.h"
#include <cstring>  // memcpyを使うため

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
                serial = new BufferedSerial(PC_10, PC_11, baudRate);
                break;
            case USB_MiniB:
                serial = new BufferedSerial(USBTX, USBRX, baudRate);
                break;
        }
    }

    // Float型配列をヘッダーとともに送信
    void sendFloatArrayWithHeader(float* data, int length) {
        uint8_t header = 0xA5;
        serial->write(&header, 1);  // ヘッダー送信
        for (int i = 0; i < length; i++) {
            sendFloat(data[i]);  // 各floatを送信
        }
    }

    // ヘッダー付きのFloat型配列を受信
    void receiveFloatArrayWithHeader(float* buffer, int& length) {
        uint8_t header;
        do {
            serial->read(&header, 1);  // ヘッダー待ち
        } while (header != 0xA5);

        for (int i = 0; i < length; i++) {
            buffer[i] = receiveFloat();  // 各floatを受信
        }
    }

private:
    BufferedSerial* serial;

    // Floatをバイト列に変換して送信
    void sendFloat(float value) {
        uint8_t floatBytes[sizeof(float)];
        memcpy(floatBytes, &value, sizeof(float));
        serial->write(floatBytes, sizeof(float));
    }

    // バイト列をFloatに変換して受信
    float receiveFloat() {
        uint8_t floatBytes[sizeof(float)];
        serial->read(floatBytes, sizeof(float));
        float value;
        memcpy(&value, floatBytes, sizeof(float));
        return value;
    }
};

#endif
