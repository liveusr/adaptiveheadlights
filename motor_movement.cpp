/*
 * motor_movement.cpp
 *
 *  Created on: Mar 29, 2015
 *      Author: Sushant Potdar
 */

#include"motor_movement.hpp"
#include <stdio.h>
#include<utilities.h>
#include<FreeRTOS.h>
#include"jetson_msg.hpp"
#include "lpc_pwm.hpp"

#define low 10.0
#define medium 7.5
#define high 5.0
#define left 10.0
#define center 7.5
#define right  5.0

extern uint32_t message_no;

    PWM pwm1(PWM::pwm1,50 );
    PWM pwm2(PWM::pwm2,50 );
    PWM pwm3(PWM::pwm3,50 );
    PWM pwm4(PWM::pwm4,50 );
    PWM pwm5(PWM::pwm5,50 );
    PWM pwm6(PWM::pwm6,50 );

motor_movement::motor_movement(uint8_t priority) : scheduler_task("motor_movement",1024,priority)
{
    // nothing to init
    pwm1.set(center);
    pwm2.set(center);
    pwm3.set(center);
    pwm4.set(center);
    pwm5.set(center);
    pwm6.set(center);

}

bool motor_movement:: run(void *p)
{
    switch(message_no)
                {
                    case 0:
                         // motor 1 left
                        pwm1.set(left);
                        printf("motor 1 left \n");
                        break;

                    case 1:
                        // motor 1 center
                        pwm1.set(center);
                        printf("motor 1 center \n");
                        break;

                    case 2:
                        // motor 1 right
                        pwm1.set(right);
                        printf("motor 1 right \n");
                        break;
                 //------------------------------------
                    case 3:
                         // motor 2 low
                        pwm2.set(low);
                        printf("motor 2 low \n");
                        break;

                    case 4:
                        // motor 2 med
                        pwm2.set(medium);
                        printf("motor 2 medium \n");
                        break;

                    case 5:
                        // motor 2 high
                        pwm2.set(high);
                        printf("motor 2 high \n");
                        break;
                //-------------------------------------
                    case 6:
                         // motor 3 low
                        pwm3.set(low);
                        printf("motor 3 low \n");
                        break;

                    case 7:
                        // motor 3 medium
                        pwm3.set(medium);
                        printf("motor 3 medium \n");
                        break;

                    case 8:
                        // motor 3 high
                        pwm3.set(high);
                        printf("motor 3 high \n");
                        break;
               //-------------------------------------
                    case 9:
                         // motor 4 low
                        pwm4.set(low);
                        printf("motor 4 low \n");
                        break;

                    case 10:
                        // motor 4 med
                        pwm4.set(medium);
                        printf("motor 4 medium \n");
                        break;

                    case 11:
                        // motor 4 high
                        pwm4.set(high);
                        printf("motor 4 high \n");
                        break;
             //------------------------------------------
                    case 12:
                         // motor 5 low
                        pwm5.set(low);
                        printf("motor 5 low \n");
                        break;

                    case 13:
                        // motor 5 med
                        pwm5.set(medium);
                        printf("motor 5 medium \n");
                        break;

                    case 14:
                        // motor 5 high
                        pwm5.set(high);
                        printf("motor 5 high \n");
                        break;
            //--------------------------------------------
                    case 15:
                         // motor 6 left
                        pwm6.set(left);
                        printf("motor 6 left \n");
                        break;

                    case 16:
                        // motor 6 center
                        pwm6.set(center);
                        printf("motor 6 center \n");
                        break;

                    case 17:
                        // motor 6 right
                        pwm6.set(right);
                        printf("motor 6 right \n");
                        break;

                    case 26:
                        printf("I am in switch case \n");
                        break;

                    default:
                        //--go to default state
                        break;

                }

    delay_ms(1000);
    return true;
}
