/*
  PROJET RUCHES CONNECTEES | Polytech 2022-2023 | Groupe 4
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
#define dht_apin22 A1     // Analog Pin sensor is connected to
#define dht_apin22ext A4  // Analog Pin sensor is connected to
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

/*  Variables */
float calibration_factor = 13350;       // Facteur de calibration capteur poids
float meas, p, real_meas, temperature;  // variables pour caluler le poids

// variables pour stocker les valeurs mesurées en 16bits
short temp_sonde1, temp_sonde2;
short temp_dht, temp_ext, hum_ext, hum_dht;
short var_poids, var_battery, val_bp;

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
  //while (!Serial)
  //  ;
  Serial.println("Welcome to MKR WAN 1300/1310 first configuration sketch");
  Serial.println("Register to your favourite LoRa network and we are ready to go!");
  modem.begin(EU868);
  //delay(1000);  // apparently the murata dislike if this tempo is removed...
  delay(50);

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

  if (!connected) {
    int ret = modem.joinOTAA(appEui, appKey);
    if (ret) {
      connected = true;
      modem.minPollInterval(60);
      Serial.println("Connected");
      //modem.setPort(3);
      modem.dataRate(5);  // switch to SF7
      //modem.dataRate(2);
      delay(100);         // because ... more stable
      err_count = 0;
    }
  }
  Serial.print("Connexion avec TTN établi ");
  //Serial.println(con);
}

void loop() {
  //char msg[2] = { 0, 1 };
  //con++;
  //Serial.print("Join test : ");
  //Serial.println(con);

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
  Serial.print("Reading: ");
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

    err = modem.endPacket(true); // fin de l'envoi du paquet

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
      delay(20000);
    }
  }
}