#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

class Encoder {
public:
    Encoder(uint8_t pinA, uint8_t pinB);

    int32_t getCount();
    float getRPS();
    void reset();

private:
    void counterArise();
    void counterAfall();
    void counterBrise();
    void counterBfall();

    volatile int32_t encoder_count;
    int32_t last_count;
    uint8_t pinA, pinB;
    unsigned long last_time;
};

#endif // ENCODER_H
