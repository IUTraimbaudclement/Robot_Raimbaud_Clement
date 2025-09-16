#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "pwm.h"
#include "robot.h"
#include "ADC.h"

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
    // Boucle Principale
    /***********************************************************************************************/
    
    
    
    
    robotState.vitesseGaucheCommandeCourante = 0;
    robotState.vitesseDroiteCommandeCourante = 0;
    
    PWMSetSpeedConsigne(MOTEUR_GAUCHE, 0.0);
    PWMSetSpeedConsigne(MOTEUR_DROIT, 0.0); 
    
while(1)
{
    
    if(ADCIsConversionFinished() == 1) {
        unsigned int * result = ADCGetResult();
        ADCClearConversionFinishedFlag();
        
        // 20cm = 0x0667
        // 40 cm = 0x02E6
        
        float volts = ((float) result [0])* 3.3 / 4096;
        robotState.distanceTelemetreGauche = 34 / volts - 5;
        volts = ((float) result [1])* 3.3 / 4096;
        robotState.distanceTelemetreCentre = 34 / volts - 5;
        volts = ((float) result [2])* 3.3 / 4096;
        robotState.distanceTelemetreDroit = 34 / volts - 5;
        
        if(robotState.distanceTelemetreGauche < 30.0)
            LED_BLANCHE_1 = 1;
        else
            LED_BLANCHE_1 = 0;
        
        if(robotState.distanceTelemetreCentre < 30.0)
            LED_BLEUE_1 = 1;
        else
            LED_BLEUE_1 = 0;    
        
        if(robotState.distanceTelemetreDroit < 30.0)
            LED_ORANGE_1 = 1;
        else
            LED_ORANGE_1 = 0;           
        
    }
    
} // fin main
}