#include "kinematics.h"
#include <math.h>

void Kinematics_GetTargetSpeeds(Kinematics *kin, float lx, float ly, float rx, float *speedFR, float *speedFL, float *speedBR, float *speedBL) {
    float WH = kin->wheel_radius;  // ホイール半径
    float DI = kin->robot_diameter / 2;  // ロボットの半径（中心からホイールまでの距離）

    // 各ホイール速度の計算
    switch (kin->mode) {
    	case OMNI_3:
    		// 120度間隔で配置された3輪オムニホイールの速度計算式
    		rx = rx * M_PI/180;
    		*speedFR = (lx * cos(M_PI / 3) + ly * sin(M_PI / 3) + DI * rx) / (2 * M_PI * WH);
    		*speedFL = (lx * cos(M_PI) + ly * sin(M_PI) + DI * rx) / (2 * M_PI * WH);
    		*speedBR = (lx * cos(-M_PI / 3) + ly * sin(-M_PI / 3) + DI * rx) / (2 * M_PI * WH);
    		if (speedBL) *speedBL = 0; // OMNI_3では不要
    		break;

        case OMNI_4:
            // OMNI_4 モードの速度計算式（ホイールが90度ごとに配置されている場合）
        	 rx = rx * M_PI/180;
            *speedFR = (-(sqrt(2) / 2) * lx + (sqrt(2) / 2) * ly + DI * rx) / (2 * M_PI * WH);
            *speedFL = ((sqrt(2) / 2) * lx + (sqrt(2) / 2) * ly + DI * rx) / (2 * M_PI * WH);
            *speedBR = ((sqrt(2) / 2) * lx - (sqrt(2) / 2) * ly + DI * rx) / (2 * M_PI * WH);
            *speedBL = (-(sqrt(2) / 2) * lx - (sqrt(2) / 2) * ly + DI * rx) / (2 * M_PI * WH);
            break;

        case MEKANUM:
            // MEKANUM モードの速度計算式（メカナムホイール特有の動き）
        	 rx = rx * M_PI/180;
            *speedFR = ((lx - ly) / sqrt(2) + DI * rx) / (2 * M_PI * WH);
            *speedFL = ((-lx - ly) / sqrt(2) + DI * rx) / (2 * M_PI * WH);
            *speedBR = ((-lx + ly) / sqrt(2) + DI * rx) / (2 * M_PI * WH);
            *speedBL = ((lx + ly) / sqrt(2) + DI * rx) / (2 * M_PI * WH);
            break;
    }
}
