// rf95_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_client
// Tested with Anarduino MiniWirelessLoRa, Rocket Scream Mini Ultra Pro with
// the RFM95W, Adafruit Feather M0 with RFM95

#include <SPI.h>
#include <RH_RF95.h>

// Singleton instance of the radio driver
RH_RF95 rf95( 4, 3 );
//RH_RF95 rf95(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W
//RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95 

// Need this on Arduino Zero with SerialUSB port (eg RocketScream Mini Ultra Pro)
//#define Serial SerialUSB

uint8_t  red_pin = 6 ;
uint8_t  blue_pin = 9 ;
uint8_t  green_pin = 10 ; 
void setup() 
{
  // Rocket Scream Mini Ultra Pro with the RFM95W only:
  // Ensure serial flash is not interfering with radio communication on SPI bus
//  pinMode(4, OUTPUT);
//  digitalWrite(4, HIGH);

  // set up led indicator
  pinMode(red_pin, OUTPUT);    
  pinMode(blue_pin, OUTPUT ) ;
  pinMode(green_pin, OUTPUT ) ;

  digitalWrite( red_pin, LOW ) ;
  digitalWrite( blue_pin, HIGH ) ;
  digitalWrite( green_pin, HIGH ) ;
   
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  
  //int rc = rf95.init ( ) ;
  if (!rf95.init())
  {
    Serial.println("init failed");  
    digitalWrite( red_pin, HIGH ) ;
    digitalWrite( blue_pin, LOW ) ;
  }
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    // Turn off red
    digitalWrite( red_pin, HIGH ) ;
    
    if (rf95.recv(buf, &len))
    {
      // turn on green
      digitalWrite( green_pin, LOW ) ;
      char msg[50] = { 0 } ;
      //RH_RF95::printBuffer("request: ", buf, len);
      strncpy ( msg, buf, len ) ;

      // format data output
      // millis,msg,rssi
      Serial.print( millis ( ) ) ;
      Serial.print( "," ) ;
      Serial.print(msg);
      Serial.print( "," ) ;
      Serial.println(rf95.lastRssi(), DEC);
      
      // Send a reply
      uint8_t data[] = "And hello back to you";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      //Serial.println("Sent a reply");

      // turn off green
       digitalWrite(green_pin, HIGH);

       // turn on red
       digitalWrite( red_pin, LOW ) ;
    }
    else
    {
      //Serial.println("recv failed");

      // Flash blue for fail
      digitalWrite ( blue_pin, LOW ) ;
      delay( 500 ) ;
      digitalWrite( blue_pin, HIGH ) ;
    }
  }
}
