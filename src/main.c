#include <stdio.h>
#include <stdlib.h>
#include "uart.h"
#include "modbus.h"

#define REQUEST_CODE 0x23
#define INTERNAL_TEMP_CODE 0xC1
#define POTENTIOMETER_TEMP_CODE 0xC2

int main(){
  float temperature = 0;
  int tentativa = 0;
  int uart_filestream;
  int escolha = 0;

  uart_filestream = init_uart();

  printf("1-Ti\n2-TP\n\n");
  scanf("%d", &escolha);

  switch (escolha)
  {
  case 1:
    send_uart_request(uart_filestream, REQUEST_CODE, INTERNAL_TEMP_CODE);
    break;

  case 2:
    send_uart_request(uart_filestream, REQUEST_CODE, POTENTIOMETER_TEMP_CODE);
    break;
  
  default:
    break;
  }

  
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