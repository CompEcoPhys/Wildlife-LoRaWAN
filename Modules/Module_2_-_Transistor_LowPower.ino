/*
 * X - The goal is to turn off and on for power saving
 * The goal is to turn off and on LED light, and use LowPower
 */

#include "LowPower.h" // include the LowPower library

const int NPN_Transistor = 2; // pin to control power to the device

void setup() {
  pinMode (NPN_Transistor, OUTPUT); // set the pin as an output
}

void loop() {

  digitalWrite (NPN_Transistor, LOW); // turn off the device
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); // put the device in power-down mode for 8 seconds
  delay(3000); // wait for 3 seconds (this delay is not needed if you want to wake up exactly after 8 seconds)
  digitalWrite (NPN_Transistor, HIGH); // turn on the device
  delay(1000); // wait for 1 second
  digitalWrite (NPN_Transistor, LOW); // turn off the device
  delay(1000); // wait for 1 second
}
