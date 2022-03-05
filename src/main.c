#include <stdio.h>
#include <stdlib.h>
#include "uart.h"
#include "modbus.h"

#define REQUEST_CODE 0x23

int main(){
  writeWithModbus();
  read_with_modbus();
  return 0;
}