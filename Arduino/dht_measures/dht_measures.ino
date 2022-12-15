#include <dht.h>

//#define dht_apin A0 // Analog Pin sensor is connected to
#define dht_apin 23

dht DHT;

void setup() {

  Serial.begin(9600);
  delay(500);  //Delay to let system boot
  Serial.println("DHT11 Humidity & temperature Sensor\n\n");
  delay(1000);  //Wait before accessing Sensor

}  //end "setup()"

void loop() {
  //Start of Program

  DHT.read11(dht_apin);
  Serial.print("Current humidity = ");
  Serial.print(DHT.humidity);
  Serial.print("%  ");
  Serial.print("temperature = ");
  Serial.print(DHT.temperature);
  Serial.println("C  ");
}