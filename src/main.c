#include <stdio.h>
#include <stdlib.h>
#include "uart.h"
#include "modbus.h"

#define REQUEST_CODE 0x23

int main(){
  float temperature = 0;
  int tentativa = 0;
  int uart_filestream;
  uart_filestream = init_uart();
  send_uart_request(uart_filestream);
  do
  {
    tentativa++;
    printf("Tentando Recuperar a temperatura %d x =========\n\n", tentativa);
    temperature = read_uart_response(uart_filestream);
    if(tentativa > 10){
      break;
    }
  } while (temperature == -1.0);
  close_uart(uart_filestream);
  printf("Temperatura recuperada -> %f", temperature);
  return 0;
}