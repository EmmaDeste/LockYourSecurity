//blink LED using 1MO and 220kO resistances, a transistor

#define REDLEDPIN 13
#define YELLOWLEDPIN 17 //correspondant au pin A3 = D17 de l'Arduino Nano 

#include <Keypad.h>

void setup() //exécuter qu'une fois au démarrage 
{
  delay(1000);
  //dire que cette PIN est utilisée en sortie 
  pinMode(REDLEDPIN, OUTPUT);
  pinMode(YELLOWLEDPIN, OUTPUT);

}

void loop() //exécuter en boucle
{
  //alternance d'allumage
  digitalWrite(REDLEDPIN, HIGH);
  delay(1000);
  digitalWrite(YELLOWLEDPIN, LOW);
  delay(1000);
  digitalWrite(REDLEDPIN, LOW);
  delay(1000);
  digitalWrite(YELLOWLEDPIN, HIGH);
  delay(1000);

}
