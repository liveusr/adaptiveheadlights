/*
 * header_movement.hpp
 *
 *  Created on: Mar 29, 2015
 *      Author: Sushant Potdar
 */

#ifndef L5_APPLICATION_MOTOR_MOVEMENT_HPP_
#define L5_APPLICATION_MOTOR_MOVEMENT_HPP_

#include "scheduler_task.hpp"


class motor_movement : public scheduler_task
{
    public:
        motor_movement(uint8_t priority);
        bool run(void *p);

};



#endif /* L5_APPLICATION_MOTOR_MOVEMENT_HPP_ */
