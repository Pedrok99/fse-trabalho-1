#ifndef UART_H_
#define UART_H_

int initUartCfg();
int writeOnUart(int messageSize);
void readFromUart(unsigned char* message, int messageSize);
void closeUart();

#endif