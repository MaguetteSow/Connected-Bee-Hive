/*
 Example using the SparkFun HX711 breakout board with a scale
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 19th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 This is the calibration sketch. Use it to determine the calibration_factor that the main example uses. It also
 outputs the zero_factor useful for projects that have a permanent mass on the scale in between power cycles.
 
 Setup your scale and start the sketch WITHOUT a weight on the scale
 Once readings are displayed place the weight on the scale
 Press +/- or a/z to adjust the calibration_factor until the output readings match the known weight
 Use this calibration_factor on the example sketch
 
 This example assumes pounds (lbs). If you prefer kilograms, change the Serial.print(" lbs"); line to kg. The
 calibration factor will be significantly different but it will be linearly related to lbs (1 lbs = 0.453592 kg).
 
 Your calibration factor may be very positive or very negative. It all depends on the setup of your scale system
 and the direction the sensors deflect from zero state
 This example code uses bogde's excellent library:"https://github.com/bogde/HX711"
 bogde's library is released under a GNU GENERAL PUBLIC LICENSE
 Arduino pin 2 -> HX711 CLK
 3 -> DOUT
 5V -> VCC
 GND -> GND

 Most any pin on the Arduino Uno will be compatible with DOUT/CLK.

 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.

*/

#include <HX711.h>

#define LOADCELL_DOUT_PIN  5
#define LOADCELL_SCK_PIN  4

HX711 scale;

float calibration_factor = 21067; // for 16,6 -> 21067 and for ~93,5 -> 21387
float meas, p, real_meas;

/*
La technique de calibration consiste à :

--envoyer la commande set_scale()   (sans aucun paramètre)
--envoyer la commande tare()   (sans aucun paramètre)
--placer un poids connu sur la balance
--faire une lecture avec get-units(10)  ( 10 mesures consécutives ) et afficher le résultat.
--diviser la valeur obtenue par le poids référence en utilisant la bonne unité ( kg, g, lbs... )
--le résultat x de cette division est la valeur à envoyer par set_scale(x) avant de faire les mesures,
--mais une seule fois pour l'ensemble des mesures, jusqu'à une éventuelle nouvelle calibration.
*/


void setup() {
  Serial.begin(9600);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
  
  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}

void loop() {

  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  Serial.print("Reading: ");
  //Serial.print(scale.get_units(), 1);
  meas = scale.get_units(10);
  Serial.print(meas);
  p = meas/3.7;
  Serial.print(p);
  Serial.print(" kg "); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  real_meas = meas - p;
  Serial.print(real_meas);
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 10;
  }
}
