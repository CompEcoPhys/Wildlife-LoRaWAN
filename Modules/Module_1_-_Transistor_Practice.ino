// The goal is to turn off and on for power saving
const int NPN_Transistor = 2; // pin to control power to the device

void setup() {
  pinMode (NPN_Transistor, OUTPUT); // set the pin as an output
  //digitalWrite (NPN_Transistor, LOW); //starts the LED light off 
  
}

void loop() {
  digitalWrite (NPN_Transistor, HIGH); // turn on the device
  delay(1000); // wait for 1 second
  digitalWrite (NPN_Transistor, LOW); // turn off the device
  delay(1000); // wait for 1 second
}

/*
 * This code uses a transistor as a switch to turn on and off power to a device. 
 * It does this by setting the pin connected to the base of the transistor as 
 * an output and then repeatedly setting the pin to HIGH, which turns on the 
 * transistor and supplies power to the device, 
 * and then setting the pin to LOW, which turns 
 * off the transistor and cuts power to the device.
 */
