#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "CB_TX1.h"
#define CBTX1_BUFFER_SIZE 128

int cbTx1Queue = 0;
unsigned char cbTx1Buffer[CBTX1_BUFFER_SIZE];
unsigned char isTransmitting = 0;

void SendMessage(unsigned char* message, int length)
{
    unsigned char i=0;
    if(CB_TX1_GetRemainingSize() > length)
    {
        //On peut écrire le message
        for(i=0;i<length;i++)
            CB_TX1_Add(message[i]);
        if(!CB_TX1_IsTranmitting())
            SendOne();
    }
}

void CB_TX1_Add(unsigned char value)
{
    cbTx1Buffer[CB_TX1_GetDataSize()] += value;
    cbTx1Queue++; // ajoute 1 caractère à la queue
    
}
unsigned char CB_TX1_Get(void)
{
    return cbTx1Buffer[0]; // renvoie le prochain caractère de la queue
}

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0; // clear TX interrupt flag
    if(cbTx1Queue > 0) // Si la queue n'est pas vide, on envoie un caractère sur TX
        SendOne();
    else
        isTransmitting = 0;
}
void SendOne()
{
    isTransmitting = 1;
    unsigned char value = CB_TX1_Get();
    U1TXREG = value; // Transmit one character
    
    // Décale la queue de -1 pour la resynchroniser
    for(int i = 1; i < cbTx1Queue; i++)
        cbTx1Buffer[i - 1] = cbTx1Buffer[i];
    cbTx1Buffer[cbTx1Queue] = 0; // supprimer le dernier élément
    cbTx1Queue--; // un élément envoyé, alors la queue a un caractère en moin
    
}

int CB_TX1_IsTranmitting(void)
{
    return isTransmitting;
}

int CB_TX1_GetDataSize(void)
{
    //return size of data stored in circular buffer
    return cbTx1Queue;
}

int CB_TX1_GetRemainingSize(void)
{
    //return size of remaining size in circular buffer
    return (CBTX1_BUFFER_SIZE - cbTx1Queue);
}

