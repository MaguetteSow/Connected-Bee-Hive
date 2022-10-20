#include <MKRWAN.h>
LoRaModem modem;

//#include "arduino_secrets.h" 
String appEui = "0000000000000000";
String appKey = "F7BDD0EB7B7E46A09F1ACA086C2968F2";

bool connected;
int err_count;
int con; 
void setup() {
   Serial.begin(115200);
  while (!Serial);
  Serial.println("Welcome to MKR WAN 1300/1310 first configuration sketch");
  Serial.println("Register to your favourite LoRa network and we are ready to go!");
   modem.begin(EU868);
   delay(1000);      // apparently the murata dislike if this tempo is removed...
   
   connected=false;
   err_count=0;
   con =0;
}

void loop() {
   char msg[2] = {0,1};
   
   if ( !connected ) {
    int ret=modem.joinOTAA(appEui, appKey);
    if ( ret ) {
      connected=true;
      modem.minPollInterval(60);
      Serial.println("Connected");
      modem.dataRate(5);   // switch to SF7
      delay(100);          // because ... more stable
      err_count=0;
    }
  }

  
  con++; 
  Serial.print("Join test : ");
  Serial.println(con);
  
  if ( connected ) {
    int err=0;
    modem.beginPacket();
    modem.write(msg,2);
    err = modem.endPacket(true);
    if ( err <= 0 ) {
      // Confirmation not received - jam or coverage fault
      err_count++;
      if ( err_count > 50 ) {
        connected = false;
      }
      // wait for 2min for duty cycle with SF12 - 1.5s frame
      for ( int i = 0 ; i < 120 ; i++ ) {
        delay(1000);
      }
    } else {
      err_count = 0;
      // wait for 20s for duty cycle with SF7 - 55ms frame
      delay(20000);
    }
  }
}
