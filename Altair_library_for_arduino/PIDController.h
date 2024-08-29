#ifndef PID_CONTROLLER_H_
#define PID_CONTROLLER_H_

class PIDController {
public:
    PIDController(float Kp, float Ki, float Kd, float time_constant, float dt)
        : Kp(Kp), Ki(Ki), Kd(Kd), time_constant(time_constant), dt(dt),
          prev_error(0), integral(0), prev_output(0) {}

    float compute(float setpoint, float measured_value) {
        float error = setpoint - measured_value;
        integral += error * dt;
        float derivative = (error - prev_error) / dt;
        float output = Kp * error + Ki * integral + Kd * derivative;
        output = (prev_output * time_constant + output) / (time_constant + dt);
        prev_error = error;
        prev_output = output;
        return output;
    }

    void reset() {
        prev_error = 0;
        integral = 0;
        prev_output = 0;
    }

private:
    float Kp, Ki, Kd;
    float time_constant;
    float dt;
    float prev_error;
    float integral;
    float prev_output;
};

#endif // PID_CONTROLLER_H_
