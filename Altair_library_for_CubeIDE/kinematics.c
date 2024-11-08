#include "kinematics.h"
#include <math.h>

void Kinematics_GetTargetSpeeds(Kinematics *kin, float lx, float ly, float rx, float *speedFR, float *speedFL, float *speedBR, float *speedBL) {
    float WH = kin->wheel_radius; // ホイール半径
    float DI = kin->robot_diameter / 2; // ロボットの半径

    switch (kin->mode) {
        case OMNI_3:
            *speedFR = (-lx + DI * rx) / (2 * M_PI * WH);
            *speedFL = (lx / 2 - ly * sqrt(3) / 2 + DI * rx) / (2 * M_PI * WH);
            *speedBR = (lx / 2 + ly * sqrt(3) / 2 + DI * rx) / (2 * M_PI * WH);
            if (speedBL) *speedBL = 0; // OMNI_3では不要
            break;

        case OMNI_4:
            *speedFR = (-(sqrt(2) / 2) * lx + (sqrt(2) / 2) * ly + DI * rx) / (2 * M_PI * WH);
            *speedFL = ((sqrt(2) / 2) * lx + (sqrt(2) / 2) * ly + DI * rx) / (2 * M_PI * WH);
            *speedBR = ((sqrt(2) / 2) * lx - (sqrt(2) / 2) * ly + DI * rx) / (2 * M_PI * WH);
            *speedBL = (-(sqrt(2) / 2) * lx - (sqrt(2) / 2) * ly + DI * rx) / (2 * M_PI * WH);
            break;

        case MEKANUM:
            *speedFR = ((lx - ly) / sqrt(2) + DI * rx) / (2 * M_PI * WH);
            *speedFL = ((-lx - ly) / sqrt(2) + DI * rx) / (2 * M_PI * WH);
            *speedBR = ((-lx + ly) / sqrt(2) + DI * rx) / (2 * M_PI * WH);
            *speedBL = ((lx + ly) / sqrt(2) + DI * rx) / (2 * M_PI * WH);
            break;
    }
}
