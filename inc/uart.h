#ifndef UART_H_
#define UART_H_

int init_uart();
int write_on_uart(int uart_filestream, unsigned char *message, int messageSize);
int read_from_uart(int uart_filestream, unsigned char* message, int messageSize);
void close_uart(int uart_filestream);

#endif