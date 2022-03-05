#include <stdio.h>
#include <stdlib.h>
#include "crc16.h"
#include "uart.h"
#include <string.h>

void writeWithModbus(){
  int message_size  = 7;

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

  short computed_crc = calcula_CRC(message, message_size);

  memcpy(&message[message_size],  &computed_crc, 2);

  writeOnUart(message, message_size+2);

  closeUart();
}