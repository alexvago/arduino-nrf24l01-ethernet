
/*
*  Scketch for Slave node, who receives sensors' data, collects it and sends it to
*  Master node through NRF24L01+
*/

#include <SPI.h>
#include <printf.h>
#include "RF24.h"

byte addresses[][6] = {"1Node","2Node"};


/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 1;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 9 & 10 */
RF24 radio(9,10);
/**********************************************************/


/**
* Create a data structure for transmitting and receiving data
*/
struct dataStruct{
  unsigned long _micros;
  float value = 0;
}myData;

void setup() {

  Serial.begin(115200);
  printf_begin(); 
  
  radio.begin();
  

  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);
  
  //Open reading pipe
  radio.openReadingPipe(1,addresses[0]);

  radio.printDetails();

}




void loop() {
  
  
/****************** Ping Out Role ***************************/  
if (role == 1)  {

    Serial.println(F("Now sending"));
    int sensorVal = analogRead(A0);
    float voltage = (sensorVal / 1024.0) * 5.0;
    float temperature = (voltage - 0.5)*100;
    Serial.print(temperature);
    
    myData.value = temperature;

    myData._micros = micros();
     if (!radio.write( &myData, sizeof(myData) )){
       Serial.println(F("failed"));
     }

    // Try again 1s later
    delay(1000);
  }

} // Loop

