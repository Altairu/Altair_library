#ifndef INVERSE_KINEMATICS_H
#define INVERSE_KINEMATICS_H

#include "encoder.h"
#include <cmath>

struct Position {
    float x;
    float y;
    float theta;
};

class InverseKinematics {
public:
    InverseKinematics();  // 修正: EncoderModeを削除
    void setWheelParameters(int index, Encoder* encoder, float angle, float distance, float diameter);
    void updatePosition();
    Position getPosition();

private:
    struct Wheel {
        Encoder* encoder;
        float angle;
        float distance;
        float diameter;
    };

    Wheel wheels[3];
    Position position;

    float wheelToVelocity(float rps, float diameter);
};

#endif // INVERSE_KINEMATICS_H
