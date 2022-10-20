/*
  First Configuration
  This sketch demonstrates the usage of MKR WAN 1300/1310 LoRa module.
  This example code is in the public domain.
*/

#include <MKRWAN.h>
#include <dht.h>

#define dht_apin A0 // Analog Pin sensor is connected to
#define dht_apin22 A1 // Analog Pin sensor is connected to

dht DHT;

LoRaModem modem;

// Uncomment if using the Murata chip as a module
// LoRaModem modem(Serial1);

String appEui;
String appKey;
String devAddr;
String nwkSKey;
String appSKey;

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);
  Serial.begin(115200);
  delay(500);//Delay to let system boot
  Serial.println("DHT11 Humidity & temperature Sensor\n\n");
  delay(1000);//Wait before accessing Sensor

  pinMode(2,OUTPUT);

  //while (!Serial);  //---on enleve la boucle serial pour voir ce qu'on a 
  Serial.println("Welcome to MKR WAN 1300/1310 first configuration sketch");
  Serial.println("Register to your favourite LoRa network and we are ready to go!");
  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    while (1) {}
  };
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  if (modem.version() != ARDUINO_FW_VERSION) {
    Serial.println("Please make sure that the latest modem firmware is installed.");
    Serial.println("To update the firmware upload the 'MKRWANFWUpdate_standalone.ino' sketch.");
  }
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());

  int mode = 0;
  while (mode != 1 && mode != 2) {
//    Serial.println("Are you connecting via OTAA (1) or ABP (2)?");
//    while (!Serial.available());
//    mode = Serial.readStringUntil('\n').toInt();
      mode = 1;
  }

  int connected;
  if (mode == 1) {
//    Serial.println("Enter your APP EUI");
//    while (!Serial.available());
//    appEui = Serial.readStringUntil('\n');
    //  appEui = 0000000000000000;


//    Serial.println("Enter your APP KEY");
//    while (!Serial.available());
//    appKey = Serial.readStringUntil('\n');
    //appKey = DB0DD3985A57E1B00C0B1871D5DD9114;

    appEui = "0000000000000000";
    appKey = "DB0DD3985A57E1B00C0B1871D5DD9114";
    appKey.trim();
    appEui.trim();

    connected = modem.joinOTAA(appEui, appKey);
  } else if (mode == 2) {

    Serial.println("Enter your Device Address");
    while (!Serial.available());
    devAddr = Serial.readStringUntil('\n');

    Serial.println("Enter your NWS KEY");
    while (!Serial.available());
    nwkSKey = Serial.readStringUntil('\n');

    Serial.println("Enter your APP SKEY");
    while (!Serial.available());
    appSKey = Serial.readStringUntil('\n');

    devAddr.trim();
    nwkSKey.trim();
    appSKey.trim();

    connected = modem.joinABP(devAddr, nwkSKey, appSKey);
  }

  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  }

  delay(5000);

  int err;
  modem.setPort(3);
  modem.beginPacket();
  //modem.print("HeLoRA world!");
  DHT.read11(dht_apin);

//  modem.print(DHT.humidity,DHT.temperature);
  //modem.print(DHT.temperature);
  Serial.print("temperature = ");
  Serial.print(DHT.temperature);
  Serial.println("C  ");

  //modem.print(DHT.humidity);
  Serial.print("Current humidity = ");
  Serial.print(DHT.humidity);
  Serial.print("%  ");

  DHT.read22(dht_apin22);
  modem.print(DHT.temperature);
  Serial.print("temperature22 = ");
  Serial.print(DHT.temperature);
  Serial.println("C  ");
  modem.print(DHT.humidity);
  Serial.print("Current humidity22 = ");
  Serial.print(DHT.humidity);
  Serial.print("%  ");
  
  err = modem.endPacket(true);
  if (err > 0) {
    Serial.println("Message sent correctly!");
  } else {
    Serial.println("Error sending message :(");
  }
}

void loop() {
  //Start of Program

  //Fastest should be once every two seconds.
  while (modem.available()) {
//    DHT.read11(dht_apin);

//    Serial.print("Current humidity = ");
//    Serial.print(DHT.humidity);
//    Serial.print("%  ");
//    Serial.print("temperature = ");
//    Serial.print(DHT.temperature);
//    Serial.println("C  ");

    //modem.print(DHT.humidity);
    Serial.write(modem.read());
     digitalWrite(2,HIGH);
  }
  modem.poll();


    //delay(5000);//Wait 5 seconds before accessing sensor again.  
}
