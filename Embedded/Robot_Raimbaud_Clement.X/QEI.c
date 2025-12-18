#include <xc.h>
#include "QEI.h"
#include "Toolbox.h"

#define DISTROUES 0.2812
#define FREQ_ECH_QEI 250
//#define PI 3.14159265358979323846
#define POSITION_DATA 0x0061

void InitQEI1()
{
    QEI1IOCbits.SWPAB = 1; //QEAx and QEBx are swapped
    QEI1GECL = 0xFFFF;
    QEI1GECH = 0xFFFF;
    QEI1CONbits.QEIEN = 1; // Enable QEI Module
}

void InitQEI2()
{
    QEI2IOCbits.SWPAB = 1; //QEAx and QEBx are not swapped
    QEI2GECL = 0xFFFF;
    QEI2GECH = 0xFFFF;
    QEI2CONbits.QEIEN = 1; // Enable QEI Module
}

double QeiDroitPosition = 0;
double QeiGauchePosition = 0;

void QEIUpdateData()
{
    
    //On sauvegarde les anciennes valeurs
    double QeiDroitPosition_T_1 = QeiDroitPosition;
    double QeiGauchePosition_T_1 = QeiGauchePosition;
    
    //On actualise les valeurs des positions
    long QEI1RawValue = POS1CNTL;
    QEI1RawValue += ((long)POS1HLD<<16);
    long QEI2RawValue = POS2CNTL;
    QEI2RawValue += ((long)POS2HLD<<16);
    
    //Conversion en mm (regle pour la taille des roues codeuses)
    double QeiDroitPosition = 0.00001620*QEI1RawValue;
    double QeiGauchePosition = -0.00001620*QEI2RawValue;
    
    //Calcul des deltas de position
    double delta_d = QeiDroitPosition - QeiDroitPosition_T_1;
    double delta_g = QeiGauchePosition - QeiGauchePosition_T_1;
    
    //Calcul des vitesses
    //attention a remultiplier par la frequence d echantillonnage
    //float delta_s = (delta_d + delta_g) * 0.5f;
    robotState.vitesseDroitFromOdometry = delta_d*FREQ_ECH_QEI;
    robotState.vitesseGaucheFromOdometry = delta_g*FREQ_ECH_QEI;
    robotState.vitesseLineaireFromOdometry = (robotState.vitesseDroitFromOdometry + robotState.vitesseGaucheFromOdometry) / 2 // Moyenne des deux vitesses
    robotState.vitesseAngulaireFromOdometry = (robotState.vitesseDroitFromOdometry - robotState.vitesseGaucheFromOdometry) / DISTROUES // Différences des deux vitesse par
                                                                                                                                       // l'entraxe des roues
    //Mise a jour du positionnement terrain a t-1
    robotState.xPosFromOdometry_1 = robotState.xPosFromOdometry;
    robotState.yPosFromOdometry_1 = robotState.yPosFromOdometry;
    robotState.angleRadianFromOdometry_1 = robotState.angleRadianFromOdometry;
    
    //Calcul des positions dans le referentiel du terrain
    robotState.xPosFromOdometry = robotState.xPosFromOdometry_1 + robotState.vitesseLineaireFromOdometry * sin(robotState.vitesseAngulaireFromOdometry) * 1 / FREQ_ECH_QEI;
    robotState.yPosFromOdometry = robotState.yPosFromOdometry_1 + robotState.vitesseLineaireFromOdometry * cos(robotState.vitesseAngulaireFromOdometry) * 1 / FREQ_ECH_QEI;
    robotState.angleRadianFromOdometry = robotState.angleRadianFromOdometry_1 + robotState.vitesseAngulaireFromOdometry * 1 / FREQ_ECH_QEI;
    if(robotState.angleRadianFromOdometry > PI)
        robotState.angleRadianFromOdometry -= 2*PI;
    if(robotState.angleRadianFromOdometry < -PI)
        robotState.angleRadianFromOdometry += 2*PI;
}

void SendPositionData()
{
    unsigned char positionPayload[24];
    getBytesFromInt32(positionPayload, 0, timestamp);
    getBytesFromFloat(positionPayload, 4, (float)(robotState.xPosFromOdometry));
    getBytesFromFloat(positionPayload, 8, (float)(robotState.yPosFromOdometry));
    getBytesFromFloat(positionPayload, 12, (float)(robotState.angleRadianFromOdometry));
    getBytesFromFloat(positionPayload, 16, (float)(robotState.vitesseLineaireFromOdometry));
    getBytesFromFloat(positionPayload, 20, (float)(robotState.vitesseAngulaireFromOdometry));
    UartEncodeAndSendMessage(POSITION_DATA, 24, positionPayload);
}


