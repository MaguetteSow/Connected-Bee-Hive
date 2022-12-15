/*
  PROJET RUCHES CONNECTEES | Polytech 2022-2023 | Groupe 4
*/

/* Bibliothéques requises */
#include <dht.h>
#include <MKRWAN.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <HX711.h>
#include <Wire.h>
#include "DFRobot_INA219.h"

/* Déclaration des Pins */
#define LOADCELL_DOUT_PIN 5
#define LOADCELL_SCK_PIN 4
#define dht_apin A0       // Analog Pin sensor is connected to
#define dht_apin22 A1     // Analog Pin sensor is connected to
#define dht_apin22ext A2  // Analog Pin sensor is connected to A4 BEFORE
#define analogPin A3      // Broche à laquelle la batterie se branche

HX711 scale;      // Capteur de poids
dht DHT;          // DHT22
LoRaModem modem;  // modem Lorawan

/*Liaison avec The Things Network */
//String appEui = "0000000000000000";
//String appKey = "DB0DD3985A57E1B00C0B1871D5DD9114";

// Carte Sapidda
String appEui = "0000000000000000";
String appKey = "0DAB3DDAF184926E0347CD274CFEF983";

// Init pour la lecture des sondes de temp
OneWire ds18x20[] = { 6, 7 };
const int oneWireCount = sizeof(ds18x20) / sizeof(OneWire);
DallasTemperature sensor[oneWireCount];

//Capteur de puissance - luminosite INA
DFRobot_INA219_IIC ina219(&Wire, INA219_I2C_ADDRESS4);
// Revise the following two paramters according to actual reading of the INA219 and the multimeter
// for linearly calibration
float ina219Reading_mA = 1000;
float extMeterReading_mA = 1000;

/*  Variables */
float calibration_factor = 13350;       // Facteur de calibration capteur poids
float meas, p, real_meas, temperature;  // variables pour caluler le poids
float meas_pow; // Variable pour mesurer la conso luminosité

// variables pour stocker les valeurs mesurées en 16bits
short temp_sonde1, temp_sonde2;
short temp_dht, temp_ext, hum_ext, hum_dht;
short var_poids, var_battery, val_bp;
short var_pow;

const byte brocheBouton = 1;  // bouton sur connecteur D2
const int L1 = 2;             // broche 2 du micro-contrôleur se nomme maintenant : L1
bool buttonState = false;     // initialisation de l'état du btn
float val = 0;                // variable to store the value read
float resultat = 0;           // le resultat en volt
float pourcentage = 0;        //la charge de la batterie en pourcentage

bool connected;
int err_count;
int con;

void setup() {
  Serial.begin(115200);
  Serial.println("Welcome to MKR WAN 1300/1310 connexion");
  modem.begin(EU868);
  delay(1000);  // apparently the murata dislike if this tempo is removed...

  connected = false;
  err_count = 0;
  con = 0;

  // Start up the library on all defined bus-wires
  DeviceAddress deviceAddress;
  for (int i = 0; i < oneWireCount; i++) {
    sensor[i].setOneWire(&ds18x20[i]);
    sensor[i].begin();
    if (sensor[i].getAddress(deviceAddress, 0))
      sensor[i].setResolution(deviceAddress, 8);
  }

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare();  //Reset the scale to 0

  long zero_factor = scale.read_average();  //Get a baseline reading
  Serial.print("Zero factor: ");            //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);

  //Initialize the sensor
  if (ina219.begin() != true) {
    Serial.println("INA219 begin failed");
  } 
  //Linear calibration
  ina219.linearCalibrate(/*The measured current before calibration*/ ina219Reading_mA, /*The current measured by other current testers*/ extMeterReading_mA);
  Serial.println();

  if (!connected) {
    int ret = modem.joinOTAA(appEui, appKey);
    if (ret) {
      connected = true;
      modem.minPollInterval(60);
      Serial.println("Connected");
      //modem.setPort(3);
      modem.dataRate(5);  // switch to SF7
      //modem.dataRate(2);
      delay(100);  // because ... more stable
      err_count = 0;
    }
  }
  Serial.print("Connexion avec TTN établi ");
}

void loop() {
  con++;
  Serial.print("Lecture n° : ");
  Serial.println(con);

  /* Lecture de la température et de l'humidité du DHT11*/
  DHT.read22(dht_apin22);
  Serial.print("temperature22 = ");
  Serial.print(DHT.temperature);
  Serial.println("C  ");

  Serial.print("Current humidity22 = ");
  Serial.print(DHT.humidity);
  Serial.print("%  ");

  // Multiplication par 100 puis formatage en 16 bits
  temp_dht = (short)((DHT.temperature) * 100);
  hum_dht = (short)((DHT.humidity) * 100);

  DHT.read22(dht_apin22ext);
  Serial.print("temperature22 = ");
  Serial.print(DHT.temperature);
  Serial.println("C  ");

  Serial.print("Current humidity22 = ");
  Serial.print(DHT.humidity);
  Serial.print("%  ");

  // Multiplication par 100 puis formatage en 16 bits
  temp_ext = (short)((DHT.temperature) * 100);
  hum_ext = (short)((DHT.humidity) * 100);

  /* Batterie */
  val = analogRead(analogPin);  // read the input pin
  Serial.print("Val : ");
  Serial.println(val);
  resultat = (val * 3.3 / 1023);  //(val*3.3/1024) 10 bits avec analogRead
  pourcentage = (resultat * 100 / 3.3);
  Serial.print("Pourcentage batterie : ");  // debug value
  Serial.println(pourcentage);
  var_battery = (short)(pourcentage * 100);
  Serial.print("Valeur batterie  : ");
  Serial.println(var_battery);

  /* Lecture de la température des sondes */
  Serial.print("Requesting temperatures...");
  for (int i = 0; i < oneWireCount; i++) {
    sensor[i].requestTemperatures();
  }
  Serial.println("DONE");
  for (int i = 0; i < oneWireCount; i++) {
    temperature = sensor[i].getTempCByIndex(0);
    Serial.print("Temperature for the sensor ");
    Serial.print(i + 1);
    Serial.print(" is ");
    Serial.println(temperature, 1);
  }
  //delay(100);

  scale.set_scale(calibration_factor);  //Adjust to this calibration factor
  //Serial.print("Reading: ");
  Serial.print((scale.get_units() / 2.2046), 1);  // 1 chiffre après la virgule
  Serial.print(" kg");
  real_meas = (scale.get_units() / 2.2046);
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();

  /*ENVOI DES DONNEES SUR TTN SUR 2 OCTETS */
  if (connected) {
    int err = 0;
    modem.beginPacket();  // début de l'envoi du paquet

    // Envoi des valeurs de température et d'humidité à TTN
    modem.write(hum_dht);
    modem.write(temp_dht);
    modem.write(hum_ext);
    modem.write(temp_ext);

    // envoi du pourcentage de la batterie
    modem.write(var_battery);

    for (int i = 0; i < oneWireCount; i++) {
      // Multiplication par 100 puis formatage en 16 bits
      temp_sonde1 = (short)(temperature * 100);
      // Envoie des valeurs des sondes
      modem.write(temp_sonde1);
    }

    // Envoi de la valeur du poids
    var_poids = (short)(real_meas * 100);
    modem.write(var_poids);

    // Mesure de la puissance

    Serial.print("Power:        ");
    Serial.print(ina219.getPower_mW(), 1);
    Serial.println("mW");
    meas_pow = ina219.getPower_mW();
    var_pow = (short)(ina219.getPower_mW() * 100);
    modem.write(var_pow);
    Serial.print("Var_pow sent:        ");
    Serial.print(var_pow);
    Serial.print("meas pow:        ");
    Serial.print(meas_pow);

    err = modem.endPacket(true);  // fin de l'envoi du paquet

    Serial.print(" Valeur erreur: ");
    Serial.println(err);
    if (err <= 0) {
      // Confirmation not received - jam or coverage fault
      err_count++;
      Serial.print("Valeur err_count: ");
      Serial.println(err_count);
      if (err_count > 50) {
        connected = false;
      }
      // wait for 2min for duty cycle with SF12 - 1.5s frame
      for (int i = 0; i < 120; i++) {
        delay(1000);
      }
    } else {
      err_count = 0;
      // wait for 20s for duty cycle with SF7 - 55ms frame
      delay(60000);
      //delay(600000); //DELAI DE 10MIN
    }
  }
}