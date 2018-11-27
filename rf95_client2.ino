// rf95_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_server
// Tested with Anarduino MiniWirelessLoRa, Rocket Scream Mini Ultra Pro with
// the RFM95W, Adafruit Feather M0 with RFM95

// JR: set up to send data to python data logging program.
// format:
//
// millis, seq_number, rssi, marker_number

#include <SPI.h>
#include <RH_RF95.h>

// Singleton instance of the radio driver
RH_RF95 rf95( 4, 3 ) ;
//RH_RF95 rf95(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W
//RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95 

// Need this on Arduino Zero with SerialUSB port (eg RocketScream Mini Ultra Pro)
//#define Serial SerialUSB

#define SW_PIN 10
uint8_t  red_pin = 6 ;
uint8_t  blue_pin = 9 ;
uint8_t  green_pin = 5 ; 

void setup() 
{
  // Rocket Scream Mini Ultra Pro with the RFM95W only:
  // Ensure serial flash is not interfering with radio communication on SPI bus
//  pinMode(4, OUTPUT);
//  digitalWrite(4, HIGH);

  pinMode ( SW_PIN, INPUT_PULLUP ) ;

  // set up led indicator
  pinMode(red_pin, OUTPUT);    
  pinMode(blue_pin, OUTPUT ) ;
  pinMode(green_pin, OUTPUT ) ;

  digitalWrite( red_pin, LOW ) ;
  digitalWrite( blue_pin, HIGH ) ;
  digitalWrite( green_pin, HIGH ) ;
  
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
  {
    Serial.println("init failed");
    Serial.println("init failed");  
    digitalWrite( red_pin, HIGH ) ;
    digitalWrite( blue_pin, LOW ) ;
  }
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // JR -- hard-coded 915 Mhz in the code.
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

void sendMsg ( char* msg )
{

  digitalWrite( red_pin, LOW ) ;
  digitalWrite( green_pin, HIGH ) ;
   
  // Send a message to rf95_server
  rf95.send((uint8_t*) msg, strlen ( msg ) );
  
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.waitAvailableTimeout(3000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      digitalWrite( red_pin, HIGH ) ;
      digitalWrite( green_pin, LOW ) ;
      delay ( 100 ) ;
      //Serial.print("got reply: ");
      //Serial.println((char*)buf);
      //Serial.print("RSSI: ");
      //Serial.println(rf95.lastRssi(), DEC);    
    }
    else
    {
      //Serial.println("recv failed");
      digitalWrite ( green_pin, HIGH ) ;
      digitalWrite ( blue_pin, LOW ) ;
      delay ( 500 ) ;
      digitalWrite( blue_pin, HIGH ) ;
    }
  }
  else
  {
    digitalWrite( green_pin, HIGH ) ;
    digitalWrite ( blue_pin, LOW ) ;
    delay (100 ) ;
    digitalWrite( blue_pin, HIGH ) ;
    //Serial.println("No reply, is rf95_server running?");
  }
}

void loop()
{
  static int seq = 0 ;
  static int markerSeq = 0 ;
  uint8_t* buf_ptr = 0 ;
  uint8_t data[50] = { 0 } ;
  uint32_t timeStamp = millis ( ) ;

  // Point to beginning of data buffer
  buf_ptr = data ;
  digitalWrite( red_pin, LOW ) ;

  // Convert the timestamp to ASCII
  ltoa ( timeStamp, buf_ptr, 10 ) ;

  // Bump past timeStamp
  buf_ptr += strlen ( buf_ptr ) ;
  //buf_ptr ++ ;
  
  Serial.print( timeStamp ) ;
  Serial.print( "," ) ;
  Serial.print ( seq ) ;
  Serial.print( "," ) ;
  Serial.print( rf95.lastRssi() ) ;
  Serial.print( "," ) ;
  Serial.println ( markerSeq ) ;

  // comma seperate
  *buf_ptr = ',' ;
  buf_ptr ++ ;

  // convert and append seq num
  itoa( seq, buf_ptr, 10 ) ;
  buf_ptr = data ;
  buf_ptr += strlen ( data ) ;
  *buf_ptr = ',' ;
  buf_ptr ++ ;
  
  // convert and append client rssi
  itoa ( rf95.lastRssi(), buf_ptr, 10 ) ;
  
  // Heartbeat sequence number
  digitalWrite( red_pin, HIGH ) ;

  //Serial.print ( "buf=" ) ;
  //buf_ptr = data ;
  //Serial.println ( ( char* ) &data[0] ) ;
  
  sendMsg ( data ) ;
  seq ++ ;

  // Send a marker number
  if ( digitalRead ( SW_PIN ) == LOW )
  {
    // flash purple for button press
    digitalWrite ( red_pin, LOW ) ;
    digitalWrite ( blue_pin, LOW ) ;
    
    delay ( 100 ) ;  // debounce
    data[0] = 'M' ;
    data[1] = ' ' ;
    itoa ( markerSeq, &data[2], 10 ) ;
    //Serial.print ( "sending marker " ) ;
    //Serial.println ( markerSeq ) ;
    sendMsg( data ) ;
    markerSeq ++ ;
    digitalWrite( blue_pin, HIGH ) ;
  }
  
  delay(400);
}
