#include <Arduino.h>
#include "MorseSender.h"
//#define DEBUG

//////////////////////////////////////////////////////////////////////////////////////////////////
//  28 April 2018
//  Gareth Davies
//
//  Morse Sender Class implements a way to send a single letter using an LED Pin
//  Assignment. 
//
//  The Mark-Space ratios have been adjusted from standard Morse DASH=3XDOT. Because I found this 
//  Hard to read when done in light. The Nato signalling speed is 8WPM and I think this feels about
//  That sort of speed.
//
//  Increasing the speed above the defaults seems to lead to breakdown in accuracy, not sure if
//  This is to do with the latency on the chip, Led hysterisis, code issues or something else. 
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////

    MorseSender::MorseSender(void)  //Normal constructor, defaults to Arduino Pin 13.
    {
      LEDPIN = 13;
      tempo=100;                    //This variable is exposed so users can set speed if needed.
      pinMode(LEDPIN, OUTPUT);
    }

    
    MorseSender::MorseSender(int ledpin)  //Specifies which pin to use for high/low signalling.
    {
      LEDPIN = ledpin;
      tempo=100;
      pinMode(LEDPIN, OUTPUT);
    }
    

    void MorseSender::SendString(String text) //Send a String and it will be sent letter by letter.
    {
      for (int i = 0; i < text.length(); i++) 
      {
          SendLetter(text[i]);
      }
      wordGap();  //Seems to make seperation between sentences make more sense this way.
    }

  void MorseSender::Flash (void)  //This is a wake-up to the operator to be ready for a transmission.
                                  //Three bursts of flashes.
    {

      for (int x = 0;x<3;x++)
      {
        for (int i = 0; i < 20; i++)
        { 
          digitalWrite(LEDPIN, HIGH);
          delay(25);
          digitalWrite(LEDPIN, LOW);
          delay(25);
        }
      delay(250);
      }
    }

  void MorseSender::Blip (void)  //This is a wake-up to the operator to be ready for a transmission.
                                  //Three bursts of flashes.
    {
        for (int i = 0; i < 20; i++)
        { 
          digitalWrite(LEDPIN, HIGH);
          delay(25);
          digitalWrite(LEDPIN, LOW);
          delay(25);
        }
      delay(250);
    }

    void MorseSender::StartTX (void)  //This is radio-standard V's for start of TX. 
                                      //Found flashes were more useful, but left for the hell of it.
    {
      int SaveTemp=tempo;
      tempo=tempo/3;
      for (int x = 0;x<2;x++)
      {
        for (int i = 0; i < 3; i++)
        { 
         SendLetter('V');
        }
       delay(6*tempo);
      }
      tempo=SaveTemp;
    }

    void MorseSender::dot (void)    //Sends a dot, but with an extended pause after each one. 
                                    //May not be standard, but makes it more readable to my eye.
    {
       digitalWrite(LEDPIN, HIGH);
       delay(1 * tempo);
       digitalWrite(LEDPIN, LOW);
       delay(3 * tempo);
    }
    
    void MorseSender::dash (void) //Sends a dash - which is 5x longer than the dot. Again not standard
                                  //But seems to make the morse readable.
    {
       digitalWrite(LEDPIN, HIGH);
       delay(5 * tempo);
       digitalWrite(LEDPIN, LOW);
       delay(3 * tempo);
    }

    void MorseSender::charGap (void) //To be readable needs a longer gap after each character. This is then
                                     //Double the gap between each of the symbols within each letter.
    {
       delay(3 * tempo);
    }

    void MorseSender::wordGap (void)  //A Gap at the end of each word, which is longer than between letters
    {
       delay(6 * tempo);
    }

    void MorseSender::SendLetter (byte letter)  //This is where the action happens.
    {
      
#ifdef DEBUG
      Serial.println((char)letter);
#endif
        if (letter == 'a' or letter == 'A') {dot(); dash();charGap();}
        if (letter == 'b' or letter == 'B') {dash(); dot(); dot(); dot();charGap();}
        if (letter == 'c' or letter == 'C') {dash(); dot(); dash(); dot();charGap();}
        if (letter == 'd' or letter == 'D') {dash(); dot(); dot();charGap();}
        if (letter == 'e' or letter == 'E') {dot();charGap();}
        if (letter == 'f' or letter == 'F') {dot(); dot(); dash(); dot();charGap();}
        if (letter == 'g' or letter == 'G') {dash(); dash(); dot();charGap();}
        if (letter == 'h' or letter == 'H') {dot(); dot(); dot(); dot();charGap();}
        if (letter == 'i' or letter == 'I') {dot(); dot();charGap();}
        if (letter == 'j' or letter == 'J') {dot(); dash(); dash(); dash();charGap();}
        if (letter == 'k' or letter == 'K') {dash(); dot(); dash();charGap();}
        if (letter == 'l' or letter == 'L') {dot(); dash(); dot(); dot();charGap();}
        if (letter == 'm' or letter == 'M') {dash(); dash();charGap();}
        if (letter == 'n' or letter == 'N') {dash(); dot();charGap();}
        if (letter == 'o' or letter == 'O') {dash(); dash(); dash();charGap();}
        if (letter == 'p' or letter == 'P') {dot(); dash(); dash(); dot();charGap();}
        if (letter == 'q' or letter == 'Q') {dash(); dash(); dot(); dash();charGap();}
        if (letter == 'r' or letter == 'R') {dot(); dash(); dot();charGap();}
        if (letter == 's' or letter == 'S') {dot(); dot(); dot();charGap();}
        if (letter == 't' or letter == 'T') {dash();charGap();}
        if (letter == 'u' or letter == 'U') {dot(); dot(); dash();charGap();}
        if (letter == 'v' or letter == 'V') {dot(); dot(); dot(); dash();charGap();}
        if (letter == 'w' or letter == 'W') {dot(); dash(); dash();charGap();}
        if (letter == 'x' or letter == 'X') {dash(); dot(); dot(); dash();charGap();}
        if (letter == 'y' or letter == 'Y') {dash(); dot(); dash(); dash();charGap();}
        if (letter == 'z' or letter == 'Z') {dash(); dash(); dot(); dot();charGap();}
        if (letter == '1') {dot(); dash(); dash(); dash(); dash();charGap();}
        if (letter == '2') {dot(); dot(); dash(); dash(); dash();charGap();}
        if (letter == '3') {dot(); dot(); dot(); dash(); dash();charGap();}
        if (letter == '4') {dot(); dot(); dot(); dot(); dash();charGap();}
        if (letter == '5') {dot(); dot(); dot(); dot(); dot();charGap();}
        if (letter == '6') {dash(); dot(); dot(); dot(); dot();charGap();}
        if (letter == '7') {dash(); dash(); dot(); dot(); dot();charGap();}
        if (letter == '8') {dash(); dash(); dash(); dot(); dot();charGap();}
        if (letter == '9') {dash(); dash(); dash(); dash(); dot();charGap();}
        if (letter == '0') {dash(); dash(); dash(); dash(); dash();charGap();}
        if (letter == '.') {charGap();dot(); dash(); dot(); dash(); dot();dash();charGap();charGap();}  //Extra Gap in the front, seems to make it easier to read
        if (letter == '?') {charGap();dot(); dot(); dash(); dash(); dot();dot();charGap();charGap();}   //Extra Gap in the front, seems to make it easier to read
        if (letter == ' ') {wordGap();}                        
 }

