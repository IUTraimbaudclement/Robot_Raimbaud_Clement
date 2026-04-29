#include "asservissement.h"
#include "QEI.h"
#include "Robot.h"

void SetupPidAsservissement(volatile PidCorrector* PidCorr, float Kp, float Ki, float Kd, float proportionelleMax, float integralMax, float deriveeMax)
{
    PidCorr->Kp = Kp;
    PidCorr->erreurProportionelleMax = proportionelleMax; //On limite la correction due au Kp
    PidCorr->Ki = Ki;
    PidCorr->erreurIntegraleMax = integralMax; //On limite la correction due au Ki
    PidCorr->Kd = Kd;
    PidCorr->erreurDeriveeMax = deriveeMax;
}

double Correcteur(volatile PidCorrector* PidCorr, double erreur)
{
    PidCorr->erreur = erreur;
    double erreurProportionnelle = LimitToInterval(erreur, -PidCorr->erreurProportionelleMax, PidCorr->erreurProportionelleMax);
    PidCorr->corrP = erreur * PidCorr->Kp;
    PidCorr->erreurIntegrale += erreur / FREQ_ECH_QEI;
    PidCorr->erreurIntegrale = LimitToInterval(PidCorr->erreurIntegrale, -PidCorr->erreurIntegraleMax, PidCorr->erreurIntegraleMax);
    PidCorr->corrI = PidCorr->erreurIntegrale * PidCorr->Ki;
    double erreurDerivee = (erreur - PidCorr->epsilon_1)*FREQ_ECH_QEI;
    double deriveeBornee = LimitToInterval(erreurDerivee, -PidCorr->erreurDeriveeMax/PidCorr->Kd, PidCorr->erreurDeriveeMax/PidCorr->Kd);
    PidCorr->epsilon_1 = erreur;
    PidCorr->corrD = deriveeBornee * PidCorr->Kd;
    return PidCorr->corrP+PidCorr->corrI+PidCorr->corrD;
}

void UpdateAsservissement()
{
    robotState.PidX.erreur = ...;
    robotState.PidTheta.erreur = ...;
    
    robotState.CorrectionVitesseLineaire = Correcteur(&robotState.PidX, robotState.PidX.erreur);
    robotState.CorrectionVitesseAngulaire = Correcteur(&robotState.PidTheta, robotState.PidTheta.erreur);
    
    
    PWMSetSpeedConsignePolaire(robotState.CorrectionVitesseLineaire, robotState.CorrectionVitesseAngulaire);
}
