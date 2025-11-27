#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "pwm.h"
#include "robot.h"
#include "ADC.h"
#include "main.h"
#include "UART.h"
#include "CB_TX1.h"

unsigned char stateRobot;

int main (void){
    /***********************************************************************************************/
    //    Initialisation oscillateur
    /***********************************************************************************************/
    InitOscillator();
    /***********************************************************************************************/
    // Configuration des input et output (IO)
    /***********************************************************************************************/
    InitIO();
    InitADC1();
    LED_BLANCHE_1 = 1;
    LED_BLEUE_1 = 1;
    LED_ORANGE_1 = 1;
    LED_ROUGE_1 = 1;
    LED_VERTE_1 = 1;

    LED_BLANCHE_2 = 1;
    LED_BLEUE_2 = 1;
    LED_ORANGE_2 = 1;
    LED_ROUGE_2 = 1;
    LED_VERTE_2 = 1;
    /***********************************************************************************************/
    //    Initialisation pwm
    /***********************************************************************************************/
    InitPWM();
    
    /***********************************************************************************************/
    //    Initialisation timers
    /***********************************************************************************************/
    InitTimer23();
    InitTimer1();
    InitTimer4();
    
    /***********************************************************************************************/
    //    Initialisation vitesse
    /***********************************************************************************************/
    robotState.vitesseGaucheCommandeCourante = 0;
    robotState.vitesseDroiteCommandeCourante = 0;
    
    
    /***********************************************************************************************/
    //    Initialisation UART
    /***********************************************************************************************/
    InitUART();
    
    /***********************************************************************************************/
    // Boucle Principale
    /***********************************************************************************************/
    
    SendMessage((unsigned char*) "TEST", 4);  
    SendMessage((unsigned char*) "Bonjour", 7); 
    SendMessage((unsigned char*) "Ceci est un", 11); 
    SendMessage((unsigned char*) "Message du", 10); 
    SendMessage((unsigned char*) "ROBOT !!", 8); 
    SendMessage((unsigned char*) "Bye", 3); 
    
    
    uint8_t motorOn = 0;
    while(1)
    {
        if(motorOn == 0)
            stateRobot = STATE_ATTENTE;
        
        if(ADCIsConversionFinished() == 1 && motorOn == 1) 
        {
            unsigned int * result = ADCGetResult();
            ADCClearConversionFinishedFlag();

            // 20cm = 0x0667
            // 40 cm = 0x02E6

            float volts = ((float) result [0])* 3.3 / 4096;
            robotState.distanceTelemetreGaucheToute = 34 / volts - 5;
            volts = ((float) result [1])* 3.3 / 4096;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result [2])* 3.3 / 4096;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [3])* 3.3 / 4096;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
            volts = ((float) result [4])* 3.3 / 4096;
            robotState.distanceTelemetreDroitToute = 34 / volts - 5;        
            
            /*
            if(robotState.distanceTelemetreGaucheToute < 30.0)
                LED_BLANCHE_1 = 1;
            else
                LED_BLANCHE_1 = 0;

            if(robotState.distanceTelemetreGauche < 30.0)
                LED_BLEUE_1 = 1;
            else
                LED_BLEUE_1 = 0;    

            if(robotState.distanceTelemetreCentre < 30.0)
                LED_ORANGE_1 = 1;
            else
                LED_ORANGE_1 = 0;    
            
             if(robotState.distanceTelemetreDroit < 30.0)
                LED_ROUGE_1 = 1;
            else
                LED_ROUGE_1 = 0; 
            
            if(robotState.distanceTelemetreDroitToute < 30.0)
                LED_VERTE_1 = 1;
            else
                LED_VERTE_1 = 0;    
            */
            
        }
    
    } // fin main
}

void OperatingSystemLoop(void)
{
    switch (stateRobot)
    {
        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;
        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
            break;
        case STATE_AVANCE:
            PWMSetSpeedConsigne(30, MOTEUR_DROIT);
            PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;
        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(10, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(10, MOTEUR_DROIT);
            PWMSetSpeedConsigne(10, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_SUR_PLACE:
            PWMSetSpeedConsigne(10, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-10, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_EN_COURS:
           SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_VITE_GAUCHE:
            PWMSetSpeedConsigne(10, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-10, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_VITE_GAUCHE_EN_COURS;
            timestamp = 0;
            break;
        case STATE_TOURNE_VITE_GAUCHE_EN_COURS:
            if(timestamp > 500)
                SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_VITE_DROITE:
            PWMSetSpeedConsigne(-10, MOTEUR_DROIT);
            PWMSetSpeedConsigne(10, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_VITE_DROITE_EN_COURS;
            timestamp = 0;
            break;
        case STATE_TOURNE_VITE_DROITE_EN_COURS:
            if(timestamp > 500)
                SetNextRobotStateInAutomaticMode();
            break;   
        case STATE_AVANCE_LENT:
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_LENT_EN_COURS;
            break;
        case STATE_AVANCE_LENT_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;        
        default :
            stateRobot = STATE_ATTENTE;
            break;
        }
    }


unsigned char nextStateRobot=0;

void SetNextRobotStateInAutomaticMode()
{
    unsigned char positionObstacle = PAS_D_OBSTACLE;
    //éDtermination de la position des obstacles en fonction des ééètlmtres
    if ( robotState.distanceTelemetreDroit < 40 &&
        robotState.distanceTelemetreCentre > 30 &&
        robotState.distanceTelemetreGauche > 40) //Obstacle àdroite
        positionObstacle = OBSTACLE_A_DROITE;
    else if(robotState.distanceTelemetreDroit > 40 &&
        robotState.distanceTelemetreCentre > 30 &&
        robotState.distanceTelemetreGauche < 40) //Obstacle àgauche
        positionObstacle = OBSTACLE_A_GAUCHE;
    else if(robotState.distanceTelemetreCentre < 40) //Obstacle en face
        positionObstacle = OBSTACLE_EN_FACE;
    else if(robotState.distanceTelemetreDroit > 40 &&
        robotState.distanceTelemetreCentre > 30 &&
        robotState.distanceTelemetreGauche > 40) //pas d?obstacle
        positionObstacle = PAS_D_OBSTACLE;
    else if(robotState.distanceTelemetreDroit < 40 &&
            robotState.distanceTelemetreGauche < 40)
        positionObstacle = OBSTACLE_GAUCHE_DROITE;
                
    //éDtermination de lé?tat àvenir du robot
    if(positionObstacle == PAS_D_OBSTACLE)
        nextStateRobot = STATE_AVANCE;
    else if (positionObstacle == OBSTACLE_A_DROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;
    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;
     else if (positionObstacle == OBSTACLE_GAUCHE_DROITE)
        nextStateRobot = STATE_AVANCE_LENT;
       
    else if (positionObstacle == OBSTACLE_EN_FACE)
    {
        if(robotState.distanceTelemetreGaucheToute > 30 && robotState.distanceTelemetreGaucheToute > robotState.distanceTelemetreDroitToute)
            nextStateRobot = STATE_TOURNE_VITE_GAUCHE; 
        else if(robotState.distanceTelemetreDroitToute > 30 && robotState.distanceTelemetreDroitToute > robotState.distanceTelemetreGaucheToute)
            nextStateRobot = STATE_TOURNE_VITE_DROITE;       
        else
            nextStateRobot = STATE_TOURNE_SUR_PLACE;          
    }

    //Si l?on n?est pas dans la transition de lé?tape en cours
    if (nextStateRobot != stateRobot - 1)
    {
        stateRobot = nextStateRobot; 
        LED_BLANCHE_1 = 0;
        LED_BLEUE_1 = 0;
        LED_ORANGE_1 = 0;
        LED_ROUGE_1 = 0;
        LED_VERTE_1 = 0;

        if(positionObstacle == OBSTACLE_A_GAUCHE)
        {
            LED_BLANCHE_1 = 1;
            LED_BLEUE_1 = 1; 
        }
         else if(positionObstacle == OBSTACLE_A_DROITE)
        {
            LED_ROUGE_1 = 1;
            LED_VERTE_1 = 1; 
        }   
           else if(positionObstacle == OBSTACLE_GAUCHE_DROITE)
        {
            LED_BLANCHE_1 = 1;
            LED_VERTE_1 = 1; 
        }         
        else if(positionObstacle == OBSTACLE_EN_FACE)
        {
            if(nextStateRobot == STATE_TOURNE_VITE_GAUCHE)
                LED_VERTE_1 = 1;
            else if(nextStateRobot == STATE_TOURNE_VITE_DROITE)
                LED_BLANCHE_1 = 1;   
            else if(nextStateRobot == STATE_TOURNE_SUR_PLACE)
            {
                LED_BLANCHE_1 = 1;
                LED_BLEUE_1 = 1;
                LED_ORANGE_1 = 1;
                LED_ROUGE_1 = 1;
                LED_VERTE_1 = 1; 
            }
        }     
    }
}