#include "InverseKinematics.h"

InverseKinematics::InverseKinematics() {
    position = {0.0f, 0.0f, 0.0f};
}

void InverseKinematics::setWheelParameters(int index, Encoder* encoder, float angle, float distance, float diameter) {
    wheels[index].encoder = encoder;
    wheels[index].angle = angle;
    wheels[index].distance = distance;
    wheels[index].diameter = diameter;
}

void InverseKinematics::updatePosition() {
    float v_x = 0.0f;
    float v_y = 0.0f;
    float delta_theta = 0.0f;
    float dt = 0.01f;
    const float rotation_diameter = 450.0f * 2;

    for (int i = 0; i < 3; i++) {
        float rps = wheels[i].encoder->getRPS();
        float wheel_velocity = wheelToVelocity(rps, wheels[i].diameter);
        float wheel_movement = wheel_velocity * dt;
        float angle_rad = wheels[i].angle * M_PI / 180.0f;

        v_x -= wheel_movement * cos(angle_rad);
        v_y += wheel_movement * sin(angle_rad);
        delta_theta -= wheel_movement / rotation_diameter;
    }

    position.x += v_x;
    position.y += v_y;
    position.theta += delta_theta * 180.0f / M_PI;

    position.theta = fmod(position.theta, 360.0f);
    if (position.theta < 0) {
        position.theta += 360.0f;
    }
}

Position InverseKinematics::getPosition() {
    return position;
}

float InverseKinematics::wheelToVelocity(float rps, float diameter) {
    return (rps * M_PI * diameter);
}
