//#define DEBUG

#include <Arduino.h>
#include "PWMLibs.h"
#include "MorseSender.h"
MorseSender CC_Morse(13); //This is a cludge because I cannot figure out how to pass the global Morse either in a constructor or just use the global

/*  28th April 2018
 *  Gareth Davies
 *  
 *  Library implements three classes 
 *  
 *  ChargePumpPWM sets up the anti-phase PWM for the on-board charge pump, required to get high
 *  side MOSFET firing.
 *  
 *  VoltageSensor includes the logic for voltage dividers so it reports converted voltage
 *  Also makes raw A->D reading and voltage on the low-side of the POT if needed
 */


///////////////////////////////////////////////////////////////////////////////////////////////
// Class ChargePumpPWM
//////////////////////////////////////////////////////////////////////////////////////////////

  ChargePumpPWM::ChargePumpPWM (int OutputA, int OutputB)
    {
      /////////////////////////////////////////////////
      //  Set up the PWM inverting pins and frequency
      //  Based on tutorial from Julian Ilett 
      //  https://www.youtube.com/watch?v=D826h-YQun4
      /////////////////////////////////////////////////
      OutA = OutputA;
      OutB = OutputB;

      //
      //This bit of code is hardcoded to pins 11 and 3 across the timer.
      //So it  means that passing the pins as parameters doesn't make a lot of sense
      //
      
      TCCR2A = TCCR2A | 0x30; //This sets bits 4&5 to 1 which puts the time 2 
                          //Into inverting mode.
                          //Timer 2 controls pins 11 and 3. 11 Being on A, and 3 on B
      TCCR2B = TCCR2B & 0xF8 | 0x1; // Max frequency, of about 30Khz.
         
      pinMode(OutA,OUTPUT);
      pinMode(OutB,OUTPUT);  
      #ifdef DEBUG
        Serial.print("Pump Charge PWM Signal Establised on Pins");
        Serial.print(OutA);
        Serial.print(" and ");
        Serial.println(OutB);
      #endif
    };

    void ChargePumpPWM::On (void)
    {
       analogWrite(OutA,117);  //Less than (127) 50% duty cycle for non overlapping
       analogWrite(OutB,137);
        #ifdef DEBUG
          Serial.println("ON: Pump Charge PWM Signal turned on");
        #endif
    }

     void  ChargePumpPWM::Off (void)
    {
       analogWrite(OutA,0);  //Sets output low
       analogWrite(OutB,255);  //Sets output low (NB It's inverting)
       #ifdef DEBUG
          Serial.println("OFF: Pump Charge PWM Signal turned OFF");
       #endif
    }
///////////////////////////////////////////////////////////////////////////////////////////////
//END Class ChargePumpPWM
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
//
//  VoltageSensor Class
//  for sensing voltages from Arduino
//  Understands the voltage divider resistors to give a scaled output
//  Note yet implemented is a read under PWM conditions for the battery
/////////////////////////////////////////////////////////////////////////////////////////////

VoltageSensor::VoltageSensor (int Pin, int HighR, int LowR)
{
  Readpin = Pin;
  Highside = HighR;
  Lowside = LowR;

  //This determines calculates the voltage seen across the Potential Divider
  AcrossLowR =   (float)Lowside / ((float)Lowside+(float)Highside);

  LowRangeConvRatio = 1023 / 5.0;    //This is the ratio that will be needed to convert back to volts from a 0-1023 A-D scale
  FullRangeConvRatio = 1023 / ( 5.0 / AcrossLowR) ;     //This is the ratio for the full voltage

   #ifdef DEBUG
    Report();
   #endif

  
}


//////////////////////////////////////////////////////////////
//Method for reporting on the Serial port the parameters of this instance
//useful for debugging or when developing hardware
//////////////////////////////////////////////////////////////
void VoltageSensor::Report()
{
    Serial.print("\n****Voltage Sensor Initialised : Pin ");
    Serial.print(Readpin);
    Serial.print(" POT-resistors H:");
    Serial.print(Highside);
    Serial.print(" L:");
    Serial.print(Lowside);
    Serial.print(" Lowside Conversion:");
    Serial.print(LowRangeConvRatio);
    Serial.print(" FullS Conversion:");
    Serial.println(FullRangeConvRatio);
}

//////////////////////////////////////////////////////////////
//Method to take a reading and stores it in the local storage
//Keeping a local copy of the last reading is helpful for users
//of the class. Especially if reading in the presence of a PWM
//Waveform that is halted prior to taking the reading.
//Saves too many interuptions
//////////////////////////////////////////////////////////////

void VoltageSensor::takeReading (void)
{
  ADReading = analogRead(Readpin);
  LowR = ADReading / LowRangeConvRatio; 
  FullScaledReading = ADReading / FullRangeConvRatio;
  
  #ifdef DEBUG
    Serial.print("Voltage Reading Taken: Pin ");
    Serial.print(Readpin);
    Serial.print(" AD Value ");
    Serial.print(ADReading);
    Serial.print(" Full V ");
    Serial.print(FullScaledReading);
    Serial.print("V across divider ");
    Serial.print(LowR);
    Serial.println("V");
  #endif
}


float VoltageSensor::volts (void)
{
  takeReading();
  return (FullScaledReading);
}

float VoltageSensor::LowReading (void)

{
  takeReading();
  return (LowR); 
}

int VoltageSensor::ADValue (void)
{
  takeReading();
  return (ADReading);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//END Class VoltageSensor
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
//
//  ChargePWM Class
//  This is for generating the PWM waveform to control the charger.
//  It has three modes of operation, including a built in Hysterisis mode 
//  In the standard PWM generator.
//
//
//  This class can be modified to make the most rapid charge possible in HardOn mode here it
//  sets the PWM to 100% duty cycle, but maybe there is a better way. If so do it here.
//
//
/////////////////////////////////////////////////////////////////////////////////////////////

        ChargePWM::ChargePWM (int InPin)
        {
          PWMPin = InPin;
          pinMode(PWMPin,OUTPUT);
          state = 0;    //Charge is off in initial state
          analogWrite (PWMPin,0);    
        }
        
        void ChargePWM::ImplementWaveForm (int desiredState)
        {
          switch (desiredState)
          {
              case 0: // Turn off
                  PulseWidth =0;
                  analogWrite (PWMPin,PulseWidth);
                  state=0;
        
  #ifdef DEBUG
    Serial.print("Charger Mode: ");
    Serial.print(state);
    Serial.print(" Selected PWM Value ");
    Serial.println(PulseWidth);
  #endif
              break;

              case 2: // Do Hard On
                PulseWidth=255;
                analogWrite (PWMPin,PulseWidth);
                state=2;
                
   #ifdef DEBUG
    Serial.print("Charger Mode: ");
    Serial.print(state);
    Serial.print(" Selected PWM Value ");
    Serial.println(PulseWidth);
  #endif
              break;
              
              case 1: 
                 /*
                  * This is smart trickle
                  * Tune the VoltageGap divisor value to modify the dynamic range of the PWM signal.
                  * The PWM developed is proportional to the delta between TARGET and ACTUAL voltage. 
                  * this Delta  is passed as a float in the method calll.
                  * tapering to zero seems to stop before full charge is reached, so using 1.2 as a divisor means that there is 20% duty cycle
                  * at full voltage. 
                  * 
                  * The Stop and Hysterisis parts of an implementation will prevent over charging.
                  * 
                  */
                  //PulseWidth= (int) (255 * (VoltageGap/1.2) ); 
                  PulseWidth= (int) (255 * VoltageGap );
                  
                  if (PulseWidth < 0) PulseWidth=0;     //Limit the PWM bottom end
                  if (PulseWidth > 255) PulseWidth=255; //Limit the PWM top end
                  state=1;
                  analogWrite(PWMPin,PulseWidth);
                  
   #ifdef DEBUG
    Serial.print("Charger Mode: ");
    Serial.print(state);
    Serial.print(" Voltage Difference ");
    Serial.print(VoltageGap);
    Serial.print(" Selected PWM Value ");
    Serial.println(PulseWidth);
  #endif
              break;     
          }
        }

        void ChargePWM::chargeHardOn (void)
        {
          ImplementWaveForm (2);
          CC_Morse.SendString("n");
        }
        
        void ChargePWM::chargeOff (void)
        {
         ImplementWaveForm(0);
         CC_Morse.SendString("f");
        }

        void ChargePWM::chargeOff (bool reporter)
        {
         if (reporter == true )
         {
          chargeOff();
          return;
         }
          ImplementWaveForm(0);
        }
        
        void ChargePWM::chargeTrickle (float VG)
        {
          VoltageGap=VG;
          ImplementWaveForm(1);
          CC_Morse.Blip();
          CC_Morse.SendString((String)PulseWidth);
        }
        
        void ChargePWM::Suspend (void)
        {
          statestore=state;
          chargeOff(false);
        }
        
        void ChargePWM::Resume (void)
        {
          ImplementWaveForm(statestore);
        }
        
        bool ChargePWM::isTrickle(void)
        {
          if ( state == 1) return true;
          return false;
        }
        
        bool ChargePWM::isOff(void)
        {
          if (state == 0) return true;
          return false;
        }
        
        bool ChargePWM::isHardOn(void)
        {
          if (state == 2) return true;
          return false;
        }
        
///////////////////////////////////////////////////////////////////////////////////////////////
//END Class ChargePWM
//////////////////////////////////////////////////////////////////////////////////////////////

