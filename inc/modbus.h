#ifndef MDB_H_
#define MBD_H_

void send_uart_request(int uart_filestream);
float read_uart_response(int uart_filestream);

#endif