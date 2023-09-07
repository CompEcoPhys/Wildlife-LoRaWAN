#include <SPI.h> //used for communication between sensors
#include <TinyGPS++.h> //used to obtain GPS coordinates
#include <SoftwareSerial.h>
#include "LowPower.h"

const int NPN_GPS = 2; // pin to control the power of the GPS module
#define RX_Pin 4 // RX pin for software serial connection to GPS module
#define TX_Pin 3 // TX pin for software serial connection to GPS module
SoftwareSerial ss(RX_Pin, TX_Pin); // software serial connection to GPS module
TinyGPSPlus GPS; // object to hold GPS data

// time GPS should stay on. GPS module uses approximately 60mAh/s
int ON = 30; // seconds
uint32_t ON_duration = (ON * 1000); // converts seconds to milliseconds 

float Lat, Long, Alt;


void get_coords() {
  // For the duration of ON_duration, parse GPS data and report key values
  for (unsigned long GPS_on = millis(); millis() - GPS_on < ON_duration;) {
    while (ss.available()) {
      char c = ss.read();
      GPS.encode(c);
    }
  }
   
  if (GPS.location.isValid()) {
    Lat = GPS.location.lat() ;
    Long = GPS.location.lng();
    Alt = GPS.altitude.meters();
    Serial.println(F("Found Early"));
    //digitalWrite(NPN_GPS, LOW); // turn off GPS if coordinates are valid
  }
}

void setup() {
  Serial.begin(9600);
  ss.begin(9600);
  
  pinMode(NPN_GPS, OUTPUT);
  digitalWrite(NPN_GPS, LOW); // start the GPS off
}

void loop() {
  Serial.println("searching..."); //provides a timestamp for when it turned on
  digitalWrite(NPN_GPS, HIGH);
  ON = millis();

  get_coords();
  Serial.println(Lat);
  Serial.println(Long);
  digitalWrite(NPN_GPS, LOW); // if no coordinates are found, turn off after the search time
  Serial.println("Turning OFF!");
  
  // calculate the time to sleep before next GPS search
  float sampl_int = 1; // sampling interval in minutes
  uint32_t OFF = ((sampl_int * 60) - ((ON/1000)/60))/8; // in seconds, for LowPower, largest is 8s
  
  // put the device to sleep for the calculated time
  for (int i = 0; i < OFF; i++) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  }
}


/*
 Overall, the code turns on the GPS module, waits for 
 the ON_duration amount of time to parse the GPS data, 
 reports the key values, and turn off the GPS module. 
 Then the device goes to sleep for the calculated time OFF before next GPS search.

>'ON' and 'ON_duration' are used to control how long 
the GPS module is powered on for, which is 30 seconds in this case. 

>'get_coords()' function is used to turn on the GPS, parse the data for the
  duration of ON_duration and report key values, Lat and Long, if the GPS location is valid.

>'setup()' function is used to initialize the serial connection,
  software serial connection and pin mode for the NPN_GPS and set it to low to start the GPS off.

>'loop()' function is used to call the get_coords() function, 
  turn off the GPS module and put the device to sleep for the calculated time OFF before next GPS search.

>'sampl_int' is used to control the sampling interval of the 
  GPS module, which is 1 minute in this case.

>'OFF' is calculated by subtracting the time GPS is on from the 
  sampling interval in seconds and divide by 8 (largest sleep time for LowPower).

> 'for' loop is used to put the device to sleep for 
   the calculated time OFF before next GPS search.
 
 */
