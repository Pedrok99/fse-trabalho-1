#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>

void update_pin_value(int PWM_pin, int value){
    int status = -1;
    do{
      status = wiringPiSetup();
    }while(status == -1);
    pinMode(PWM_pin, OUTPUT);	/* set GPIO as output */
	softPwmCreate(PWM_pin,1,100);	/* set PWM channel along with range*/
    softPwmWrite (PWM_pin, value); /* change the value of PWM */
    delay (10);
}