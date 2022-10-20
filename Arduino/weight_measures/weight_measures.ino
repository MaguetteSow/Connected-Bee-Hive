#include "HX711.h"
 
#define LOADCELL_DOUT_PIN  5
#define LOADCELL_SCK_PIN  4
 
HX711 scale;

float calibration_factor = 13350; //-7050 cette variable à régler selon le capteur de poids, 235 avant, 13350 de tare avec une incertitude de .1 à .2
 
void setup() {
//  hc06.begin(9600);
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
  Serial.print((scale.get_units()/2.2046),1); // 1 chiffre après la virgule
  Serial.print(" kg"); 
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();
 
  if(Serial.available())    //permet de calibrer en direct via le moniteur série
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 10;
  }
delay(1000);
}
/*

#include "HX711.h"
const uint8_t&  PIN_WEIGHT_DOUT =A0;
const uint8_t& PIN_WEIGHT_PSCK =A1;

 // DT, CLK                        


HX711 scale(PIN_WEIGHT_DOUT,PIN_WEIGHT_PSCK)



float calibration_factor = -3.7;
float units;
float ounces;

void setup() {
   Serial.begin(9600);
   Serial.println("HX711 calibration sketch");
   Serial.println("Remove all weight from scale");
   Serial.println("Press + or a to increase calibration factor");
   Serial.println("Press - or z to decrease calibration factor");

   scale.set_scale();
   scale.tare();

   long zero_factor = scale.read_average();
   Serial.print("Zero factor: ");
   Serial.println(zero_factor);
}

void loop() {
   scale.set_scale(calibration_factor);

   Serial.print("Reading: ");
   units = scale.get_units(), 10;
   if (units < 0) { units = 0.00; }
   ounces = units * 0.035274;

   Serial.print(ounces);
   Serial.print(" grams"); 
   Serial.print(" calibration_factor: ");
   Serial.print(calibration_factor);
   Serial.println();

   if (Serial.available())
   {
       char temp = Serial.read();
       if (temp == '+' || temp == 'a')
          calibration_factor += 1;
       else if (temp == '-' || temp == 'z')
          calibration_factor -= 1;
   }
}*/
