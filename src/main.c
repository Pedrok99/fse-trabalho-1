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
  ClrLcd();
  lcdLoc(LINE1);
  typeln("Finalizado ;)");
  close_uart(uart_filestream);
  printf("Sistema desligado...\n\n");
  exit(0);
}

void write_lcd_with_mode(int mode, float internal_temp, float reference_temp, float external_temp){

  switch (mode)
  {
  case 1:
    show_temp_and_mode_on_lcd("TERMINAL ", internal_temp, reference_temp, external_temp);
    break;

  case 2:
    show_temp_and_mode_on_lcd("POTEN ", internal_temp, reference_temp, external_temp);
    break;

  case 3:
    show_temp_and_mode_on_lcd("REFLOW ", internal_temp, reference_temp, external_temp);
    break;
  
  default:
    show_temp_and_mode_on_lcd("ERR-MODE ", internal_temp, reference_temp, external_temp);
    break;
  }
  
}

int main(){
  float internal_temp = 0, last_internal_temp = 0;
  float reference_temp = 0, last_reference_temp = 0;
  float external_temp = 0, last_external_temp = 0;
  int user_action = -1;
  int current_fan_value, current_resistor_value;
  int ref_temperature_source = 0; // 1 = teclado, 2 = potenciomentro, 3 = curva de temperatura
  double pid_computed_value = 0;
  int rasp_number = 0;
  int reflow_times[10], reflow_temps[10], /*reflow_timer_count = 0,*/ current_reflow_pos = -1;
  float reflow_timer_count = 0.0;
  float kp, ki, kd;
  // ===================================================main================================================================

  uart_filestream = init_uart();
  init_csv_file();
  signal(SIGINT, finish_app); // ctrl-c handler

  printf("Escolha a origem da temperatura de referência (TR) :\n\n1-Teclado\n2-Potenciometro\n3-Curva de temperatura\n\n");
  scanf("%d", &ref_temperature_source);

  switch (ref_temperature_source)
  {
  case 1:
    printf("Digite a temperatura desejada:\n");
    scanf("%f", &reference_temp);
    set_reference_temperature(uart_filestream, reference_temp);
    set_reference_temperature_source(uart_filestream, FROM_TERMINAL);
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
 
  printf("\n\n Escolha as constantes a serem usadas:\n\n1-rasp42 (Kp = 30.0 , Ki = 0.2, Kd = 400.0)\n2-rasp43 (Kp = 20.0, Ki = 0.1, Kd = 100.0)\n3-Personalizadas\n\n");
  scanf("%d", &rasp_number);

  if(rasp_number == 1){
    pid_configura_constantes(30.0,  0.2, 400.0); // resp 2
  }else if(rasp_number == 2){
    pid_configura_constantes(20.0,  0.1, 100.0); // resp 3
  }else{
    printf("Insira Kp:\n");
    scanf("%f", &kp);
    printf("Insira Ki:\n");
    scanf("%f", &ki);
    printf("Insira Kd:\n");
    scanf("%f", &kd);
    pid_configura_constantes(kp, ki, kd); // personalizado
  }

  set_system_state(uart_filestream, ON);
 
  close_uart(uart_filestream);

  // ================================ program loop ========================================

  while (1)
  {
    uart_filestream = init_uart();
    internal_temp = get_internal_temperature(uart_filestream);
    internal_temp == -1.0 ? (internal_temp = last_internal_temp) : (last_internal_temp = internal_temp);

    // get TR
    switch (ref_temperature_source)
    {
    case 1:
      set_reference_temperature(uart_filestream, reference_temp);
      break;

    case 2:
      reference_temp = get_reference_temperature(uart_filestream);
      reference_temp == -1.0 ? (reference_temp = last_reference_temp) : (last_reference_temp = reference_temp);
      break;
    
    default: // reflow
      reflow_timer_count+=1.3;
      // reflow_timer_count+=10;
      // reflow_timer_count = reflow_timer_count%600.0;
      
      if(current_reflow_pos != (int)(reflow_timer_count/60)){
        current_reflow_pos = (int)(reflow_timer_count/60);
        reference_temp = reflow_temps[current_reflow_pos];
        set_reference_temperature(uart_filestream, reference_temp);
      }
      break;
    }

    read_bme_temperature(&external_temp);
    external_temp == -1.0 ? (external_temp = last_external_temp) : (last_external_temp = external_temp);

    write_lcd_with_mode(ref_temperature_source, internal_temp, reference_temp, external_temp);

    user_action = get_user_command(uart_filestream);

    switch (user_action)
    {
    case 1:
      printf("Ligando sistema...\n\n");
      send_uart_request(uart_filestream, SEND_CODE, SYSTEM_STATE_CODE, 1, 1, INTEGER_TYPE);
      set_system_state(uart_filestream, ON);
      break;
    case 2:
      finish_app(0);
      break;
    case 3:
      printf("Controle via potenciometro...\n\n");
      ref_temperature_source = 2;
      set_reference_temperature_source(uart_filestream, FROM_POTENTIOMETER);
      break;
    case 4:
      printf("Controle via curva...\n\n");
      ref_temperature_source = 3;
      set_reference_temperature_source(uart_filestream, FROM_TERMINAL);
      read_reflow_csv(reflow_times, reflow_temps, 10);
      reflow_timer_count = 0;
      break;
    
    default:
      break;
    }

    // pid calc 
    pid_atualiza_referencia(reference_temp);
    pid_computed_value = pid_controle(internal_temp);
    if(pid_computed_value > 0){
      update_pin_value(RESISTOR_PIN, (int)pid_computed_value);
      current_resistor_value = (int)pid_computed_value;
      update_pin_value(FAN_PIN, 0);
      current_fan_value = 0;
    }else{
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

    write_on_csv(internal_temp,reference_temp,external_temp,reference_temp, current_fan_value, current_resistor_value, pid_computed_value);
    // debug
    printf("Modo atual: %d | TI: %.2f | TR: %.2f | TE: %.2f | PID: %.2f\n\n", ref_temperature_source, internal_temp, reference_temp, external_temp, pid_computed_value); // debug

    close_uart(uart_filestream);

    sleep(1);
  }

  return 0;
}