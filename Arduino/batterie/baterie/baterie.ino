//int analogPin = A3; // potentiometer wiper (middle terminal) connected to analog pin 3
                    // outside leads to ground and +5V
#define analogPin A3
float val = 0;  // variable to store the value read
float resultat= 0; // le resultat en volt
float pourcentage=0; //la charge de la batterie en pourcentage

void setup() {
  Serial.begin(9600);           //  setup serial
}
void loop() {
  val = analogRead(analogPin);  // read the input pin
    Serial.print("Val : ");   
    Serial.println(val);
    resultat = (val*3.3/1023);  //(val*3.3/1024) 10 bits avec analogRead
    pourcentage =(resultat*100/3.3);
    Serial.print("Pourcentage batterie : ");          // debug value
    Serial.println(pourcentage);

  delay(1000);
}
