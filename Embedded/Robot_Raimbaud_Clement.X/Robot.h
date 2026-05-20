#ifndef ROBOT_H
#define ROBOT_H

#define DISTROUES 0.219

#include "Asservissement.h"

typedef struct robotStateBITS {
    union {
        struct {
            unsigned char taskEnCours;
            int mode;
            float vitesseGaucheConsigne;
            float vitesseGaucheCommandeCourante;
            float vitesseDroiteConsigne;
            float vitesseDroiteCommandeCourante;
            float distanceTelemetreGaucheToute;           
            float distanceTelemetreGauche;
            float distanceTelemetreCentre;
            float distanceTelemetreDroit;
            float distanceTelemetreDroitToute;
            float vitesseDroitFromOdometry;
            float vitesseGaucheFromOdometry;
            float vitesseLineaireFromOdometry;
            float vitesseAngulaireFromOdometry;
            float consigneLineaireFromOdometry;
            float consigneAngulaireFromOdometry;
            float xPosFromOdometry_1;
            float yPosFromOdometry_1;
            float angleRadianFromOdometry_1;
            float xPosFromOdometry;
            float yPosFromOdometry;
            float angleRadianFromOdometry;
            PidCorrector PidX;
            PidCorrector PidTheta;
            float CorrectionVitesseLineaire;
            float CorrectionVitesseAngulaire;
            };
    };
} ROBOT_STATE_BITS;

extern volatile ROBOT_STATE_BITS robotState;

extern volatile PidCorrector PidX;
extern volatile PidCorrector PidTheta;

#endif /* ROBOT_H */
