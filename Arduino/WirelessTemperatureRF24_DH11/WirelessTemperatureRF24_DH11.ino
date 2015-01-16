/*
 * Sketch to send DHT-11/DHT-22 temperature sensor data over 
 * a wireless connection using the nRF24l01+ chip.
 */

#include <SPI.h>
#include "RF24.h"
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11
#define CEPIN 7
#define CSPIN 8
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

byte addresses[][6] = {"1Node","2Node"};


/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 1;

// Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 
RF24 radio(CEPIN, CSPIN);
// Temperature sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.begin(115200);
  Serial.println(F("RF24/examples/GettingStarted"));
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));
  
  radio.begin();
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
  
  // Start the radio listening for data
  radio.startListening();
  dht.begin();
}



/**
* Create a data structure for transmitting and receiving data
* This allows many variables to be easily sent and received in a single transmission
* See http://www.cplusplus.com/doc/tutorial/structures/
*/
struct dataStruct{
  float temperature;
  float humidity;  
}sensorData;

void loop() {
  // Home node will transmit packet when temp requested
  if (radio.available()) {
    // Read data from home 
    while(radio.available()) {
     radio.read(&sensorData, sizeof(sensorData));
    } 
    radio.stopListening();              // Stop listening in order write
    sensorData.temperature = dht.readHumidity();
    sensorData.humidity = dht.readTemperature(true);
    radio.write(&sensorData, sizeof(sensorData));
    radio.startListening();
    Serial.print(F("Sent data humidity: "));
    Serial.print(sensorData.humidity);  
    Serial.print(" temperature:  ");
    Serial.println(sensorData.temperature);
  }
} 
