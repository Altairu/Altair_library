#include "pid.h"

void Pid_Init(Pid *pid)
{
    pid->integral_error = 0;
    pid->before_error = 0;
    pid->kp = 0;
    pid->ki = 0;
    pid->kd = 0;
    pid->d_error = 0;
    pid->p_control = 0;
    pid->i_control = 0;
    pid->d_control = 0;
    pid->time_constant = 0;
    pid->integral_limit = 0;
    pid->output_invert = 1;
}

void Pid_setGain(Pid *pid, double p_gain, double i_gain, double d_gain, double time_constant)
{
    pid->kp = p_gain;
    pid->ki = i_gain;
    pid->kd = d_gain;
    pid->time_constant = (time_constant > 0) ? time_constant : 0;
}

void Pid_setGainWithLimit(Pid *pid, double p_gain, double i_gain, double d_gain, double time_constant, double integral_limit)
{
    Pid_setGain(pid, p_gain, i_gain, d_gain, time_constant);
    pid->integral_limit = (integral_limit > 0.0) ? integral_limit : 0.0;
}

void Pid_setInvert(Pid *pid, int invert)
{
    if (invert < 0)
    {
        pid->output_invert = -1;
    }
    else
    {
        pid->output_invert = 1;
    }
}

double Pid_control(Pid *pid, double target, double now, int control_period)
{
    return Pid_controlError(pid, target - now, control_period);
}

double Pid_controlError(Pid *pid, double error, int control_period)
{
    pid->p_control = error * pid->kp;

    if (control_period > 0)
    {
        pid->integral_error += error * (control_period / 1000.0);
    }

    if (pid->integral_limit > 0.0)
    {
        if (pid->integral_error > pid->integral_limit)
        {
            pid->integral_error = pid->integral_limit;
        }
        else if (pid->integral_error < -pid->integral_limit)
        {
            pid->integral_error = -pid->integral_limit;
        }
    }

    pid->i_control = pid->integral_error * pid->ki;

    if (pid->time_constant != 0)
    {
        double alpha = pid->time_constant / (pid->time_constant + control_period);
        pid->d_error = pid->before_error * alpha + (1 - alpha) * error;
    }
    else
    {
        pid->d_error = error;
    }

    if (control_period != 0)
    {
        pid->d_control = ((pid->d_error - pid->before_error) / (control_period / 1000.0)) * pid->kd;
    }

    pid->before_error = error;
    return (pid->p_control + pid->i_control + pid->d_control) * pid->output_invert;
}

void Pid_reset(Pid *pid)
{
    pid->integral_error = 0;
    pid->before_error = 0;
    pid->d_error = 0;
}

double Pid_getControlValue(Pid *pid, ControlType control_type)
{
    switch (control_type)
    {
    case P:
        return pid->p_control;
    case I:
        return pid->i_control;
    case D:
        return pid->d_control;
    default:
        return 0;
    }
}
