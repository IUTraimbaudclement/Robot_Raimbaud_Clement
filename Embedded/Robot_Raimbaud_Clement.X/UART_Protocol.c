#include <xc.h>
#include "UART_Protocol.h"
#include "UART.h"
#include "CB_TX1.h"
#include "IO.h"
#include "robot.h"
#include "main.h"

#define WAITING 0
#define FUNCTION_MSB 1
#define FUNCTION_LSB 2
#define LENGTH_MSB 3
#define LENGTH_LSB 4
#define PAYLOAD 5
#define CHECKSUM 6

#define SET_ROBOT_STATE 0x0051
#define SET_ROBOT_MANUAL_CONTROL 0x0052

unsigned char UartCalculateChecksum(int msgFunction, int msgPayloadLength, unsigned char* msgPayload)
{
    //Fonction prenant entree la trame et sa longueur pour calculer le checksum
    unsigned char checksum = 0;
    
    checksum ^= 0xFE;
    checksum ^= (unsigned char) (msgFunction >> 8);
    checksum ^= (unsigned char) (msgFunction >> 0);
    checksum ^= (unsigned char) (msgPayloadLength >> 8);
    checksum ^= (unsigned char) (msgPayloadLength >> 0);

    for(int i = 0; i < msgPayloadLength; i++)
    {
        checksum ^= msgPayload[i];
    }
    return checksum;
}

void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, unsigned char* msgPayload)
{
    //Fonction d?encodage et d?envoi d?un message
    
    int length = 6 + msgPayloadLength;
    unsigned char trame[length];
    unsigned char pos=0;
    trame[pos++] = 0xFE;
    trame[pos++] = (unsigned char) (msgFunction >> 8);
    trame[pos++] = (unsigned char) (msgFunction >> 0);
    trame[pos++] = (unsigned char) (msgPayloadLength >> 8);
    trame[pos++] = (unsigned char) (msgPayloadLength >> 0);
    
    int i = 0;
    for(i=0; i < msgPayloadLength; i++)
    {
        trame[pos++] = msgPayload[i];
    }
    
    trame[pos++] = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
    
    SendMessage(trame, length);
    
}

int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;

int rcvState = WAITING;

void UartDecodeMessage(unsigned char c)
{
    //Fonction prenant en entree un octet et servant a reconstituer les trames
    switch (rcvState)
    {
        case WAITING:
            if (c == 0xFE)
                rcvState = FUNCTION_MSB;
            break;
        case FUNCTION_MSB:
            msgDecodedFunction = c << 8;
            rcvState = FUNCTION_LSB;
            break;
        case FUNCTION_LSB:
            msgDecodedFunction += c << 0;
            rcvState = LENGTH_MSB;
            break;

        case LENGTH_MSB:
            msgDecodedPayloadLength = c << 8;
            rcvState = LENGTH_LSB;
            break;
            
        case LENGTH_LSB:
            msgDecodedPayloadLength += c << 0;
            msgDecodedPayloadIndex = 0;
            if(msgDecodedPayloadLength>0)
                rcvState = PAYLOAD;
            else
                rcvState = CHECKSUM;
            break;

        case PAYLOAD:
            msgDecodedPayload[msgDecodedPayloadIndex] = c;
            msgDecodedPayloadIndex++;
            if (msgDecodedPayloadIndex >= msgDecodedPayloadLength)
                rcvState = CHECKSUM;

            break;

        case CHECKSUM: ;
            unsigned char receivedChecksum = c;

            unsigned char calculatedChecksum = UartCalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);

            if(calculatedChecksum == receivedChecksum)
                UartProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);

            rcvState = WAITING;
            break;
        default:
            rcvState = WAITING;
            break;
    }
}

void UartProcessDecodedMessage(int function, int payloadLength, unsigned char* payload)
{
    //Fonction appelee apres le decodage pour executer l?action
    //correspondant au message recu
    //...
    
    switch (function)
    {
        case SET_ROBOT_STATE:
            SetRobotState(payload[0]);
            break;
        case SET_ROBOT_MANUAL_CONTROL:
            SetRobotAutoControlState(payload[0]);
            break;
        default:
            break;
    }
}
    
void SetRobotAutoControlState(unsigned char c)
{
    if(c == 0x00 || c == 0x01)
    {
        unsigned char payload[1] = { c };
        UartEncodeAndSendMessage((int) 0x0052, (int) 0x0001, payload);
        robotState.mode = c;    
    }

    if(c == 0x01)
        stateRobot = STATE_ARRET; // Arrete de robot
}

void SetRobotState(unsigned char c)
{
    switch(c)
    {
        case STATE_TOURNE_VITE_GAUCHE:
            stateRobot = STATE_TOURNE_VITE_GAUCHE;
            break;
        case STATE_TOURNE_VITE_DROITE:
            stateRobot = STATE_TOURNE_VITE_DROITE;
            break;        
        case STATE_AVANCE:
            stateRobot = STATE_AVANCE;
            break;          
        case STATE_ARRET:
            stateRobot = STATE_ARRET;
            break; 
        case STATE_RECULE:
            stateRobot = STATE_RECULE;
            break;    
    }  
}

//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/
