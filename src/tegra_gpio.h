#ifndef TEGRAGPIO_H_
#define TEGRAGPIO_H_

/************************************************************************
 * Configuration for pins on Jetson Tegra

 * use init_gpio before using any gpio's
 * use exit_gpio after execution of the program - Close Application
 * use select_motor before performing any operation motor
 * 
 * *********************************************************************/


#include <iostream>
#include <string>
#include <unistd.h>
#include "SimpleGPIO.h"


bool init_gpio(); //use at start of the program
bool exit_gpio(); //use at the end of the program
bool select_motor(int motor_no,int position); //
bool init_motor(); //initialize all the motor




#endif /* TEGRAGPIO_H_ */
