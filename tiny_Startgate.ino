/*
Tiny Start Gate - Uses an ATTiny85 to control a NeoPixel strip (1m or 5m selectable by jumper or switch)
and a piezo buzzer to start 'races'. The total time is slightly randomized (4.5 - 5.5 seconds) to prevent
racers from predicting the GO signal and "jumping the gun".

Copyright (C) 2015, Forrest Woodward

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <Adafruit_NeoPixel.h> //Can be download from https://github.com/adafruit/Adafruit_NeoPixel

#define BASETIME 4500   //Minimum time in miliseconds for countdown
#define RANDOMTIME 1000 //Random time in miliseconds added to BASETIME for countdown

#define LEDPIN 0        //Connects to the data in pin on the NeoPixel strip
#define BUZZER 1        //Connects to a Piezo Buzzer for audio signalling of racers
#define ACTIVATORPIN 2  //Connects to an IR Photoresistor
#define LENGTHPIN 3     //Connect to a jumper or switch to select a 60 or 90 pixel strip (1 meter at 60 pixels/meter or 3 meters at 30 pixels/meter)

#define LEDTESTDELAY 4  //Microseconds to delay after each pixel is drawn during the startup test. Larger values for longer startup test

#define LEADUPBUZZ 64   //PWM value for the four beeps leading up to "GO". Higher values are higher pitch and louder
#define FINALBUZZ 255   //PWM value for the final "GO" beep. Higher values are higher pitch and louder

uint16_t SLength = 0;   //Used for the total number of pixels to update in the program. 

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(90, LEDPIN, NEO_GRB + NEO_KHZ800); //90 pixels is the most that an ATTiny85 can control with this code due to its small amount of RAM (512 bytes)

uint32_t red;
uint32_t yellow;
uint32_t green;
uint32_t black;

int timeDelay=0; //Used in loop() to determine the total time from button press to "GO" signal

void setup() {
  //Set the pin modes of each of the pins we will be using
  pinMode(LEDPIN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(ACTIVATORPIN, INPUT);
  pinMode(LENGTHPIN, INPUT);

  pixels.begin(); //Tell the NeoPixel library that we are ready to start using it

  if (digitalRead(LENGTHPIN) == LOW) { //Determine the number of pixels we will actually be using based on a jumper
    SLength = 90;                      //If there is no jumper then we have 90 LEDs (3 meters of 30 pixels/meter)
  }
  else {
    SLength = 60;                      //If there is a jumper then we have 60 LEDs (1 meter of 60 pixels/meter)
  }

  //Defines colors
  //Colors are a stored as a unit32_t variable
  //pixels.Color(Red value, Green value, Blue value)
  red = pixels.Color(255, 0, 0);
  yellow = pixels.Color(128, 128, 0);
  green = pixels.Color(0, 255, 0);
  black = pixels.Color(0, 0, 0);

  // -------------
  // Startup Tests
  // -------------

  //Test buzzer by buzzing for 1/20th of a second
  analogWrite(1, LEADUPBUZZ);
  delay(50);
  analogWrite(1, 0);

  //Test LEDs by turning each pixel on in sequence
  for (int i = 0; i < SLength; i++) {   //Go through each pixel in the strip 1 at a time
    pixels.setPixelColor(i, red);       //Set the pixel to red
    pixels.show();                      //Write all pixels to the strip
    delay(LEDTESTDELAY);                //Wait a few miliseconds
  }                                     //Do the same for the rest of the pixels in the strip
  
  //Do the same sequence for each other color and then turn them all off one at a time
  for (int i = 0; i < SLength; i++) {
    pixels.setPixelColor(i, yellow);
    pixels.show();
    delay(LEDTESTDELAY);
  }
  for (int i = 0; i < SLength; i++) {
    pixels.setPixelColor(i, green);
    pixels.show();
    delay(LEDTESTDELAY);
  }
  for (int i = 0; i < SLength; i++) {
    pixels.setPixelColor(i, black);
    pixels.show();
    delay(LEDTESTDELAY);
  }
}

void loop() {
  bool Activated = digitalRead(ACTIVATORPIN);   // Read the pin connected to the IR receiver
  while (Activated == HIGH) {                   //--]
    Activated = digitalRead(ACTIVATORPIN);      //  |--If the IR receiver gets a signal continue down loop()
    delay (5);                                  //  |--If not wait 5 miliseconds and then check again
  }                                             //--]

  timeDelay = BASETIME + random(0, RANDOMTIME); //Randomizes the total time it takes from the begginning of the countdown to "GO"
  delay(500);                                   //Wait 1/2 second after button press to begin countdown

  for (uint16_t i = 0; i < SLength; i++) {      //--]
    pixels.setPixelColor(i, red);               //  |-- Set all pixels to red
  }                                             //--]
  pixels.show();                                //Write pixel data to strip
  analogWrite(1, LEADUPBUZZ);                   //Turn on buzzer
  delay(timeDelay / 10);                        //What 1/10 of total time
  analogWrite(1, 0);                            //Turn off buzzer
  for (uint16_t i = 0; i < SLength; i++) {      //--]
    pixels.setPixelColor(i, black);             //  |-- Set all pixels to black
  }                                             //--]
  pixels.show();                                //Write pixel data to strip
  delay(timeDelay / 10);                        //Wait 1/10 of total time

  //Repeat above sequence for the next 3 countdown flashes

  for (uint16_t i = 0; i < SLength; i++) {
    pixels.setPixelColor(i, red);
  }
  pixels.show();
  //tone(BUZZER, 300, 300);
  analogWrite(1, LEADUPBUZZ);
  delay(timeDelay / 10);
  analogWrite(1, 0);
  for (uint16_t i = 0; i < SLength; i++) {
    pixels.setPixelColor(i, black);
  }
  pixels.show();  delay(timeDelay / 10);

  for (uint16_t i = 0; i < SLength; i++) {
    pixels.setPixelColor(i, yellow);
  }
  pixels.show();
  //tone(BUZZER, 300, 300);
  analogWrite(1, LEADUPBUZZ);
  delay(timeDelay / 10);
  analogWrite(1, 0);
  for (uint16_t i = 0; i < SLength; i++) {
    pixels.setPixelColor(i, black);
  }
  pixels.show();
  delay(timeDelay / 10);

  for (uint16_t i = 0; i < SLength; i++) {
    pixels.setPixelColor(i, yellow);
  }
  pixels.show();
  //tone(BUZZER, 300, 300);
  analogWrite(1, LEADUPBUZZ);
  delay(timeDelay / 10);
  analogWrite(1, 0);
  for (uint16_t i = 0; i < SLength; i++) {
    pixels.setPixelColor(i, black);
  }
  pixels.show();
  delay(timeDelay / 10);


  //Countdown has ended. Signal "GO"
  for (uint16_t i = 0; i < SLength; i++) { //--]
    pixels.setPixelColor(i, green);        //  |-- Set all pixels to green
  }                                        //--]
  pixels.show();                           //Write pixel data to strip
  analogWrite(1, FINALBUZZ);               //Turn on buzzer to loud high pitch tone
  delay(1000);                             //Leave buzzer and LEDs on for 1 second
  analogWrite(1, 0);                       //Turn off buzzer
  delay(4000);                             //Leave LEDs on for 4 more seconds
  for (uint16_t i = 0; i < SLength; i++) { //--]
    pixels.setPixelColor(i, black);        //  |-- Set all pixels to black
  }                                        //--]
  pixels.show();                           //Write pixel data to strip
  
} //Go back to the beginning of loop() and begin again.
