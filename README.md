# Wildlife-LoRaWAN
Development of wildlife tracking and data transmission

Sections
1.	General Overview
2.	Bill of Materials
3.	Pin Mapping (Arduino Pro-Mini + RFM95W)
4.	Power Consumption (coming soon)
5.	Battery considerations (further details coming soon)
6.	Module Sketches



## 1. General Overview
Current repository is for a tracking device that be used for monitoring animal movement. However, the LoRa node can be designed according to a specific study organism. Current iterations are designed for small sized mammals (e.g., squirrels), but designs can be done for turtle species as well. This can include, but is not limited to, vest attachments, external attachments on collars, shell mounting, and more. The approach uses commercial off the shelf parts to create a relatively cost-effective GPS tracker that will transmit data at set intervals to a gateway. The current version is for the ‘SquirrelTracker’ GPS unit. 

The collar is a combination of steel fishing wire inside a nylon sheath, and a crimp is used to hold the collar together. Superglue is applied to the crimp so it adheres the nylon sheath and prevents the animal from prematurely breaking off the unit.

Animals that are released are intended to be recaptured near the end-of-life for the unit (see R script for power consumptions) to then remove the unit. If an animal does not move from its location for an extended period of time, then a researcher will go to the last known location to determine whether the collar has fallen off, or the state of the animal (e.g., mortality due to predation, roadkill, etc.)


## 2. Bill of Materials
Current price is based on most recent purchase for COTS breakout boards. 

| Part        | Supplier         | Cost  |
| ------------- |:-------------:| -----:|
| Arduino ProMini      | Sparkfun | $10.95|
| RFM95W Breakout Board      | Adafruit      |   $19.95 |
| GPS Module  (GP-U027 or GP-1818MK)| Sparkfun  |    $19.95 |
| NPN Transistor (PN2222)  |  Digikey |$0.49|
| 1200mAh Battery (TL-4902/S)| Digikey | $6.89|
| Supercapacitor (TPLC-3R8/10MR8X14)| Digikey | $2.37|
| uFL Antenna (915mHz) (1173-1135-ND) | Digikey| $9.66 |
| uFL connector jack (CONUFL001-SMD-T) | Digikey| $0.71|

approximate total: $70.97

## 3. Pin Mapping
__RFM95W to Arduino Pro Mini__       

| RFM95W |  Pro Mini|
| ------------- |-----:|
|G1 | (DIO1) 6 |                  
|RST | 9   |                     
|CS | 10  |                       
|MOSI | 11 |
|MISO | 12 |
|SCK | 13 |
|G0 (DIO0) | 5|                   
|GND | GND  |                
|Vin | VCC |
       
__GP-U027 or GP-1818MK to Arduino Pro Mini__                 
|GPS Unit | Pro Mini |
| ------------- |-----:|
|Black wire (GND) | NPN |       
|White Wire (TX) | 4  |         
|Red Wire (Power) | VCC (connect with node) |

## 4. Power Consumption


## 5. Battery Considerations
The LoRa transceiver will work well with a LiPo battery. However, these batteries can be rather bulky. The limitation with using a different energy dense battery chemistry (e.g., Lithium Thionyl Chloride 1200mAh battery ½ AA battery) is the voltage spike from the LoRa unit will cause the device to 

## 6. Module Sketches
To facilitate a better understanding of the full Wildlife LoRaWAN sketch, there are individual modules that contain sketches for (1) using an NPN transistor, which is for turning off and on the GPS module with the LowPower library, (2) obtaining GPS coordinates at set intervals using the TinyGPS++ library, and (3) using the GPS in low power mode at set intervals with the NPN transistor.  

