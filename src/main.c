#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "uart.h"
#include "modbus.h"
#include "lcd.h"

#define REQUEST_CODE 0x23
#define INTERNAL_TEMP_CODE 0xC1
#define POTENTIOMETER_TEMP_CODE 0xC2
#define MAX_READ_ATTEMPTS 5


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
  while (1)
  {

  uart_filestream = init_uart();

    // get internal temp =========================================
    printf("Lendo temperatura interna\n");
    send_uart_request(uart_filestream, REQUEST_CODE, INTERNAL_TEMP_CODE);
    do{
      internal_temp = read_uart_response(uart_filestream);
      read_attempts++;
    }while (internal_temp == -1.0 && read_attempts>= MAX_READ_ATTEMPTS);
    read_attempts = 0;



    // get temp from potentiometer =========================================
    printf("Lendo temperatura de referencia\n");
    send_uart_request(uart_filestream, REQUEST_CODE, POTENTIOMETER_TEMP_CODE);
    do{
      reference_temp = read_uart_response(uart_filestream);
      read_attempts++;
    }while (reference_temp == -1.0 && read_attempts>=MAX_READ_ATTEMPTS);
    read_attempts = 0;
    show_temp_and_mode_on_lcd("TERM  ", internal_temp, reference_temp, 0.0);
    sleep(1);
  close_uart(uart_filestream);
  }

  return 0;
}