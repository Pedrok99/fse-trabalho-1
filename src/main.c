#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "uart.h"
#include "modbus.h"
#include "lcd.h"

#define REQUEST_CODE 0x23
#define SEND_CODE 0x16
#define INTERNAL_TEMP_CODE 0xC1
#define POTENTIOMETER_TEMP_CODE 0xC2
#define USER_ACTION_CODE 0xC3
#define SYSTEM_STATE_CODE 0xD3
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
  // temps
  float internal_temp = 0;
  float reference_temp = 0;
  int user_action = -1;
  int current_sys_state = 0;
  while (1)
  {

    uart_filestream = init_uart();

    // get internal temp =========================================
    printf("Lendo temperatura interna\n");
    send_uart_request(uart_filestream, REQUEST_CODE, INTERNAL_TEMP_CODE, NO_DATA_FLAG, 0);
    do{
      internal_temp = read_uart_response(uart_filestream, FLOAT_TYPE);
      read_attempts++;
    }while (internal_temp == -1.0 && read_attempts>= MAX_READ_ATTEMPTS);
    read_attempts = 0;



    // get temp from potentiometer =========================================
    printf("Lendo temperatura de referencia\n");
    send_uart_request(uart_filestream, REQUEST_CODE, POTENTIOMETER_TEMP_CODE, NO_DATA_FLAG, 0);
    do{
      reference_temp = read_uart_response(uart_filestream, FLOAT_TYPE);
      read_attempts++;
    }while (reference_temp == -1.0 && read_attempts>=MAX_READ_ATTEMPTS);
    read_attempts = 0;
    show_temp_and_mode_on_lcd("TERM  ", internal_temp, reference_temp, 0.0);
    printf("Lendo comandos do usuario\n");
    send_uart_request(uart_filestream, REQUEST_CODE, USER_ACTION_CODE, NO_DATA_FLAG, 0);
    do{
      printf("tentando ler\n");
      user_action = (int)read_uart_response(uart_filestream, 'i');
      read_attempts++;
    }while (user_action == -1 && read_attempts <= MAX_READ_ATTEMPTS);
    read_attempts = 0;
    printf("AÇÃO USUARIO -> \n\n%d\n\n", user_action);

    switch (user_action)
    {
    case 1:
      printf("Ligando sistema...\n\n");
      send_uart_request(uart_filestream, SEND_CODE, SYSTEM_STATE_CODE, 1, 1);
      break;
    case 2:
      printf("Desligando sistema...\n\n");
      send_uart_request(uart_filestream, SEND_CODE, SYSTEM_STATE_CODE, 0, 1);
      break;
    
    default:
      break;
    }

    close_uart(uart_filestream);
    sleep(1);
  }

  return 0;
}