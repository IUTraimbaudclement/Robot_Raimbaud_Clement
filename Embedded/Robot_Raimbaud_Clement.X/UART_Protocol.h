#ifndef UART_PROTOCOL_H
#define	UART_PROTOCOL_H

unsigned char UartCalculateChecksum(int, int, unsigned char*);
void UartEncodeAndSendMessage(int, int, unsigned char*);
void UartProcessDecodedMessage(int, int, unsigned char*);
void UartDecodeMessage(unsigned char);
void SetRobotAutoControlState(unsigned char );
void SetRobotState(unsigned char);
void sendPIDcorrection(unsigned char*);

#endif	/* UART_PROTOCOL_H */

