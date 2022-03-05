#include <stdio.h>
#include <stdlib.h>
#include "crc16.h"
#include "uart.h"

void writeWithModbus(){
  initUartCfg();

  unsigned char message[200];
  unsigned char *msg_ptr = &message;

  *msg_ptr++ =0x01;
  *msg_ptr++ =0x23;
  *msg_ptr++ =0xC1;// code
  *msg_ptr++ =0x01;
  *msg_ptr++ =0x02;
  *msg_ptr++ =0x03;
  *msg_ptr++ =0x04;

  writeOnUart(message, 7);

  closeUart();
}