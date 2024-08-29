#include "MotorDriver.h"

MotorDriver::MotorDriver(int pin1, int pin2) : pin1(pin1), pin2(pin2)
{
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    stop(); // 初期状態は停止
}

void MotorDriver::setSpeed(int speed)
{
    if (speed > maxPWM)
        speed = maxPWM;
    if (speed < -maxPWM)
        speed = -maxPWM;

    if (speed > 0)
    {
        forward(speed);
    }
    else if (speed < 0)
    {
        reverse(-speed);
    }
    else
    {
        stop();
    }
}

void MotorDriver::brake()
{
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
}

void MotorDriver::forward(int speed)
{
    analogWrite(pin1, speed);
    analogWrite(pin2, 0);
}

void MotorDriver::reverse(int speed)
{
    analogWrite(pin1, 0);
    analogWrite(pin2, speed);
}

void MotorDriver::stop()
{
    analogWrite(pin1, 0);
    analogWrite(pin2, 0);
}
