#ifndef MDB_H_
#define MBD_H_

void send_uart_request(int uart_filestream, unsigned char code, unsigned char sub_code, int message_data);
float read_uart_response(int uart_filestream, char expected_data_type);
void send_uart_request_w_byte(int uart_filestream, unsigned char code, unsigned char sub_code, unsigned char byte);

#endif