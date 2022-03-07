#include <stdio.h>
#include <stdlib.h>

#include "core_functions.h"
#include "modbus.h"

// setters

int set_system_state(int uart_filestream, int system_state){
  int response = -1;
  int read_attempts = 0;
  send_uart_request(uart_filestream, SEND_CODE, SYSTEM_STATE_CODE, system_state, 1, INTEGER_TYPE);
  do{
      response = read_uart_response(uart_filestream, INTEGER_TYPE);
      read_attempts++;
  }while (response == -1.0 && read_attempts>= MAX_READ_ATTEMPTS);
  return response; 
}

int set_reference_temperature_source(int uart_filestream, int reference_temperature_source){
  int response = -1;
  int read_attempts = 0;
  send_uart_request(uart_filestream, SEND_CODE, TR_SOURCE_CODE, reference_temperature_source, 1, INTEGER_TYPE);
  do{
      response = read_uart_response(uart_filestream, INTEGER_TYPE);
      read_attempts++;
  }while (response == -1.0 && read_attempts>= MAX_READ_ATTEMPTS);
  return response; 
}

void set_reference_temperature(int uart_filestream, int reference_temperature){
  send_uart_request(uart_filestream, SEND_CODE, SET_RT_CODE, reference_temperature, 4, FLOAT_TYPE);
}

// getters

float get_internal_temperature(int uart_filestream){
  float temperature = 0.0;
  int read_attempts = 0;
  send_uart_request(uart_filestream, REQUEST_CODE, INTERNAL_TEMP_CODE, NO_DATA_FLAG, 0, FLOAT_TYPE);
  do{
    temperature = read_uart_response(uart_filestream, FLOAT_TYPE);
    read_attempts++;
  }while (temperature == -1.0 && read_attempts>= MAX_READ_ATTEMPTS);
  read_attempts = 0;
  return temperature;
}