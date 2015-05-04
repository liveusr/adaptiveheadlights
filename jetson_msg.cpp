/*
 * jetson_msg.cpp
 *
 *  Created on: Mar 29, 2015
 *      Author: Sushant Potdar
 */
#include <stdio.h>
#include<utilities.h>
#include<FreeRTOS.h>
#include"jetson_msg.hpp"

// Global variables
uint32_t message_no =0;

jetson_to_sjone::jetson_to_sjone(uint8_t priority) :scheduler_task("Jetson_to_sjone",1024,priority)
{
    //configure input pins here
    LPC_GPIO0->FIODIR &= ~(1<<29);
    LPC_GPIO0->FIODIR &= ~(1<<30);
    LPC_GPIO1->FIODIR &= ~(1<<19);
    LPC_GPIO1->FIODIR &= ~(1<<20);
    LPC_GPIO1->FIODIR &= ~(1<<22);

}

bool jetson_to_sjone::run(void *p)
{
    //uint32_t all_32bits= ;
    pin0= LPC_GPIO0->FIOPIN & (1<<29);

   // all_32bits = ;
    pin1= LPC_GPIO0->FIOPIN & (1<<30);

   // all_32bits= ;
    pin2= LPC_GPIO1->FIOPIN & (1<<19);

 //   all_32bits= ;
    pin3= LPC_GPIO1->FIOPIN & (1<<20);

//    all_32bits= ;
    pin4= LPC_GPIO1->FIOPIN & (1<<22);
printf("  %x %x %x %x %x \n",pin4,pin3,pin2,pin1,pin0);


                if(pin0 != 0 )
                  temp |= (1 << 0);
                    else
                  temp &= ~(1 <<0);
                if(pin1 != 0 )
                    temp |= (1 << 1);
                else
                    temp &= ~(1 <<1);
                if(pin2 != 0)
                    temp |= (1 << 2);
                else
                    temp &= ~(1 <<2);
                if(pin3 != 0)
                    temp |= (1 << 3);
                else
                    temp &= ~(1 <<3);
                if(pin4 != 0)
                    temp |= (1 << 4);
                else
                    temp &= ~(1 <<4);

                message_no  = temp ;
              printf("message number is %u \n",message_no);
              delay_ms(500);
              return true;
}

