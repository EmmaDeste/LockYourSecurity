//blink LED using 1MO and 220kO resistances, a transistor

#define REDLEDPIN 13
#define YELLOWLEDPIN 17 //correspondant au pin A3 = D17 de l'Arduino Nano 

#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = 
{
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
//byte rowPins[ROWS] = {6, 7, 8, 9}; //connect to the row pinouts of the keypad
//byte colPins[COLS] = {10, 11, 12}; //connect to the column pinouts of the keypad

//byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
//byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() //exécuter qu'une fois au démarrage 
{
  delay(1000);
  //dire que cette PIN est utilisée en sortie 
  pinMode(REDLEDPIN, OUTPUT);
  pinMode(YELLOWLEDPIN, OUTPUT);

  //initialiser la liaison série à 9600 bits par seconde
  Serial.begin(9600);

}

void loop() //exécuter en boucle
{
  //alternance d'allumage
  digitalWrite(REDLEDPIN, HIGH);
  digitalWrite(YELLOWLEDPIN, LOW);
  delay(100);
  digitalWrite(REDLEDPIN, LOW);
  digitalWrite(YELLOWLEDPIN, HIGH);
  delay(100);

  //déclare une variable qui va récupérer ce que l'on tape sur le clavier
  char key = keypad.getKey();
  //si il y a eu une touche d'entrée, il l'écrit sur le moniteur (écran)
  if (key != NO_KEY){
    Serial.println(key);
  }
  
}
