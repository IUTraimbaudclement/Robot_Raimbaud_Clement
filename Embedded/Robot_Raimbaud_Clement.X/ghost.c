#include "ghost.h"

#define FREQ 250
#define v0max 10

void calculGhost()
{

    0restant = ModuloByAngle(0ghost, 0waypoint) - 0ghost;
    
    0arret = v0*v0 / (2*acc0);
    increment0 = v0 / FREQ;
    
    
    if(v0 < 0)
        0arret = -0arret;
    
    if( (0arret >= 0 && 0restant >= 0) || (0arret <= 0 && 0restant <= 0) )
        // On accélère en rampe saturée
        if(0restant > 0)
        // Si la destination est devant, on accélère en positif en saturant la vitesse à V?Max
            v0 = Min(v0 + acc0/FREQ, v0max);
        else if (0restant < 0)
            //Si la destination est derrière, on accélère en négatif en saturant la vitesse à ?V?Max
            return; // CHANGER
    else 
    {
        // On freine en rampe saturée
        if(v0 > 0)
            //Si la vitesse positive est positive, on freine en positif en saturant la vitesse à 0
            return; // CHANGER
        else if (v0 < 0)
            //Si la vitesse est négative, on freine en négatif en saturant la vitesse à 0
            return; // CHANGER
        
        if(Abs(thetaRestant) < Abs(incrementAng))
            increment0 = 0restant
    }
    
    // On intègre le déplacement
    0ghost = 0ghost + increment0;
    
    // On gère les erreurs numériques d?arrondis
    if(v0 == 0 && Abs(0restant) < 0.01)
        0ghost == 0waypoint;
}

void ModuloByAngle(float 0ghost, float 0waypoint)
{
    
}



