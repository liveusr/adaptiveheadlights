/*
 * jetson_msg.hpp
 *
 *  Created on: Mar 29, 2015
 *      Author: Sushant Potdar
 */

#ifndef L5_APPLICATION_JETSON_MSG_HPP_
#define L5_APPLICATION_JETSON_MSG_HPP_

#include "scheduler_task.hpp"

extern uint32_t message_no;

class jetson_to_sjone : public scheduler_task
{
    public:
        jetson_to_sjone(uint8_t priority);
        bool run(void *p);
        uint32_t pin0=0;
         uint32_t pin1=1;
         uint32_t pin2=0;
         uint32_t pin3=1;
         uint32_t pin4=0;
         uint32_t temp=0;

};




#endif /* L5_APPLICATION_JETSON_MSG_HPP_ */
