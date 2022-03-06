#ifndef CORE_F_
#define CORE_F_

#define ON 1
#define OFF 0
#define FROM_POTENTIOMETER 0
#define FROM_TERMINAL 1

#define REQUEST_CODE 0x23
#define SEND_CODE 0x16
#define INTERNAL_TEMP_CODE 0xC1
#define POTENTIOMETER_TEMP_CODE 0xC2
#define USER_ACTION_CODE 0xC3
#define CONTROL_SIGNAL_CODE 0xD1
#define TR_CMD_SOURCE 0xD2
#define SYSTEM_STATE_CODE 0xD3
#define TR_SOURCE_CODE 0xD4
#define MAX_READ_ATTEMPTS 5
#define NO_DATA_FLAG -1
#define INTEGER_TYPE 'i'
#define FLOAT_TYPE 'f'
#define RESISTOR_PIN 4
#define FAN_PIN 5

int set_system_state(int uart_filestream, int system_state);
int set_reference_temperature_source(int uart_filestream, int reference_temperature_source);

#endif