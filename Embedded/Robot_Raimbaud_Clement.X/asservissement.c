#include "asservissement.h"
#include "QEI.h"
#include "Robot.h"
#include "UART_Protocol.h"
#include "Toolbox.h"
#include "PWM.h"

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
    double erreurProportionnelle = LimitToInterval(erreur, -PidCorr->erreurProportionelleMax/PidCorr->Kp, PidCorr->erreurProportionelleMax/PidCorr->Kp);
    PidCorr->corrP = erreurProportionnelle * PidCorr->Kp;
    PidCorr->erreurIntegrale += erreur / FREQ_ECH_QEI;
    PidCorr->erreurIntegrale = LimitToInterval(PidCorr->erreurIntegrale, -PidCorr->erreurIntegraleMax/PidCorr->Ki, PidCorr->erreurIntegraleMax/PidCorr->Ki);
    PidCorr->corrI = PidCorr->erreurIntegrale * PidCorr->Ki;
    double erreurDerivee = (erreur - PidCorr->epsilon_1)*FREQ_ECH_QEI;
    double deriveeBornee = LimitToInterval(erreurDerivee, -PidCorr->erreurDeriveeMax/PidCorr->Kd, PidCorr->erreurDeriveeMax/PidCorr->Kd);
    PidCorr->epsilon_1 = erreur;
    PidCorr->corrD = deriveeBornee * PidCorr->Kd;
    return PidCorr->corrP+PidCorr->corrI+PidCorr->corrD;
}

void UpdateAsservissement()
{
    robotState.PidX.erreur = robotState.consigneLineaireFromOdometry - robotState.vitesseLineaireFromOdometry;
    robotState.PidTheta.erreur = robotState.consigneAngulaireFromOdometry - robotState.vitesseAngulaireFromOdometry;
    
    robotState.CorrectionVitesseLineaire = Correcteur(&robotState.PidX, robotState.PidX.erreur);
    robotState.CorrectionVitesseAngulaire = Correcteur(&robotState.PidTheta, robotState.PidTheta.erreur);
    
    PWMSetSpeedConsignePolar(robotState.CorrectionVitesseLineaire, robotState.CorrectionVitesseAngulaire);
}

void SendPidData()
{
    unsigned char PidPayload[72];
    
    getBytesFromFloat(PidPayload, 0, robotState.PidX.Kp);
    getBytesFromFloat(PidPayload, 4, robotState.PidX.Ki);
    getBytesFromFloat(PidPayload, 8, robotState.PidX.Kd);
    getBytesFromFloat(PidPayload, 12, robotState.PidX.erreurProportionelleMax);
    getBytesFromFloat(PidPayload, 16, robotState.PidX.erreurIntegraleMax);
    getBytesFromFloat(PidPayload, 20, robotState.PidX.erreurDeriveeMax); 
    getBytesFromFloat(PidPayload, 24, robotState.PidX.corrP); 
    getBytesFromFloat(PidPayload, 28, robotState.PidX.corrI); 
    getBytesFromFloat(PidPayload, 32, robotState.PidX.corrD); 
    
    getBytesFromFloat(PidPayload, 36, robotState.PidTheta.Kp);
    getBytesFromFloat(PidPayload, 40, robotState.PidTheta.Ki);
    getBytesFromFloat(PidPayload, 44, robotState.PidTheta.Kd);
    getBytesFromFloat(PidPayload, 48, robotState.PidTheta.erreurProportionelleMax);
    getBytesFromFloat(PidPayload, 52, robotState.PidTheta.erreurIntegraleMax);
    getBytesFromFloat(PidPayload, 56, robotState.PidTheta.erreurDeriveeMax); 
    getBytesFromFloat(PidPayload, 60, robotState.PidTheta.corrP); 
    getBytesFromFloat(PidPayload, 64, robotState.PidTheta.corrI); 
    getBytesFromFloat(PidPayload, 68, robotState.PidTheta.corrD); 
    
    UartEncodeAndSendMessage(PID_DATA, 72, PidPayload);
}

void SendAsservData()
{
    unsigned char AsservPayload[32];
    
    getBytesFromFloat(AsservPayload, 0, robotState.consigneLineaireFromOdometry);
    getBytesFromFloat(AsservPayload, 4, robotState.consigneAngulaireFromOdometry);
    getBytesFromFloat(AsservPayload, 8, robotState.vitesseLineaireFromOdometry);
    getBytesFromFloat(AsservPayload, 12, robotState.vitesseAngulaireFromOdometry);   
    getBytesFromFloat(AsservPayload, 16, robotState.PidX.erreur);
    getBytesFromFloat(AsservPayload, 20, robotState.PidTheta.erreur);   
    getBytesFromFloat(AsservPayload, 24, robotState.CorrectionVitesseLineaire);
    getBytesFromFloat(AsservPayload, 28, robotState.CorrectionVitesseAngulaire);   
       
    UartEncodeAndSendMessage(ASSERV, 32, AsservPayload);
}
