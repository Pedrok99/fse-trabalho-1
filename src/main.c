#include <stdio.h>
#include <stdlib.h>
#include "uart.h"

int main(){

  initUartCfg();
  writeOnUart(7);
  closeUart();
  return 0;
}