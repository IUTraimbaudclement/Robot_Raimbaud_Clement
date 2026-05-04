#ifndef UART_PROTOCOL_H
#define	UART_PROTOCOL_H

#define SET_ROBOT_STATE 0x0051
#define SET_ROBOT_MANUAL_CONTROL 0x0052
#define PID_DATA 0x0070
#define ASSERV 0x0071

unsigned char UartCalculateChecksum(int, int, unsigned char*);
void UartEncodeAndSendMessage(int, int, unsigned char*);
void UartProcessDecodedMessage(int, int, unsigned char*);
void UartDecodeMessage(unsigned char);
void SetRobotAutoControlState(unsigned char );
void SetRobotState(unsigned char);
void getPidData(unsigned char*);
void getConsigne(unsigned char* payload);

#endif	/* UART_PROTOCOL_H */

