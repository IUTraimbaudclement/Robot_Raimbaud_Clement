#ifndef ASSERVISSEMENT_H
#define	ASSERVISSEMENT_H


typedef struct _PidCorrector
{
    float Kp;
    float Ki;
    float Kd;
    float erreurProportionelleMax;
    float erreurIntegraleMax;
    float erreurDeriveeMax;
    float erreurIntegrale;
    float epsilon_1;
    float erreur;
    //For Debug only
    float corrP;
    float corrI;
    float corrD;
}PidCorrector;

void SetupPidAsservissement(volatile PidCorrector* PidCorr, float Kp, float Ki, float Kd, float proportionelleMax, float integralMax, float deriveeMax);
void UpdateAsservissement();
void SendPidData();
void SendAsservData();
#endif	/* ASSERVISSEMENT_H */

