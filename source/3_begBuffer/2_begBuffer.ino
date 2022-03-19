//blink LED using 1MO and 220kO resistances, a transistor

#define REDLEDPIN 13
#define YELLOWLEDPIN 17 //correspondant au pin A3 = D17 de l'Arduino Nano 



#include <Keypad.h>

//we will a buffer to store keystroke
#define BUFFKEYSIZE 12 //since we need 10 to up the date

char tabBuff[BUFFKEYSIZE];
int nbBuff = 0; //number of characters currently in the buffer, so indicate the position of the next character that will be enter

void clearBuff(void)
{
  nbBuff = 0;
}

void addBuff(char c)
{
  if (nbBuff < BUFFKEYSIZE)
  {
    tabBuff[nbBuff]=c;
    nbBuff++;
  }
  //TODO: what to do in else case
}

void serialShowBuff()
{
  for (int i=0; i<nbBuff; i++)
  {
    Serial.print(tabBuff[i]);
  }
}

void processBuff(void)
{
  if (nbBuff > 0 && nbBuff < BUFFKEYSIZE && tabBuff[nbBuff-1] == '#')
  {
    serialShowBuff();
    Serial.println();
    clearBuff();
  }
}



const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = 
{
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

//we connected the keyboard to the pins from D2 to D8
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

  clearBuff();

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
    addBuff(key); //ln pour sauter la ligne après chaque caractère
  }
  processBuff();
  
}
