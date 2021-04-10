//////////////////////////////////////////////////////////////////////////////////////////////////
//
//  28th April 2018, Gareth Davies
//
//  Charge pump class handles the initialisation of the charge pump
//  Sets the registers etc.
//  Based on tutorial from Julian Ilett 
//  https://www.youtube.com/watch?v=D826h-YQun4
//
//////////////////////////////////////////////////////////////////////////////////////////////////

class ChargePumpPWM {

  public:
        ChargePumpPWM (int, int);
        void On (void);
        void Off (void);
  private:
        int OutA;
        int OutB;
};

///////////////////////////////////////////////////////////////////////////////////////////////
//END Class ChargePumpPWM
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Class for Charging PWM waveform control. This implements the various waveforms based on 
//  Requirements for PWM function of the charge waveform.
//  The theory of Lead-Acid charging is from here: 
//  http://batteryuniversity.com/learn/article/charging_the_lead_acid_battery
//
/////////////////////////////////////////////////////////////////////////////////////////////

class ChargePWM {
  private:
        int state;  //This provides a record of it's current charge configuration
        int statestore;
        int PWMPin;
        int PulseWidth;
        float VoltageGap;      
        void ImplementWaveForm (int desiredState);

  public:
        ChargePWM (int);
        void chargeHardOn (void);
        void chargeOff (void);
        void chargeOff (bool);
        void chargeTrickle (float);
        void Suspend (void);
        void Resume (void);
        bool isTrickle(void);
        bool isOff(void);
        bool isHardOn(void);
};

///////////////////////////////////////////////////////////////////////////////////////////////
//END Class ChargePWM
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Class for sensing voltages from Arduino
//  Understands the voltage divider resistors to give a scaled output
//  Note yet implemented is a read under PWM conditions for the battery
/////////////////////////////////////////////////////////////////////////////////////////////


class VoltageSensor {

   private:
          int Highside;
          int Lowside;
          int Readpin;
          float FullScale;
          float AcrossLowR;
          float LowRangeConvRatio;
          float FullRangeConvRatio;
          float LowR;
          float FullScaledReading;
          int ADReading;

          void takeReading(void);
   public:
          float volts (void);
          float LowReading (void);
          int ADValue (void);
          VoltageSensor (int ,int , int );
          void Report();
};

