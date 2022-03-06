#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "uart.h"
#include "modbus.h"
#include "lcd.h"
#include "bme280.h"
#include "pid.h"

#define REQUEST_CODE 0x23
#define SEND_CODE 0x16
#define INTERNAL_TEMP_CODE 0xC1
#define POTENTIOMETER_TEMP_CODE 0xC2
#define USER_ACTION_CODE 0xC3
#define SYSTEM_STATE_CODE 0xD3
#define TR_SOURCE_CODE 0xD4
#define TR_CMD_SOURCE 0xD2
#define MAX_READ_ATTEMPTS 5
#define NO_DATA_FLAG -1
#define INTEGER_TYPE 'i'
#define FLOAT_TYPE 'f'

void show_temp_and_mode_on_lcd(char *mode, float internal_temp, float reference_temp, float external_temp){
  lcd_init();
  ClrLcd();
  // linha 1
  lcdLoc(LINE1);
  typeln(mode);
  typeln("TE: ");
  typeFloat(external_temp);

  // linha 1
  lcdLoc(LINE2);
  typeln("TI:");
  typeFloat(internal_temp);
  typeln(" TR:");
  typeFloat(reference_temp);
}

int main(){
  int uart_filestream;
  int read_attempts = 0;
  float internal_temp = 0;
  float reference_temp = 0;
  float external_temp = 0;
  int user_action = -1;
  int current_sys_state = 0;
  int tr_source = 0; // 1 = teclado, 2 = potenciomentro, 3 = curva de temperatura

  // initial state of app
  // resp 2
  // pid_configura_constantes(30.0,  0.2, 400.0);
    
  // resp 3
  pid_configura_constantes(20.0,  0.1, 100.0);

  printf("Escolha a origem da temperatura de referência (TR) :\n\n1-Teclado\n2-Potenciometro\n3-Curva de temperatura\n\n");
  scanf("%d", &tr_source);
  uart_filestream = init_uart();
  send_uart_request(uart_filestream, SEND_CODE, SYSTEM_STATE_CODE, 1, 1, INTEGER_TYPE); // turn on sys
  send_uart_request(uart_filestream, SEND_CODE, TR_SOURCE_CODE, 1, 1, INTEGER_TYPE); // turn of potentiometer

  switch (tr_source)
  {
  case 1:
    printf("Digite a temperatura desejada:\n");
    scanf("%f", &reference_temp);
    send_uart_request(uart_filestream, SEND_CODE, TR_CMD_SOURCE, reference_temp, 4, FLOAT_TYPE);
    break;
  case 2:
    send_uart_request(uart_filestream, SEND_CODE, TR_SOURCE_CODE, 0, 1, INTEGER_TYPE);  
    break;
  case 3:
    send_uart_request(uart_filestream, SEND_CODE, TR_SOURCE_CODE, 1, 1, INTEGER_TYPE);  
    break;
  
  default:
    printf("\n\nEscolha invalida, finalizando...\n\n");
    break;
  }
  close_uart(uart_filestream);

  while (1)
  {

    uart_filestream = init_uart();

    // get internal temp =========================================
    printf("Lendo temperatura interna\n");
    send_uart_request(uart_filestream, REQUEST_CODE, INTERNAL_TEMP_CODE, NO_DATA_FLAG, 0, FLOAT_TYPE);
    do{
      internal_temp = read_uart_response(uart_filestream, FLOAT_TYPE);
      read_attempts++;
    }while (internal_temp == -1.0 && read_attempts>= MAX_READ_ATTEMPTS);
    read_attempts = 0;



    // get TR=========================================
    if(tr_source == 1){
      send_uart_request(uart_filestream, SEND_CODE, TR_CMD_SOURCE, reference_temp, 4, FLOAT_TYPE);
    }else if(tr_source == 2){
      send_uart_request(uart_filestream, REQUEST_CODE, POTENTIOMETER_TEMP_CODE, NO_DATA_FLAG, 0, FLOAT_TYPE);
      do{
        reference_temp = read_uart_response(uart_filestream, FLOAT_TYPE);
        read_attempts++;
      }while (reference_temp == -1.0 && read_attempts>=MAX_READ_ATTEMPTS);
      read_attempts = 0;
    }else{
      // implement later reflow
      send_uart_request(uart_filestream, SEND_CODE, TR_SOURCE_CODE, 1, 1, INTEGER_TYPE);
    }

    if(tr_source == 1 || tr_source == 3){
      read_bme_temperature(&external_temp);
      show_temp_and_mode_on_lcd("CMD  ", internal_temp, reference_temp, external_temp);
    }else{
      show_temp_and_mode_on_lcd("UART  ", internal_temp, reference_temp, 0.0);
    }
    // get user command 
    send_uart_request(uart_filestream, REQUEST_CODE, USER_ACTION_CODE, NO_DATA_FLAG, 0, FLOAT_TYPE);
    do{
      printf("tentando ler\n");
      user_action = (int)read_uart_response(uart_filestream, 'i');
      read_attempts++;
    }while (user_action == -1 && read_attempts <= MAX_READ_ATTEMPTS);
    read_attempts = 0;

    switch (user_action)
    {
    case 1:
      printf("Ligando sistema...\n\n");
      send_uart_request(uart_filestream, SEND_CODE, SYSTEM_STATE_CODE, 1, 1, INTEGER_TYPE);
      break;
    case 2:
      printf("Desligando sistema...\n\n");
      send_uart_request(uart_filestream, SEND_CODE, SYSTEM_STATE_CODE, 0, 1, INTEGER_TYPE);
      break;
    case 3:
      printf("Controle via potenciometro...\n\n");
      tr_source = 2;
      send_uart_request(uart_filestream, SEND_CODE, TR_SOURCE_CODE, 0, 1, INTEGER_TYPE);
      break;
    case 4:
      printf("Controle via curva...\n\n");
      tr_source = 3;
      send_uart_request(uart_filestream, SEND_CODE, TR_SOURCE_CODE, 1, 1, INTEGER_TYPE);
      break;
    
    default:
      break;
    }

    // pid area 

    pid_atualiza_referencia(reference_temp);
    printf("SAIDA DO PID CTRL => %lf\n", pid_controle(internal_temp));

    // ===================================
    close_uart(uart_filestream);
    sleep(1);
  }

  return 0;
}