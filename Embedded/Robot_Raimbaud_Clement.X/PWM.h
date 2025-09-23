#ifndef PWM_H
#define PWM_H

#define MOTEUR_DROIT 0
#define MOTEUR_GAUCHE 1

void InitPWM(void);
void PWMSetSpeedConsigne(float, int);
void PWMUpdateSpeed(void);

#endif /* PWM_H */