#include"tegra_gpio.h"
#include"stdio.h"

using namespace std;

unsigned int output_enable =57; //output_enable
unsigned int msb =161; //MSB
//---------pin 162 disabled by default-------------
unsigned int lsb3 =163; //lsb3
unsigned int lsb2 =164; //lsb2
unsigned int lsb1 =165; //lsb1
unsigned int lsb0 =166; //lsb0

/******************************************************
 *  				Init the gpio
 * export the gpio's
 * configure the gpio to input/output
 * returns true 
*******************************************************/
bool init_gpio()
{
	 // export the gpio's
	 
	gpio_export(lsb3);
	gpio_export(lsb2);
	gpio_export(lsb1);
	gpio_export(lsb0);
	gpio_export(msb);
	gpio_export(output_enable);
		 
	 //set direction of the gpio's
	gpio_set_dir(lsb3, OUTPUT_PIN); 
	gpio_set_dir(lsb2, OUTPUT_PIN); 
	gpio_set_dir(lsb1, OUTPUT_PIN); 
	gpio_set_dir(lsb0, OUTPUT_PIN); 
	gpio_set_dir(msb, OUTPUT_PIN); 
	gpio_set_dir(output_enable, OUTPUT_PIN);
	gpio_set_value(output_enable,HIGH); 
	   
	return true;
}

/******************************************************
 * 					Exit the Gpio
 *	unexport the gpio's
 *  returns true		
 * ***************************************************/
bool exit_gpio()
{
	// unexport the gpio's
	gpio_unexport(lsb3);
	gpio_unexport(lsb2);
	gpio_unexport(lsb1);
	gpio_unexport(lsb0);
	gpio_unexport(msb);
	gpio_unexport(output_enable);
	
	return true;
}

/******************************************************
 * 					Init Servo Motor
 *  returns true		
 * ***************************************************/
bool init_motor()
{							//10010
	gpio_set_value(msb, HIGH);
	gpio_set_value(lsb3, LOW);
	gpio_set_value(lsb2, LOW);
	gpio_set_value(lsb1, HIGH);
	gpio_set_value(lsb0, LOW);
	
	return true;
}

/*******************************************************
 *  				Select Motor
 * This method sets the pins of the demux such that the 
 * desired motor is selected.
 * should be used before perfoming any operation on motors
 * return true
 * ****************************************************/
bool select_motor(int motor_no, int position)
{
	if(motor_no==1)
	{
		switch(position)
		{
			case 1:							//10011
				gpio_set_value(msb, HIGH);
				gpio_set_value(lsb3, LOW);
				gpio_set_value(lsb2, LOW);
				gpio_set_value(lsb1, HIGH);
				gpio_set_value(lsb0, HIGH);		
				break;

			case 2:							//00001
				gpio_set_value(msb, LOW);
				gpio_set_value(lsb3, LOW);
				gpio_set_value(lsb2, LOW);
				gpio_set_value(lsb1, LOW);
				gpio_set_value(lsb0, HIGH);
				break;

			case 3:						  //00010
				gpio_set_value(msb, LOW);
				gpio_set_value(lsb3, LOW);
				gpio_set_value(lsb2, LOW);
				gpio_set_value(lsb1, HIGH);
				gpio_set_value(lsb0, LOW);
				break;

		}
	}

	else if(motor_no==2)
	{
		switch(position)
		{
			case 1:								//00011
				gpio_set_value(msb, LOW);
				gpio_set_value(lsb3, LOW);
				gpio_set_value(lsb2, LOW);
				gpio_set_value(lsb1, HIGH);
				gpio_set_value(lsb0, HIGH);
				break;

			case 2:								//00100
				gpio_set_value(msb, LOW);
				gpio_set_value(lsb3, LOW);
				gpio_set_value(lsb2, HIGH);
				gpio_set_value(lsb1, LOW);
				gpio_set_value(lsb0, LOW);
				break;

			case 3:								//00101
				gpio_set_value(msb, LOW);
				gpio_set_value(lsb3, LOW);
				gpio_set_value(lsb2, HIGH);
				gpio_set_value(lsb1, LOW);
				gpio_set_value(lsb0, HIGH);
				break;
		}
	}

	else if(motor_no==3)
	{
		switch(position)
		{
			case 1:						   //00110
				gpio_set_value(msb, LOW);
				gpio_set_value(lsb3, LOW);
				gpio_set_value(lsb2, HIGH);
				gpio_set_value(lsb1, HIGH);
				gpio_set_value(lsb0, LOW);
				break;

			case 2:							//00111
				gpio_set_value(msb, LOW);
				gpio_set_value(lsb3, LOW);
				gpio_set_value(lsb2, HIGH);
				gpio_set_value(lsb1, HIGH);
				gpio_set_value(lsb0, HIGH);
				break;

			case 3:							//01000
				gpio_set_value(msb, LOW);
				gpio_set_value(lsb3, HIGH);
				gpio_set_value(lsb2, LOW);
				gpio_set_value(lsb1, LOW);
				gpio_set_value(lsb0, LOW);
				break;
		}

	}

	else if(motor_no==4)
	{
		switch(position)		
		{
			case 1:							//01001
				gpio_set_value(msb, LOW);
				gpio_set_value(lsb3, HIGH);
				gpio_set_value(lsb2, LOW);
				gpio_set_value(lsb1, LOW);
				gpio_set_value(lsb0, HIGH);
				break;

			case 2:							//01010
				gpio_set_value(msb, LOW);
				gpio_set_value(lsb3, HIGH);
				gpio_set_value(lsb2, LOW);
				gpio_set_value(lsb1, HIGH);
				gpio_set_value(lsb0, LOW);
				break;

			case 3:							//01011
				gpio_set_value(msb, LOW);
				gpio_set_value(lsb3, HIGH);
				gpio_set_value(lsb2, LOW);
				gpio_set_value(lsb1, HIGH);
				gpio_set_value(lsb0, HIGH);
				break;
		}
	}

    else if(motor_no==5)
	{
		switch(position)
		{
			case 1:						//01100
				gpio_set_value(msb, LOW);
				gpio_set_value(lsb3, HIGH);
				gpio_set_value(lsb2, HIGH);
				gpio_set_value(lsb1, LOW);
				gpio_set_value(lsb0, LOW);
				break;

			case 2:						//01101
				gpio_set_value(msb, LOW);
				gpio_set_value(lsb3, HIGH);
				gpio_set_value(lsb2, HIGH);
				gpio_set_value(lsb1, LOW);
				gpio_set_value(lsb0, HIGH);
				break;

			case 3:						//01110
				gpio_set_value(msb, LOW);
				gpio_set_value(lsb3, HIGH);
				gpio_set_value(lsb2, HIGH);
				gpio_set_value(lsb1, HIGH);
				gpio_set_value(lsb0, LOW);
				break;
		}
	}

	else if(motor_no==6)
	{
		switch(position)
		{
			case 1:							//01111
				gpio_set_value(msb, LOW);
				gpio_set_value(lsb3, HIGH);
				gpio_set_value(lsb2, HIGH);
				gpio_set_value(lsb1, HIGH);
				gpio_set_value(lsb0, HIGH);
				break;

			case 2:							//10000
				gpio_set_value(msb, HIGH);
				gpio_set_value(lsb3, LOW);
				gpio_set_value(lsb2, LOW);
				gpio_set_value(lsb1, LOW);
				gpio_set_value(lsb0, LOW);
				break;

			case 3:							//10001
				gpio_set_value(msb, HIGH);
				gpio_set_value(lsb3, LOW);
				gpio_set_value(lsb2, LOW);
				gpio_set_value(lsb1, LOW);
				gpio_set_value(lsb0, HIGH);
				break;

	   }
	}		
	return 0;
}	

#if 0
int main()
{
	exit_gpio();
	init_gpio();
	select_motor(1,1);
	sleep(1);
	select_motor(1,2);
	sleep(1);
	select_motor(1,3);
	sleep(1);
	select_motor(2,1);
	sleep(1);
	select_motor(2,2);
	sleep(1);
	select_motor(2,3);
	sleep(1);
	select_motor(3,1);
	sleep(1);
	select_motor(3,2);
	sleep(1);
	select_motor(3,3);
	sleep(1);
	select_motor(4,1);
	sleep(1);
	select_motor(4,2);
	sleep(1);
	select_motor(4,3);
	sleep(1);
	select_motor(5,1);
	sleep(1);
	select_motor(5,2);
	sleep(1);
	select_motor(5,3);
	sleep(1);
	select_motor(6,1);
	sleep(1);	
	select_motor(6,2);
	sleep(1);
	select_motor(6,3);
	sleep(1);
	exit_gpio();
	return 0;

}
#endif
