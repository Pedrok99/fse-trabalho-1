#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wiringPi.h>
#include "uart.h"
#include "modbus.h"
#include "lcd.h"
#include "bme280.h"
#include "pid.h"
#include "pwm.h"
#include "csv.h"
#include "core_functions.h"

// GLOBAL VARS
int uart_filestream;

void finish_app(int value){
  uart_filestream = init_uart();
  printf("Desligando sistema...\n\n");
  send_uart_request(uart_filestream, SEND_CODE, SYSTEM_STATE_CODE, 0, 1, INTEGER_TYPE);
  send_uart_request(uart_filestream, SEND_CODE, CONTROL_SIGNAL_CODE, 0, 4, INTEGER_TYPE);  
  update_pin_value(FAN_PIN, 0);
  update_pin_value(RESISTOR_PIN, 0);

  close_uart(uart_filestream);
  exit(0);
}

int main(){
  int read_attempts = 0;
  float internal_temp = 0;
  float reference_temp = 0;
  float external_temp = 0;
  int user_action = -1;
  int current_fan_value, current_resistor_value;
  int ref_temperature_source = 0; // 1 = teclado, 2 = potenciomentro, 3 = curva de temperatura
  double pid_computed_value = 0;
  int reflow_times[10], reflow_temps[10], reflow_timer_count = 0, current_reflow_pos = -1;
  uart_filestream = init_uart();

  signal(SIGINT, finish_app);
  init_csv_file();
  // initial state of app
  
  pid_configura_constantes(30.0,  0.2, 400.0); // resp 2
  // pid_configura_constantes(20.0,  0.1, 100.0); // resp 3

  printf("Escolha a origem da temperatura de referência (TR) :\n\n1-Teclado\n2-Potenciometro\n3-Curva de temperatura\n\n");
  scanf("%d", &ref_temperature_source);
  set_system_state(uart_filestream, ON);
  set_reference_temperature_source(uart_filestream, FROM_TERMINAL);

  switch (ref_temperature_source)
  {
  case 1:
    printf("Digite a temperatura desejada:\n");
    scanf("%f", &reference_temp);
    set_reference_temperature(uart_filestream, reference_temp);
    break;
  case 2:
    set_reference_temperature_source(uart_filestream, FROM_POTENTIOMETER);
    break;
  case 3:
    set_reference_temperature_source(uart_filestream, FROM_TERMINAL);
    read_reflow_csv(reflow_times, reflow_temps, 10);
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
    // send_uart_request(uart_filestream, REQUEST_CODE, INTERNAL_TEMP_CODE, NO_DATA_FLAG, 0, FLOAT_TYPE);
    // do{
    //   internal_temp = read_uart_response(uart_filestream, FLOAT_TYPE);
    //   read_attempts++;
    // }while (internal_temp == -1.0 && read_attempts>= MAX_READ_ATTEMPTS);
    // read_attempts = 0;

    internal_temp = get_internal_temperature(uart_filestream);



    // get TR=========================================
    if(ref_temperature_source == 1){
      // send_uart_request(uart_filestream, SEND_CODE, SET_RT_CODE, reference_temp, 4, FLOAT_TYPE);
      set_reference_temperature(uart_filestream, reference_temp);
    }else if(ref_temperature_source == 2){
      send_uart_request(uart_filestream, REQUEST_CODE, POTENTIOMETER_TEMP_CODE, NO_DATA_FLAG, 0, FLOAT_TYPE);
      do{
        reference_temp = read_uart_response(uart_filestream, FLOAT_TYPE);
        read_attempts++;
      }while (reference_temp == -1.0 && read_attempts>=MAX_READ_ATTEMPTS);
      read_attempts = 0;
    }else{
      // implement later reflow
      reflow_timer_count++;
      if(current_reflow_pos != (int)(reflow_timer_count/60.0)){
        current_reflow_pos = (int)(reflow_timer_count/60.0);
        reference_temp = reflow_temps[current_reflow_pos];
        send_uart_request(uart_filestream, SEND_CODE, SET_RT_CODE, reference_temp, 4, FLOAT_TYPE);
      }
      // send_uart_request(uart_filestream, SEND_CODE, TR_SOURCE_CODE, 1, 1, INTEGER_TYPE);
      printf("Reflow timer -> %d\n", reflow_timer_count);
    }

    if(ref_temperature_source == 1 || ref_temperature_source == 3){
      read_bme_temperature(&external_temp);
      show_temp_and_mode_on_lcd("TERMINAL ", internal_temp, reference_temp, external_temp);
    }else{
      read_bme_temperature(&external_temp);
      show_temp_and_mode_on_lcd("UART  ", internal_temp, reference_temp, external_temp);
    }
    // get user command 
    send_uart_request(uart_filestream, REQUEST_CODE, USER_ACTION_CODE, NO_DATA_FLAG, 0, FLOAT_TYPE);
    do{
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
      finish_app(0);
      break;
    case 3:
      printf("Controle via potenciometro...\n\n");
      ref_temperature_source = 2;
      send_uart_request(uart_filestream, SEND_CODE, TR_SOURCE_CODE, 0, 1, INTEGER_TYPE);
      break;
    case 4:
      printf("Controle via curva...\n\n");
      ref_temperature_source = 3;
      send_uart_request(uart_filestream, SEND_CODE, TR_SOURCE_CODE, 1, 1, INTEGER_TYPE);
      read_reflow_csv(reflow_times, reflow_temps, 10);
      break;
    
    default:
      break;
    }

    // pid area 

    pid_atualiza_referencia(reference_temp);
    pid_computed_value = pid_controle(internal_temp);
    printf("SAIDA DO PID CTRL => %lf\n", pid_computed_value);

    if(pid_computed_value > 0){
      printf("Esquentando\n\n");
      update_pin_value(RESISTOR_PIN, (int)pid_computed_value);
      current_resistor_value = (int)pid_computed_value;
      update_pin_value(FAN_PIN, 0);
      current_fan_value = 0;
    }else{
      printf("Resfriando\n\n");
      if(pid_computed_value < -40){
        update_pin_value(FAN_PIN, (int)(-1.0*pid_computed_value));
        current_fan_value = (int)(-1.0*pid_computed_value);
      }else{
        update_pin_value(FAN_PIN, 40);
        current_fan_value = 40;
      }
      update_pin_value(RESISTOR_PIN, 0);
      current_resistor_value = 0;

    }
    send_uart_request(uart_filestream, SEND_CODE, CONTROL_SIGNAL_CODE, (int)pid_computed_value, 4, INTEGER_TYPE);  


    // ===================================

    // save log on csv ==================
    write_on_csv(internal_temp,reference_temp,external_temp,reference_temp, current_fan_value, current_resistor_value);

    close_uart(uart_filestream);
    sleep(1);
  }

  return 0;
}