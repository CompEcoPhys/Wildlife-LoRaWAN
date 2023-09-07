/*--------------------------------------*\
 *        Squirrel Tracker              *     
 *    Code by: Adam F. Parlin           *
 *  Current e-mail: afparlin@esf.edu    *
 *                                      *
 *    Board: Arduino Pro Mini 3.3v 8Mhz *
 *    GPS Unit: GP-20U7                 *
 *    Transciever: RFM95W (Adafruit)    *                                
\*--------------------------------------*/

//Below is an annotated version of the LoRaWAN GPS 
//Tracker with notes on formats using the LMIC library. 
//Other LoRaWAN protocols exist, so make sure to use
//The necessary formatting.

/*-------------------*\
 *    Libraries      *     
\*-------------------*/
// LoRaWAN
#include "lmic.h"
#include "hal/hal.h"

// Low Power
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include "LowPower.h"

// Arduino
#include <avr/pgmspace.h>
#include <Arduino.h>

// Timing Interval
#include <elapsedMillis.h>

//GPS Module
#include <TinyGPS++.h> //used to obtain GPS coordinates
#include <SoftwareSerial.h>

//CayanneLPP 
#include "CayenneLPP.h" //used for decoding GPS coordinates and plotting them in the application server

/*-------------------------------------------------*\
 *   Part 1: Setting groundwork for communication  *
\*-------------------------------------------------*/

/* App EUI */
//Little Endian Format (LSB) when using LMIC library
//if using chirpstack, leave empty
static const u1_t PROGMEM APPEUI[8]={}; //Fill in your AppEUI
void os_getArtEui (u1_t* buf) {memcpy_P(buf, APPEUI, 8);}

/* Dev EUI */
//Little endian format (LSB) when using LMIC library
//Chirpstack will give a Big Endian format. Convert to little endian (e.g., http:// https://www.save-editor.com/tools/wse_hex.html)
static const u1_t PROGMEM DEVEUI[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //Fill in your DevEUI
void os_getDevEui (u1_t* buf) {memcpy_P(buf, DEVEUI, 8);}

/* App Key */
// Big endian format (MSB) when using LMIC library
static const u1_t PROGMEM APPKEY[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
void os_getDevKey (u1_t* buf) {memcpy_P(buf, APPKEY, 16);}

/*---------------------------------------------------*\
 *   Part 2: Setting and defining variables / pinout *
\*---------------------------------------------------*/

//The Arduino Pro Mini need pinmap defined. Below is the
//mapping used for the SquirrelTracker
const lmic_pinmap lmic_pins = {
    .nss = 10, 
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 9,
    .dio = {5,6,LMIC_UNUSED_PIN}, //works with pins 5 and 6, which allow 2 (NPN + GND) and 4 for GPS
};

/*---------------------------------------------------*\
 *   Part 3: Variables for sampling interval         *
\*---------------------------------------------------*/
#define Sampl_Int 180 //how often the device will sample in minutes

//GPS setup
#define RX_Pin 4
#define TX_Pin 3

const int NPN_GPS = 2;
SoftwareSerial ss(RX_Pin, TX_Pin);
TinyGPSPlus GPS;

//Duration
   //time GPS to stay on. Device uses approximately 60mAh/s,
int ON = 30; // seconds
uint32_t ON_duration = (ON * 1000); // converts ms to seconds 

uint32_t LoRa_ON = 90000; //How long the LoRa attempts to send signal in ms. multiply seconds by 1000

//CayanneLPP variables to send data packet
CayenneLPP lpp(51);
float Long, Lat, Alt;

//Placed in the onEvent()
bool dataSent = false;

/*-------------------------------------------------*\
 *   Part 4: Setting up functions to run           *
\*-------------------------------------------------*/

//Obtaining Coordinates
void get_coords () {
  // For one second parse GPS data and report key values
  digitalWrite(NPN_GPS, HIGH);
   for (unsigned long GPS_on = millis(); millis() - GPS_on < ON_duration;) { //turn the number to ON_duration
    while (ss.available()) {
      GPS.encode(ss.read());
    }
   }
   
  if (GPS.location.isValid()) {

    Lat = GPS.location.lat() ;
    Long = GPS.location.lng();
    Alt = GPS.altitude.meters();
    Serial.println(F("Found Early")); //for testing
    delay(100);
    digitalWrite(NPN_GPS, LOW); // turn off GPS if coordinates are valid
      }
 }

void onEvent (ev_t ev) {
  //More options are available, but for this code the focus is on sending data 
  //after a connection has been established. If this fails, it will try again 
  //after the set interval
  switch(ev) {
    case EV_TXSTART:
    Serial.println(F("Attempting Transmission"));
    break;
    case EV_TXCOMPLETE:
      dataSent = true;
      Serial.println(F("EV_TXCOMPLETE"));
      break;
  }
}

void transmit_coords() {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & (1 << 7)) {
    // Something already in the queque
    return;
  }
  // Put together the data to send, add your sensor data here
  digitalWrite(NPN_GPS, HIGH);
  get_coords();
  digitalWrite(NPN_GPS, LOW); 
  lpp.reset();
  lpp.addGPS(1, Lat, Long, Alt);
 
  // Add to the queque
  dataSent = false;
  LMIC_setTxData2(1, lpp.getBuffer(), lpp.getSize(),0);
  Serial.println(F("Done collecting"));
   
}

void deepSleep(unsigned long sleepMilliSeconds) {
  while (sleepMilliSeconds >= 8000) { LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); sleepMilliSeconds -= 8000; }
  if (sleepMilliSeconds >= 4000)    { LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF); sleepMilliSeconds -= 4000; }
  if (sleepMilliSeconds >= 2000)    { LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF); sleepMilliSeconds -= 2000; }
  if (sleepMilliSeconds >= 1000)    { LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); sleepMilliSeconds -= 1000; }
  if (sleepMilliSeconds >= 500)     { LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF); sleepMilliSeconds -= 500; }
  if (sleepMilliSeconds >= 250)     { LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF); sleepMilliSeconds -= 250; }
  if (sleepMilliSeconds >= 125)     { LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF); sleepMilliSeconds -= 120; }
  if (sleepMilliSeconds >= 64)      { LowPower.powerDown(SLEEP_60MS, ADC_OFF, BOD_OFF); sleepMilliSeconds -= 60; }
  if (sleepMilliSeconds >= 32)      { LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF); sleepMilliSeconds -= 30; }
  if (sleepMilliSeconds >= 16)      { LowPower.powerDown(SLEEP_15MS, ADC_OFF, BOD_OFF); sleepMilliSeconds -= 15; }
}

/*-------------------------------------------------*\
 *   Part 5: Setup code, runs once                 *
\*-------------------------------------------------*/
void setup() {
  
  Serial.begin(9600); //baud rate

  pinMode(NPN_GPS, OUTPUT); //set the NPN transistor for the GPS
  digitalWrite(NPN_GPS, LOW); // start the GPS in OFF mode to avoid power drain
  
  ss.begin(9600); //gps baud rate
  // LMIC init
  os_init();

  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  // Enabled data rate adaptation
  LMIC_setAdrMode(1);

  // Disable link check validation
  LMIC_setLinkCheckMode(0);

  // Set data rate and transmit power
  LMIC_setDrTxpow(DR_SF7, 15);

  LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100); 

  //Serial.println(F("Begin Process!")); 
}

/*-------------------------------------------------*\
 *   Part 5: Main code, runs repeatedly            *
\*-------------------------------------------------*/

void loop() {
  // Reset timer
  elapsedMillis unit_on = 0;
  Serial.println(F("Collecting Data"));
  // Queue the data packet to be sent
  transmit_coords();

  // Wait for the data to send or timeout
  while (!dataSent && unit_on < LoRa_ON) { //On Time of the unit - basically will be on for a total of 2 mimnutes max
    os_runloop_once();
    delay(1);
  }
  
  os_runloop_once();
  Serial.println(F("LoRa Time On met, going to sleep"));
  
  // Shutdown the radio
  os_radio(RADIO_RST); // erases any data that did not transmit, in order to clear and send for the next time

  // Go to sleep until next time
  signed long sleep = 60000 * Sampl_Int;
  sleep -= unit_on;
  deepSleep(constrain(sleep, 10000, 60000 * Sampl_Int));
}
