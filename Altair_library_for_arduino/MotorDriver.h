#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <Arduino.h>

class MotorDriver
{
public:
    MotorDriver(int pin1, int pin2);

    void setSpeed(int speed);
    void brake();
    void stop();

private:
    int pin1, pin2;
    const int maxPWM = 250;

    void forward(int speed);
    void reverse(int speed);
};

#endif // MOTOR_DRIVER_H
