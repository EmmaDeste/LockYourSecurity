//blink LED using 1MO and 220kO resistances, a transistor

//Works with v4 of the app

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

int min5Num(time_t t)
{
  int res = (t - FIRSTMONDAYSEC) / 300; // 5min = 300s 
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



int nbErrors = 0;
int refracDelay = 0;

void refracGoodCode()
{
  nbErrors = 0;
  refracDelay = 0;

}

void refracWrongCode()
{
  
  nbErrors += 1;

  if (nbErrors == 3)
    refracDelay = 1;

  if (nbErrors >= 7)
    nbErrors = 7; // to put a ceiling at 32s, not block the system for days
   else
    refracDelay *= 2;

   if (nbErrors > 3)
   {
      Serial.print("Resistance delay of ");
      Serial.print(refracDelay);
      Serial.println();
      delay(refracDelay * 1000); // to have s instead of ms
   }
}

void refracSetup()
{
  refracGoodCode();
}

void refracLoop()
{
  
}









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

void openDoor(int deg, int ms)
{
  stepperTurn(deg);
  delay(ms);
  stepperTurn(-deg); //don't forget to close the door after
}

void openS(void)
{
  openDoor(30, 1000);
}

void openL(void)
{
  openDoor(90, 2000);
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





#define GOODKEY 1
#define NOKEY 2
#define FAKEKEY 3
int keyStatus = NOKEY; //first initilization to 2
int keyVoltage; //voltage at the middle of the divider bridge


void keySetup (void)
{
  keyStatus = NOKEY;
  pinMode(A5, INPUT);
}

void keyLoop(void)
{
  keyStatus = (now() /20) %3 + 1;
  
  keyVoltage = analogRead(A5); // we read the tension corresponding to the divider bridge
  long int voltExpected = (1023L * 938L) / (2000L + 938L);
  if ( (voltExpected * 0.9 < keyVoltage) && (keyVoltage < 1.1 * voltExpected) )
    keyStatus = GOODKEY;
   else if (keyVoltage < 1023 / 20) //meaning very low value: 5% of full scale
          keyStatus = NOKEY;
        else
          keyStatus = FAKEKEY;
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

void min5Change()
{
  codes.currentDeliverySlotNumber = now();
  
  copyCodes(codes.currentDeliveryCodeS, codes.nextDeliveryCodeS);
  setRandCodes(codes.nextDeliveryCodeS);

  copyCodes(codes.currentDeliveryCodeL, codes.nextDeliveryCodeL);
  setRandCodes(codes.nextDeliveryCodeL);

}

void serializeCodes (char* dest)
{
  char serialCodes;
  
  //use of dictionary structure

  sprintf(dest, " {\"CDS\" : \"%s\", \"CDL\" : \"%s\", \"NDS\" : \"%s\", \"NDL\" : \"%s\", \"CWS\" : \"%s\", \"CWL\" : \"%s\", \"NWS\" : \"%s\", \"NWL\" : \"%s\", \"KEY\" : %i } ", 
          codes.currentDeliveryCodeS, codes.currentDeliveryCodeL, codes.nextDeliveryCodeS, codes.nextDeliveryCodeL,
          codes.currentWeekCodeS, codes.currentWeekCodeL, codes.nextWeekCodeS, codes.nextWeekCodeL, 
          keyStatus );

  int i=strlen(dest);

  //{ "CD1" : "123456", "CD2" : "123456", "ND1" : "123456", "ND2" : "123456", "CW1" : "123456", "CW2" : "123456", "NW1" : "123456", "NW2" : "123456", "KEY" : 2 }
  
}

void codesSetup()
{ 
  weekChange(); // first creating nextWeekCode 
  weekChange(); // second nextWeekCode becomes currentWeekCode, and an other nextWeekCode is created

  min5Change(); // first creating nextDeliveryCode 
  min5Change(); // second nextDeliveryCode becomes currentDeliveryCode, and an other nextDeliveryCode is created
  
  setRandCodes(codes.currentDeliveryCodeS);
  setRandCodes(codes.currentDeliveryCodeL);
  setRandCodes(codes.nextDeliveryCodeS);
  setRandCodes(codes.nextDeliveryCodeL);
}


void codesLoop()
{
  int wn = weekNum(now());
  if (wn != codes.currentWeekNumber)
    weekChange();

   int sn = min5Num(now()); //slotnumber
   if (sn != codes.currentDeliverySlotNumber)
    min5Change();

  char buff[111 + 8*CODELENGTH]; // 8 for the codes and 111 for the identifier & syntax { "CD1" : "  ",
  serializeCodes(buff); //load in another emplacement in memory
  MyBlue.write(buff);
    
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
  
  else
  {
    clearBuff();
  }
}

void serialShowBuff()
{
  Serial.print(nbBuff);
  Serial.print(" --> ");
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
    

    if (nbBuff==6 && tabBuff[0] == '*' && keyStatus==GOODKEY) //*2134# for 21h34 -> administrator
    {
      int h;
      int m;
      sscanf(tabBuff, "*%02d%02d#", &h, &m);
      setHM(h,m);
    }

    if (nbBuff==10 && tabBuff[0] == '*' && keyStatus==GOODKEY) //*19032022# for 19th March 2022 -> administrator
    {
      int d;
      int m;
      int y;
      sscanf(tabBuff, "*%02d%02d%04d#", &d, &m, &y);
      setDMY(d,m,y);
    }

    if (nbBuff==4 && tabBuff[0] == '*' && keyStatus==GOODKEY) // *06# for 6° and *95# for -5° //for maintenance only -> administrator
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


    if (nbBuff==7) // 213532# for 213532
    {

      //test large at first, in case of small & large codes are unlikely (almost impossible) the same
      //using OR instead of conditions, in case of larges codes are unlikely (almost impossible) the same
      if ( strncmp(tabBuff, codes.currentWeekCodeL, 6) == 0 || 
            strncmp(tabBuff, codes.currentDeliveryCodeL, 6) == 0 || 
            strncmp(tabBuff, codes.nextDeliveryCodeL, 6) == 0 ) //6 characters compared in order to ignore #
       { 
          openL();
          refracGoodCode();
       }
        
      else if (strncmp(tabBuff, codes.currentWeekCodeS, 6) == 0 || 
            strncmp(tabBuff, codes.currentDeliveryCodeS, 6) == 0 ||
            strncmp(tabBuff, codes.nextDeliveryCodeS, 6) == 0 )
       { 
          openS();
          refracGoodCode();
       }

          else
            refracWrongCode();
      
    }
    
    clearBuff();
  }
}


void buffSetup()
{
  clearBuff();
}

void buffLoop()
{
  processBuff();
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






void setup() //exécuter qu'une fois au démarrage 
{
  delay(1000);
  //dire que cette PIN est utilisée en sortie 
  pinMode(REDLEDPIN, OUTPUT);

  //initialiser la liaison série à 9600 bits par seconde
  Serial.begin(9600);

  setTime(12,0,0, 23,5,2018);
  setHM(21,20);

  refracSetup();

  buffSetup();
  
  stepperSetup();

  btSetup();

  codesSetup();

  keySetup();
}


void loop() //exécuter en boucle
{

  //déclare une variable qui va récupérer ce que l'on tape sur le clavier
  char key = keypad.getKey();
  //si il y a eu une touche d'entrée, il l'écrit sur le moniteur (écran)
  if (key != NO_KEY){
    addBuff(key); 
  }
  

  time_t t = now();
  //TODO: deal with  something typed quickly after # (meaning less that 1s being lastTime != t)
  if (lastTime != t)
  {
    
    buffLoop();

    if (t%10 ==0)
    {
      Serial.print(keyStatus);
      Serial.print(". keyVoltage = ");
      Serial.print(keyVoltage);
      Serial.print(" ");
      printTime(t);
      Serial.println();
    }
    

    if (t%10 == 0)
    {
      codesLoop(); //calcule et envoie les codes toutes les 10 secondes
    }


    if (t%3==0)
    {
      digitalWrite(REDLEDPIN, HIGH);
    }
    else{
      digitalWrite(REDLEDPIN, LOW);
    }
    keyLoop();
    
    lastTime = t;
  }

  
  
}
