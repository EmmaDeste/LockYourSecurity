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

int weekNum(time_t t)
{
  int res = (t - FIRSTMONDAYSEC) / WEEKDURATION;
  return res;
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
  Serial.print(weekNum(t)); //weeks since beginning of 2022
  Serial.print("\ttime_t:");
  Serial.print(t); //seconds since 01/01/1970
  Serial.print("\t"); //seconds since 01/01/1970
}




#include <SoftwareSerial.h> 
SoftwareSerial MyBlue(3, 2); // RX | TX //in of the Arduino is the out of the Bluetooth module: arduino TX corresponds to the Bluetooth RXD

void btSetup(void)
{
  MyBlue.begin(9600); //meaning the communication between Arduino and HC-05 is 9600bits/s (default value)
}

void btSendChar(char c)
{
  int flag = 0;
  if (MyBlue.available()) 
   flag = MyBlue.write(c);
}





#include <Keypad.h>

//we will a buffer to store keystroke
#define BUFFKEYSIZE 12 //since we need 10 to up the date

char tabBuff[BUFFKEYSIZE+1];  // +1 to have enough space for the '\0' ending  character
int nbBuff = 0; //number of characters currently in the buffer, so indicate the position of the next character that will be enter

void clearBuff(void)
{
  nbBuff = 0;
}

void addBuff(char c)
{
  if (nbBuff < BUFFKEYSIZE)
  {
    tabBuff[nbBuff] = c;
    nbBuff++;
    tabBuff[nbBuff] = '\0';
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
      Serial.println("Essai envoie bt");
      MyBlue.write(tabBuff);
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

    if (nbBuff==4 && tabBuff[0] == '*') // *06# for 6° and *95# for -5° //for maintenance only
    {
      int sign;
      int value;
      sscanf(tabBuff, "*%01d%01d#", &sign, &value);
      if (sign == 0)
      {
        stepperTurn(value);
      }
      if (sign == 9)
      {
        stepperTurn(-value);
      }
    }

    // Emma: begin the rotations according to code
    if (nbBuff==4 && tabBuff[0] == '*') // *25# for 25° and *95# for -95° //for maintenance only
    {
      int sign;
      int value;
      sscanf(tabBuff, "*%02d#", &value);
      if (value > 00 && value <= 90) 
      {
        stepperTurn(value);
      }
      if (value > 90)
      {
        stepperTurn(-value);
      }
    }
    
    clearBuff();
  }
}



#define CODELENGTH 6


typedef struct Codes
{
  int currentWeekNumber;
  char currentWeekCodeS[CODELENGTH +1];  //so of type char*   //Small or Large
  char currentWeekCodeL[CODELENGTH +1];

  char nextWeekCodeS[CODELENGTH +1];
  char nextWeekCodeL[CODELENGTH +1];

  int currentDeliverySlotNumber;
  char currentDeliveryCodeS[CODELENGTH +1];
  char currentDeliveryCodeL[CODELENGTH +1];
  
  char nextDeliveryCodeS[CODELENGTH +1];
  char nextDeliveryCodeL[CODELENGTH +1];

}Codes;

Codes codes;


void copyCodes(char* dest, char* source)
{
  int i;
  for(i=0; i<CODELENGTH; i++)
  {
    dest[i] = source[i];
  }
}

char randChar()
{
  int r = rand() % 10;
  char res = (char)('0' + r);
  return res;
}

char* setRandCodes(char* dest)
{
  int i;
  for(i=0; i<CODELENGTH; i++)
  {
    dest[i] = randChar();
  }
  dest[CODELENGTH] = '\0'; //not +1: I leave the loop because I am at CODELENGTH = 6 since begin at i=0
}

void weekChange()
{
  codes.currentWeekNumber = weekNum(now());
  
  copyCodes(codes.currentWeekCodeS, codes.nextWeekCodeS);
  setRandCodes(codes.nextWeekCodeS);

  copyCodes(codes.currentWeekCodeL, codes.nextWeekCodeL);
  setRandCodes(codes.nextWeekCodeL);
}

void serializeCodes (char* dest)
{
  char serialCodes;
  
  //use of dictionary structure

  //TODO: 7 codes to add with %s
  sprintf(dest, "{\"CD1\" : \"%s\", \"CD2\" : \"123456\", \"ND1\" : \"123456\", \"ND2\" : \"123456\", \"CW1\" : \"%s\", \"CW2\" : \"123456\", \"NW1\" : \"123456\", \"NW2\" : \"123456\"}", 
          codes.currentDeliveryCodeS, codes.currentWeekCodeS);

  int i=strlen(dest);
  Serial.print("longueur: ");
  Serial.println(i);

  //{ "CD1" : "123456", "CD2" : "123456", "ND1" : "123456", "ND2" : "123456", "CW1" : "123456", "CW2" : "123456", "NW1" : "123456", "NW2" : "123456" }
  
}

void codesSetup()
{ 
  weekChange(); // first creating nextWeekCode 
  weekChange(); // second nextWeekCode becomes currentWeekCode, and an other nextWeekCode is cerated
  
  setRandCodes(codes.currentDeliveryCodeS);
  setRandCodes(codes.currentDeliveryCodeL);
  setRandCodes(codes.nextDeliveryCodeS);
  setRandCodes(codes.nextDeliveryCodeL);
}


void codesLoop()
{
  Serial.print(" week num: ");
  Serial.print(codes.currentWeekNumber);
  Serial.print(" week code: ");
  Serial.print(codes.currentWeekCodeL);
  Serial.print(" next week code: ");
  Serial.println(codes.nextWeekCodeL);

  int wn = weekNum(now());
  if (wn != codes.currentWeekNumber)
    weekChange();

  char buff[100 + 8*CODELENGTH]; // 8 for the codes and 100 for the identifier & syntax { "CD1" : "  ",
  serializeCodes(buff); //change in the emplacement in memory
  //sprintf(buff,"*1345#");
  MyBlue.write(buff);
    
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

//we connected the keyboard to the pins from D6 to D12 //old: from D2 to D8
byte rowPins[ROWS] = {6, 7, 8, 9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {10, 11, 12}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );



#include <Stepper.h>

#define STEPSPERREVOLUTION 2048  // 2048 steps results in 1 revolution

// initialize the stepper library on pins 14 through 17: 
Stepper myStepper(STEPSPERREVOLUTION, 14, 15, 16, 17);

void stepperSetup(void)
{
  // set the speed of the motor to 6 RPMs (= 1 rev in 10s)
  myStepper.setSpeed(6); // at maximum speeed (500sps/s) 1step = 2ms, so 1 revolution = 2048 * 2 ms = 4.096s, so maximum RPMs is 60s/4s = 15RPMs
  //one full opening of the door is one quarter revolution, so 2.5s (=10s/4)
}

void stepperTurn(int deg)
{
  long nbSteps = (deg * (long)STEPSPERREVOLUTION) /360;
  myStepper.step(nbSteps);
}



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

  stepperSetup();

  btSetup();

  codesSetup();
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

    if (t%10 == 0)
    {
      codesLoop();
      char s[140];
      serializeCodes (s);
      Serial.print("Json="); // ou printf("Json=");
      Serial.println(s); // ou printf("%s\n”,s);
    }
    
    
  }

  digitalWrite(REDLEDPIN, HIGH);
  
  // we perform a quarter revolution clockwise
  //stepperTurn(90);
  //delay(4000);//maintain the door open for 4s

  // we perform a quarter revolution anticlockwise
  //stepperTurn(-90);
  //delay(8000); //maintain the door closed 8s between two vehicles

  digitalWrite(REDLEDPIN, HIGH);
  delay(200);

  digitalWrite(REDLEDPIN, LOW);

  
}
