#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "CB_RX1.h"
#define CBRX1_BUFFER_SIZE 128

int cbRx1Queue = 0;
unsigned char cbRx1Buffer[CBRX1_BUFFER_SIZE];

void CB_RX1_Add(unsigned char value)
{
    if(CB_RX1_GetRemainingSize()>0)
    {
        cbRx1Buffer[cbRx1Queue] = value;
        cbRx1Queue++;
    }
}

unsigned char CB_RX1_Get(void)
{
    unsigned char value = cbRx1Buffer[cbRx1Queue];
    
        // Décale la queue de -1 pour la resynchroniser
    for(int i = 1; i < cbRx1Queue; i++)
        cbRx1Buffer[i - 1] = cbRx1Buffer[i];
    //cbTx1Buffer[cbTx1Queue] = 0; // supprimer le dernier élément
    cbRx1Queue--; // un élément envoyé, alors la queue a un caractère en moin
    
    return value;
}

unsigned char CB_RX1_IsDataAvailable(void)
{
    if(cbRx1Queue > 0)
    return 1;
    else
    return 0;
}


void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) {
    IFS0bits.U1RXIF = 0; // clear RX interrupt flag
    /* check for receive errors */
    if (U1STAbits.FERR == 1) {
        U1STAbits.FERR = 0;
    }
    /* must clear the overrun error to keep uart receiving */
    if (U1STAbits.OERR == 1) {
        U1STAbits.OERR = 0;
    }
    /* get the data */
    while(U1STAbits.URXDA == 1) {
        CB_RX1_Add(U1RXREG);
    }
}

int CB_RX1_GetDataSize(void)
{
    //return size of data stored in circular buffer
    return cbRx1Queue;
}

int CB_RX1_GetRemainingSize(void)
{
    //return size of remaining size in circular buffer
    return CBRX1_BUFFER_SIZE - cbRx1Queue;
}

