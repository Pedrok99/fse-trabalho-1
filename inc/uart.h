#ifndef UART_H_
#define UART_H_

int initUartCfg();
int writeOnUart(unsigned char *message, int messageSize);
int readFromUart(unsigned char* message, int messageSize);
void closeUart();

#endif