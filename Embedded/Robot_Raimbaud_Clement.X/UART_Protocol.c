#include <xc.h>
#include "UART_Protocol.h"
#include "UART.h"
#include "CB_TX1.h"

#define WAITING 0
#define FUNCTION_MSB 1
#define FUNCTION_LSB 2
#define LENGTH_MSB 3
#define LENGTH_LSB 4
#define PAYLOAD 5
#define CHECKSUM 6

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
                rcvState = StateReception.FunctionMSB;
            break;
        case StateReception.FunctionMSB:
            msgDecodedFunction = c<<8;
            rcvState = StateReception.FunctionLSB;
            break;
        case StateReception.FunctionLSB:
            msgDecodedFunction += c << 0;
            rcvState = StateReception.PayloadLengthMSB;
            break;

        case StateReception.PayloadLengthMSB:
            msgDecodedPayloadLength = c << 8;
            rcvState = StateReception.PayloadLengthLSB;
            break;
        case StateReception.PayloadLengthLSB:

            msgDecodedPayloadLength += c << 0;
            msgDecodedPayload = new byte[msgDecodedPayloadLength];
            msgDecodedPayloadIndex = 0;
            if(msgDecodedPayloadLength>0)
                rcvState = StateReception.Payload;
            else
                rcvState = StateReception.CheckSum;
            break;

        case StateReception.Payload:
            msgDecodedPayload[msgDecodedPayloadIndex] = c;
            msgDecodedPayloadIndex++;
            if (msgDecodedPayloadIndex >= msgDecodedPayloadLength)
                rcvState = StateReception.CheckSum;

            break;

        case StateReception.CheckSum:
            byte receivedChecksum = c;

            byte calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);

            if(calculatedChecksum == receivedChecksum)
                ProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            else          
                TextBoxReception.Text += "Erreur de Trame: " + Encoding.UTF8.GetString(msgDecodedPayload, 0, msgDecodedPayload.Length);

            rcvState = StateReception.Waiting;
            break;
        default:
            rcvState = StateReception.Waiting;
            break;
    }
}

void UartProcessDecodedMessage(int function, int payloadLength, unsigned char* payload)
{
    //Fonction appelee apres le decodage pour executer l?action
    //correspondant au message recu
    //...
}

//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/
