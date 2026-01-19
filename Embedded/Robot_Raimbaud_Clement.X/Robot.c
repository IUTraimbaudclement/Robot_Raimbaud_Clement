#include "Robot.h"
#include "Asservissement.h"

volatile ROBOT_STATE_BITS robotState;
volatile PidCorrector* PidX;
volatile PidCorrector* PidTheta;
