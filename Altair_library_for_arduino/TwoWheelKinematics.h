#ifndef TWO_WHEEL_KINEMATICS_H
#define TWO_WHEEL_KINEMATICS_H

class TwoWheelKinematics {
public:
    TwoWheelKinematics(float wheel_diameter, float wheel_distance)
        : wheel_diameter(wheel_diameter), wheel_distance(wheel_distance) {}

    void calculateWheelSpeeds(float v, float omega, float &vR, float &vL) {
        float R = v / omega;
        vR = (R + wheel_distance / 2.0) * omega;
        vL = (R - wheel_distance / 2.0) * omega;
    }

private:
    float wheel_diameter;
    float wheel_distance;
};

#endif // TWO_WHEEL_KINEMATICS_H
