/* 
 *  For more information see http://42bots.com
 *  Configure the ESP8266 unit as a Wi-Fi access point
 */

#include <ESP8266WiFi.h>
#include <SPI.h>

#define SCLK 22
#define MOSI 25
#define MISO 24
#define CS   23
#define ADS_CHANNELS 6

/* Your WiFi Soft Access Point settings */
const char* ssid = "ESP8266";          //this will be the network name
const char* password = "ESP8266Test";  //this will be the network password

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("Configuring WiFi access point...");
  
  /* You can remove the password parameter if you want the AP to be open. */
  boolean result = WiFi.softAP(ssid, password);
  if(result==true) {
    IPAddress myIP = WiFi.softAPIP();
  
    Serial.println("done!");
    Serial.println("");
    Serial.print("WiFi network name: ");
    Serial.println(ssid);
    Serial.print("WiFi network password: ");
    Serial.println(password);
    Serial.print("Host IP Address: ");
    Serial.println(myIP);
    Serial.println("");
  }

  else {
    Serial.println("error! Something went wrong...");
  }

  //setup SPI communication
  SPI.begin(4000000);
  
  pinMode(MOSI, INPUT);

  // turn on SPI in slave mode
  SPCR |= _BV(SPE);
  SPI.attachInterrupt();

  pos = 0;
  received = false;
}

// The value from master is taken from SPDR and stored in Slavereceived
ISR (SPI_STC_vect) {
  byte n = SPDR;

  if (pos < 3*ADS_CHANNELS + 4) // 3 bytes for each of the 6 channels + 3 bytes for status data + 1 byte for battery sensor
    buf [pos++] = n;
  else
    received = true;
}

void loop() {
  Serial.printf("Number of connected devices (stations) = %d\n", WiFi.softAPgetStationNum());
  delay(3000);
  //read data from ads
  if (received) {
    buf [pos] = 0;
    pos = 0;
    received = false;
  }
  //send data over wifi

}
