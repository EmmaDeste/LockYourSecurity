#define PINLED 13

void setup() //exécuter qu'une fois au démarrage
{
  //dire que cette PIN est utilisée en sortie
  pinMode(PINLED, OUTPUT);

}

void loop() //exécuter en boucle
{
  //allumer la LED, attendre 1s, puis l'éteindre, attendre 1s, puis la rallumer, ...
  digitalWrite(PINLED, HIGH);
  delay(1000);
  digitalWrite(PINLED, LOW);
  delay(1000);

}
