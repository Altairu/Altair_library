#ifndef PID_H_
#define PID_H_

typedef enum
{
    P,
    I,
    D
} ControlType;

typedef struct
{
    double integral_error;
    double before_error;
    double kp;
    double ki;
    double kd;
    double d_error;
    double p_control;
    double i_control;
    double d_control;
    double time_constant;
} Pid;

void Pid_Init(Pid *pid);
void Pid_setGain(Pid *pid, double p_gain, double i_gain, double d_gain, double time_constant);
double Pid_control(Pid *pid, double target, double now, int control_period);
double Pid_controlError(Pid *pid, double error, int control_period);
void Pid_reset(Pid *pid);
double Pid_getControlValue(Pid *pid, ControlType control_type);

#endif /* PID_H_ */
