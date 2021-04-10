//28th April 2018, Gareth Davies


#define WAIT_TIME 2000 //Used in testing PWM waveform generation

#define TARGET  14.00
#define HYSTGAP  0.50

//These define the resistor values used in the voltage sensing potentiameters, this will be scaled to 1-5V
#define SOLARPOT_HIGHSIDE 680
#define SOLARPOT_LOWSIDE  101
#define BATTPOT_HIHGSIDE  680
#define BATTPOT_LOWSIDE   230

/*
The frequency of the PWM signal on most pins is approximately 490 Hz. On the Uno and similar boards, pins 5 and 6
have a frequency of approximately 980 Hz.
On most Arduino boards (those with the ATmega168 or ATmega328P), this function works on pins 3, 5, 6, 9, 10, and 11. 
On the Arduino Mega, it works on pins 2 - 13 and 44 - 46. 
Older Arduino boards with an ATmega8 only support analogWrite() on pins 9, 10, and 11.
 */
 
//The charge pump come from timer 2 on the Arduino
#define CHARGEPUMP_PWM_A 11 //Pin generating the charge pump PWM
#define CHARGEPUMP_PWM_B 3  //Pin generating the inverse charge pump PWM
#define CHARGEWAVEFORM 5    //This is the pin generating the charge waveform   

//These are constants used to configure the algorithm
#define CHARGE_LOW  0     //Returned when max charge is required
#define CHARGE_LOW_VOLTAGE 12.5
#define CHARGE_HIGH 1  //This is the voltage at fully charged
#define CHARGE_HIGH_VOLTAGE 14.0


