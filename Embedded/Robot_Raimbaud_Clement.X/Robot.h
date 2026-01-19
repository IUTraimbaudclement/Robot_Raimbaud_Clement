#ifndef ROBOT_H
#define ROBOT_H

#include "Asservissement.h"

typedef struct robotStateBITS {
    union {
        struct {
            unsigned char taskEnCours;
            unsigned char mode;
            float vitesseGaucheConsigne;
            float vitesseGaucheCommandeCourante;
            float vitesseDroiteConsigne;
            float vitesseDroiteCommandeCourante;
            float distanceTelemetreGaucheToute;           
            float distanceTelemetreGauche;
            float distanceTelemetreCentre;
            float distanceTelemetreDroit;
            float distanceTelemetreDroitToute;
            double vitesseDroitFromOdometry;
            double vitesseGaucheFromOdometry;
            double vitesseLineaireFromOdometry;
            double vitesseAngulaireFromOdometry;
            double xPosFromOdometry_1;
            double yPosFromOdometry_1;
            double angleRadianFromOdometry_1;
            double xPosFromOdometry;
            double yPosFromOdometry;
            double angleRadianFromOdometry;
            };
    };
} ROBOT_STATE_BITS;

extern volatile ROBOT_STATE_BITS robotState;

extern volatile PidCorrector* PidX;
extern volatile PidCorrector* PidTheta;

#endif /* ROBOT_H */
