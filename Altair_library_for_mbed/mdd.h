#ifndef MDD_H_
#define MDD_H_

#include "mbed.h"

union ConvertIntFloat {
    int int_val;
    uint8_t uint8_val[4];
    float float_val;
};

enum MddCommandId {
    MOTOR_RPS_COMMAND_MODE = 0,
    MOTOR_PWM_COMMAND_MODE,
    MECANUM_MODE,
    OMNI3_MODE,
    OMNI4_MODE,
    M1_PID_GAIN_CONFIG,
    M2_PID_GAIN_CONFIG,
    M3_PID_GAIN_CONFIG,
    M4_PID_GAIN_CONFIG,
    ROBOT_DIAMETER_CONFIG,
    PID_RESET_COMMAND,
    MOTOR_COMMAND_MODE_SELECT,
    ENCODER_RESOLUTION_CONFIG
};

class SkenMdd {
private:
    BufferedSerial& serial;
    uint8_t seq;
    void sendData(uint8_t id, const float (&data)[4]);
    void setFloatData(const float (&command_data)[4], uint8_t (&send_data)[21]);
    uint8_t calcChecksum(const uint8_t (&send_data)[21]);

public:
    SkenMdd(BufferedSerial& s);
    void init();
    bool tcp(uint8_t id, const float (&command_data)[4], unsigned int resend_time, unsigned int max_wait_time);
    void udp(uint8_t id, const float (&command_data)[4]);
};

#endif /* MDD_H_ */

