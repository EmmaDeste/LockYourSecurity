//blink LED using 1MO and 220kO resistances, a transistor

#define REDLEDPIN 13


#include <TimeLib.h>

#define DAYDURATION 86400 //24hrs * 60min * 60s
#define WEEKDURATION 604800 //7 * 24hrs * 60min * 60s
#define FIRSTMONDAYSEC 1641081600 // time between 1st January 1970 and Sunday 2nd January 2022, 00:00

time_t lastTime = 0;

// set the clock time during the current day
void setHM(int hour, int minute)
{
  time_t oldNow = now(); //get date and time
  time_t midnightToday = oldNow-oldNow%DAYDURATION; //00:00 today

  time_t newNow = midnightToday + (time_t)hour*3600 + (time_t)minute*60;

  setTime(newNow);
}

void setDMY(int day, int month, int year)
{
  time_t oldNow = now(); //get date and time
  
  setTime(0,0,0, day, month, year);

  time_t newNow = now() + oldNow%DAYDURATION;

  setTime(newNow);
}

void printTime(time_t t) 
{
  Serial.print(day(t)); 
  Serial.print("/");
  Serial.print(month(t)); 
  Serial.print("/");
  Serial.print(year(t)); 
  Serial.print("\t");
  
  Serial.print(hour(t)); 
  Serial.print(":");
  Serial.print(minute(t)); 
  Serial.print(":");
  Serial.print(second(t));
  Serial.print("\tDay of the week is:");
  Serial.print(weekday(t)); 
  Serial.print("\tWeek number is:");
  //Serial.print(weekNum(t)); //weeks since beginning of 2022
  Serial.print("\ttime_t:");
  Serial.print(t); //seconds since 01/01/1970
  Serial.print("\t"); //seconds since 01/01/1970
}



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

    if (nbBuff==6 && tabBuff[0] == '*') //*2134# for 21h34
    {
      int h;
      int m;
      sscanf(tabBuff, "*%02d%02d#", &h, &m);
      setHM(h,m);
    }

    if (nbBuff==10 && tabBuff[0] == '*') //*19032022# for 19th March 2022
    {
      int d;
      int m;
      int y;
      sscanf(tabBuff, "*%02d%02d%04d#", &d, &m, &y);
      setDMY(d,m,y);
    }
    
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



#include <Stepper.h>

#define STEPSPERREVOLUTION 10

// initialize the stepper library on pins 14 through 17: 
Stepper myStepper(STEPSPERREVOLUTION, 14, 15, 16, 17);







void setup() //exécuter qu'une fois au démarrage 
{
  delay(1000);
  //dire que cette PIN est utilisée en sortie 
  pinMode(REDLEDPIN, OUTPUT);

  //initialiser la liaison série à 9600 bits par seconde
  Serial.begin(9600);

  clearBuff();

  setTime(12,0,0, 23,5,2018);
  setHM(21,20);

}


void loop() //exécuter en boucle
{

  //déclare une variable qui va récupérer ce que l'on tape sur le clavier
  char key = keypad.getKey();
  //si il y a eu une touche d'entrée, il l'écrit sur le moniteur (écran)
  if (key != NO_KEY){
    addBuff(key); //ln pour sauter la ligne après chaque caractère
  }
  

  time_t t = now();
  //TODO: deal with  something typed quickly after # (meaning less that 1s being lastTime != t)
  if (lastTime != t)
  {
    printTime(t);
    processBuff();
    Serial.println();
    lastTime = t;
  }

  digitalWrite(REDLEDPIN, HIGH);
  
  // set the speed of the motor to 3 RPMs (= 1 rev in 20s)
  myStepper.setSpeed(3);
  
  // one rev is 10 steps representing 20s, so 8 steps represents 16s
  myStepper.step(8); //this line will take 16s
  delay(2000);

  myStepper.step(-8); //this line will take 16s
  delay(2000);
  
  digitalWrite(REDLEDPIN, LOW);
  delay(200);

  digitalWrite(REDLEDPIN, HIGH);
  delay(200);

  digitalWrite(REDLEDPIN, LOW);
  
}
