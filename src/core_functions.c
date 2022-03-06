#include <stdio.h>
#include <stdlib.h>

#include "core_functions.h"
#include "modbus.h"

int set_system_state(int uart_filestream, int system_state){
  send_uart_request(uart_filestream, SEND_CODE, SYSTEM_STATE_CODE, system_state, 1, INTEGER_TYPE);
  return 0; 
}
