#include <stdio.h>
#include <stdlib.h>
#include "crc16.h"
#include "uart.h"
#include <string.h>

void send_uart_request(int uart_filestream, unsigned char code, unsigned char sub_code){
  int message_size  = 7;

  unsigned char message[200];
  unsigned char *msg_ptr = message;

  *msg_ptr++ =0x01;
  *msg_ptr++ =code;
  *msg_ptr++ =sub_code;
  *msg_ptr++ =0x09;
  *msg_ptr++ =0x02;
  *msg_ptr++ =0x08;
  *msg_ptr++ =0x07;

  short computed_crc = calcula_CRC(message, message_size);

  memcpy(&message[message_size],  &computed_crc, 2);

  write_on_uart(uart_filestream, message, message_size+2);

}

float read_uart_response(int uart_filestream){

  short expected_crc, recieved_crc;
  int response_length;
  float temperature;

  unsigned char response[256];

  response_length = read_from_uart(uart_filestream, response, 10);
  if(response_length < 0){
    return -1.0;
  }
  memcpy(&recieved_crc, &response[7], 2);
  expected_crc = calcula_CRC(response, 7);  

  if(recieved_crc != expected_crc){
    printf("CRCs Diferem!\n");
    temperature = -1.0;
  }else if(response_length != 9){
    printf("Tamanho da mensagem incorreto!\n");
    temperature = -1.0;
  }else{
    memcpy(&temperature, &response[3], 4);
  }

  return temperature;
}