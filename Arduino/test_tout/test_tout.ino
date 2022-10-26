/*
  Connected Bee-Hive Projet - Polytech 2022-2023 
  This sketch demonstrates the usage of MKR WAN 1300/1310 LoRa module.
*/


/* Bibliothéques requises */
#include <dht.h>
#include <MKRWAN.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <HX711.h>

/* Déclaration des Pins */
#define LOADCELL_DOUT_PIN 5
#define LOADCELL_SCK_PIN 4
#define dht_apin A0       // Analog Pin sensor is connected to
#define dht_apin22 A1    // Analog Pin sensor is connected to
HX711 scale;            // Capteur de poids 
float calibration_factor = 21067;  // for 16,6 -> 21067 and for ~93,5 -> 21387
float meas, p, real_meas, var_poids; // variables poids
short temp_sonde1, temp_sonde2, temp_dht, hum_dht;
dht DHT; // DHT22
LoRaModem modem; // modem Lorawan

/* Liaison avec The Things Network  */
String appEui;
String appKey;
String devAddr;
String nwkSKey;
String appSKey;

OneWire ds18x20[] = { 6, 7 };
const int oneWireCount = sizeof(ds18x20) / sizeof(OneWire);
DallasTemperature sensor[oneWireCount];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(50);  //Delay to let system boot

  // Start up the library on all defined bus-wires
  DeviceAddress deviceAddress;
  for (int i = 0; i < oneWireCount; i++) {
    sensor[i].setOneWire(&ds18x20[i]);
    sensor[i].begin();
    if (sensor[i].getAddress(deviceAddress, 0))
      //sensor[i].setResolution(deviceAddress, 8);
      sensor[i].setResolution(deviceAddress, 8);
  }

  Serial.println("DHT11 Humidity & temperature Sensor\n\n");
  delay(100);  //Wait before accessing Sensor

  pinMode(2, OUTPUT);

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
    mode = 1;
  }

  int connected;
  if (mode == 1) {
    appEui = "0000000000000000";
    appKey = "DB0DD3985A57E1B00C0B1871D5DD9114";
    appKey.trim();
    appEui.trim();

    connected = modem.joinOTAA(appEui, appKey);
  } else if (mode == 2) {

    Serial.println("Enter your Device Address");
    while (!Serial.available())
      ;
    devAddr = Serial.readStringUntil('\n');

    Serial.println("Enter your NWS KEY");
    while (!Serial.available())
      ;
    nwkSKey = Serial.readStringUntil('\n');

    Serial.println("Enter your APP SKEY");
    while (!Serial.available())
      ;
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

  delay(500);

  int err;
  modem.setPort(3);
  modem.beginPacket();
  /* Lecture de la température et de l'humidité du DHT11*/
  //DHT.read11(dht_apin);

  //modem.print(DHT.temperature);
  //Serial.print("temperature = ");
  //Serial.print(DHT.temperature);
  //Serial.println("C  ");

  //modem.print(DHT.humidity);
  //Serial.print("Current humidity = ");
  //Serial.print(DHT.humidity);
  //Serial.print("%  ");

  /* Lecture de la température et de l'humidité du DHT11*/
  DHT.read22(dht_apin22);
  modem.print(DHT.temperature);
  Serial.print("temperature22 = ");
  Serial.print(DHT.temperature);
  Serial.println("C  ");

  modem.print(DHT.humidity);
  Serial.print("Current humidity22 = ");
  Serial.print(DHT.humidity);
  Serial.print("%  ");

  /* Lecture de la température des sondes */
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  for (int i = 0; i < oneWireCount; i++) {
    sensor[i].requestTemperatures();
  }
  Serial.println("DONE");

  delay(100);
  for (int i = 0; i < oneWireCount; i++) {
    float temperature = sensor[i].getTempCByIndex(0);
    Serial.print("Temperature for the sensor ");
    Serial.print(i + 1);
    Serial.print(" is ");
    Serial.println(temperature, 1);
    modem.print(temperature);
  }
  Serial.println();

  err = modem.endPacket(true);
  if (err > 0) {
    Serial.println("Message sent correctly!");
  } else {
    Serial.println("Error sending message :(");
  }

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare();  //Reset the scale to 0

  long zero_factor = scale.read_average();  //Get a baseline reading
  Serial.print("Zero factor: ");            //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);

  meas = scale.get_units(10);
  //Serial.print(meas);
  p = meas / 3.7;
  //Serial.print(p);
  //Serial.print(" kg ");  //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  real_meas = meas - p;
  var_poids = real_meas;
  
  meas = scale.get_units(10);
  //Serial.print(meas);
  p = meas / 3.7;
  //Serial.print(p);
  //Serial.print(" kg ");  //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  real_meas = meas - p;

  //Serial.print(" calibration_factor: ");
  while (real_meas = var_poids ) {
    meas = scale.get_units(10);
    //Serial.print(meas);
    p = meas / 3.7;
    //Serial.print(p);
    //Serial.print(" kg ");  //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
    real_meas = meas - p;
    var_poids = real_meas;
    Serial.print(real_meas);
    Serial.println(" kg ");  
  }
  modem.print(real_meas);  

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
    //modem.print(real_meas);
    Serial.write(modem.read());
    digitalWrite(2, HIGH);
  }
  modem.poll();


  //delay(5000);//Wait 5 seconds before accessing sensor again.
}