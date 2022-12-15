/*
  Connected Bee-Hive Projet | Polytech 2022-2023 | Groupe WEUZ
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
#define dht_apin22 A1     // Analog Pin sensor is connected to
#define dht_apin22ext A4  // Analog Pin sensor is connected to

#define analogPin A3  // Broche à laquelle la batterie se branche


HX711 scale;      // Capteur de poids
dht DHT;          // DHT22
LoRaModem modem;  // modem Lorawan

//Liaison avec The Things Network
String appEui = "0000000000000000";
String appKey = "DB0DD3985A57E1B00C0B1871D5DD9114";

// Pour la lecture des sondes de temp
OneWire ds18x20[] = { 6, 7 };
const int oneWireCount = sizeof(ds18x20) / sizeof(OneWire);
DallasTemperature sensor[oneWireCount];

/*  Variables */
float calibration_factor = 13350;                                                                      // Facteur de calibration capteur poids
float meas, p, real_meas;                                                                              // variables pour caluler le poids
short temp_sonde1, temp_sonde2, temp_dht, temp_ext, hum_ext, hum_dht, var_poids, var_battery, val_bp;  // pour stocker les valeurs mesurées en 16bits

const byte brocheBouton = 1;  // bouton sur connecteur D2
const int L1 = 2;             // broche 2 du micro-contrôleur se nomme maintenant : L1
bool buttonState = false;
float val = 0;          // variable to store the value read
float resultat = 0;     // le resultat en volt
float pourcentage = 0;  //la charge de la batterie en pourcentage

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(50);  //Delay to let system boot

  pinMode(brocheBouton, INPUT);  // configuration en entrée
  pinMode(L1, OUTPUT);           //L1 est une broche de sortie

  //while (!Serial);  //---on enleve la boucle serial pour voir ce qu'on a
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    while (1) {}
  };
  //Serial.print("Your module version is: ");
  //Serial.println(modem.version());
  if (modem.version() != ARDUINO_FW_VERSION) {
    Serial.println("Please make sure that the latest modem firmware is installed.");
    Serial.println("To update the firmware upload the 'MKRWANFWUpdate_standalone.ino' sketch.");
  }
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());
}

void loop() {

  byte etatEntree = digitalRead(brocheBouton);  // retourne 0 (LOW) ou 1 (HIGH)
  //Serial.println(etatEntree);                   // affichage dans Terminal Série
  //delay(1000); // je mets un temps de delai

  if (etatEntree == HIGH && !buttonState) {
    digitalWrite(L1, HIGH);      // j'allume ma led
    delay(1000);                 // je mets un temps de delai
    digitalWrite(L1, LOW);       // Eteindre L1
    buttonState = !buttonState;  // je change l'état de false à true
  }
  buttonState = etatEntree;
  int mode = 1;
  int connected;
  if (mode == 1) {
    appKey.trim();
    appEui.trim();
    connected = modem.joinOTAA(appEui, appKey);
  }

  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  }

  delay(5000);

  int err;
  modem.setPort(3);
  modem.beginPacket();
  modem.dataRate(2);

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

  // Envoie des valeurs
  modem.write(hum_dht);
  modem.write(temp_dht);
  modem.write(hum_ext);
  modem.write(temp_ext);

  val = analogRead(analogPin);  // read the input pin
  Serial.print("Val : ");
  Serial.println(val);
  resultat = (val * 3.3 / 1023);  //(val*3.3/1024) 10 bits avec analogRead
  pourcentage = (resultat * 100 / 3.3);
  Serial.print("Pourcentage batterie : ");  // debug value
  Serial.println(pourcentage);
  var_battery = (short)(pourcentage * 100);
  modem.write(var_battery);
  Serial.print("Valeur batterie envoyée  : ");
  Serial.println(var_battery);

  // Start up the library on all defined bus-wires
  DeviceAddress deviceAddress;
  for (int i = 0; i < oneWireCount; i++) {
    sensor[i].setOneWire(&ds18x20[i]);
    sensor[i].begin();
    if (sensor[i].getAddress(deviceAddress, 0))
      sensor[i].setResolution(deviceAddress, 8);
  }

  /* Lecture de la température des sondes */
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

    // Multiplication par 100 puis formatage en 16 bits
    temp_sonde1 = (short)(temperature * 100);
    // Envoie des valeurs
    modem.write(temp_sonde1);
  }
  Serial.println();

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare();  //Reset the scale to 0

  long zero_factor = scale.read_average();  //Get a baseline reading
  Serial.print("Zero factor: ");            //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);

  scale.set_scale(calibration_factor);  //Adjust to this calibration factor
  Serial.print("Reading: ");
  Serial.print((scale.get_units() / 2.2046), 1);  // 1 chiffre après la virgule
  real_meas = (scale.get_units() / 2.2046);
  real_meas = (scale.get_units() / 2.2046);
  real_meas = (scale.get_units() / 2.2046);
  real_meas = (scale.get_units() / 2.2046);
  Serial.print(" kg");
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();

  var_poids = (short)(real_meas * 100);
  modem.write(var_poids);

  err = modem.endPacket(true);
  if (err > 0) {  //En cas d'erreur, indiquer sur moniteur serie
    Serial.println("Message sent correctly!");
  } else {
    Serial.println("Error sending message :(");
  }
  //while (modem.available()) {
  //  Serial.write(modem.read());
  //}
  modem.poll();
}