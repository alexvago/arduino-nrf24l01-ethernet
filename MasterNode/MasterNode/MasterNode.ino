
/*
* Scketch for Master Node. It represents the center of a star network, 
* directly connected to Internet through Ethernet and connected to Slave nodes, 
* who provide sensor readings, through NRF24L01+ module.
*/

#include <SPI.h>
#include <Ethernet.h>
#include <printf.h>
#include "DigitalIO.h"
#include "RF24_soft.h"

#define delayMillis 3000UL  //delay for each transmission

unsigned long thisMillis = 0;
unsigned long lastMillis = 0;

char params[32]; //POST parameters

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Enter a MAC address for your controller below.

char server[] = "192.168.1.100";    // name address for local Server
int port = 3000;

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 99);

// Initialize the Ethernet client library

EthernetClient client;

byte addresses[][6] = {"1Node","2Node"};


/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 0;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
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
    // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }

   // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");
  
  radio.begin();

  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);
  
  // Open a writing pipe
    radio.openWritingPipe(addresses[0]);
  // Start the radio listening for data
  radio.startListening();
  radio.printDetails();
}




void loop() {
  

/****************** Pong Back Role ***************************/


   thisMillis = millis();
   if(thisMillis - lastMillis > delayMillis)
  {
    lastMillis = thisMillis;

    // params must be url encoded.
    char str_temp[6];
    dtostrf(myData.value, 4, 2, str_temp);
    sprintf(params,"templ=%s", str_temp);    
    Serial.println(params);
    if(!postPage(server,port,"/",params)) Serial.print(F("Fail "));
    else Serial.print(F("Pass "));
  }    

  if( radio.available()){
                                               // While there is data ready
      radio.read( &myData, sizeof(myData) ); // Get the payload

      Serial.print(F("Sent response "));
      Serial.print(myData._micros);  
      Serial.print(F(" : "));
      Serial.println(myData.value);
      delay(2);
   }

} // Loop

byte postPage(char* domainBuffer,int port, char* page,char* thisData)
{
  int inChar;
  char outBuf[64];

  Serial.print(F("connecting..."));

  if(client.connect(domainBuffer,port) == 1)
  {
    Serial.println(F("connected"));

    // send the header
    sprintf(outBuf,"POST %s HTTP/1.1",page);
    client.println(outBuf);
    sprintf(outBuf,"Host: %s",domainBuffer);
    client.println(outBuf);
    client.println(F("Connection: close\r\nContent-Type: application/x-www-form-urlencoded"));
    sprintf(outBuf,"Content-Length: %u\r\n",strlen(thisData));
    client.println(outBuf);

    // send the body (variables)
    client.print(thisData);
  }
  else
  {
    Serial.println(F("failed"));
    return 0;
  }

  while(client.connected()){
      while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
    delay(1);
    client.stop();
  }
  return 1;
}


