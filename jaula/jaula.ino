#include <Wire.h>
#include "DHT.h"
#include <Servo.h> 
#include <LiquidCrystal_I2C.h>
#include "pitches.h"

#define BUZZER 6 
#define DHTPIN 8
#define FEEDBUTTON A0
#define HEATER 9
#define FAN 10
#define FEED 11
#define FLASH 13

//-----------------------------
int melody[] = {
  NOTE_A7,0, NOTE_A7,0,NOTE_A7,0,NOTE_A7,0,NOTE_A7,0,NOTE_A7,0,NOTE_A7,0};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
   4, 2, 4, 2, 4, 8, 4, 8, 4, 8, 4, 8, 2, 4 };
//-----------------------------

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
const long hours = 3.6e6;
const long minutes = 60000;
const long seconds = 1000;

// CONFIGURACION
const int minTemp = 19; // por debajo encender calefaccion
const int maxTemp = 23; // por encima encender ventilacion
const int HUM = 62; // humedad de referencia

const long feedInterval = 12*hours;
const long fanInterval = 3*hours;

const long maxTempVentilando = 40*minutes; // tiempo maximo con los ventiladores encendido
const long tiempoDescanso = 2*hours; // tiempo que los ventiladores deben estar apagados como minimo para dejar descansar entre ventilaciones.


// Almacenan el instante en que se apagara el recurso
long apagarCalefaccion = 0;

// Ultimo instante en que se ejecuto el recurso
long lastComedero = 0;
long lastVentilacion = 0;


const long refrescoPantalla = 20*seconds;
const long refrescoBotonera = 180;
long lastRefrescoPantalla = 0;
long lastRefrescoBotonera = 0;

Servo comedero;

int botonComedero= 0;
int botonera = 0;
int BP = 0;
int LBP = 0;
long KPA = 0;

bool ventilando = false;
long descansando = -999999;

// addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

void feed(){
  digitalWrite(FEED, HIGH);
  for (int i=95; i > 30;i-=15) {
    comedero.write(i);
    delay(5);
  }
  for (int i=29; i <= 95 ;i+=15) {
    comedero.write(i);
    delay(5);
  }
  delay(100);
  digitalWrite(FEED, LOW);
}


void communication () {
  if (Serial.available()) { //Si está disponible
    char c = Serial.read(); //Guardamos la lectura en una variable char
    if (c == 'H') { //Si es una 'H', enciendo el LED
      
        for (int thisNote = 0; thisNote < 14; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(6, melody[thisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(6);
    
  }
      digitalWrite(FLASH, HIGH);
    } 
    if (c == 'L') { //Si es una 'L', apago el LED
      digitalWrite(FLASH, LOW);
    }
    if (c == 'W') { //Si es una 'W', enciendo calefaccion y piloto indicativo
      digitalWrite(HEATER, HIGH);
    }
    if (c == 'S') { //Si es una 'S', apago la calefaccion
      digitalWrite(HEATER, LOW);
    }
    if (c == 'F') { 
      feed();
  }
}
}

int menuIndex = 0;
void (*menuFunction)(int, bool);

void showMainMenu(int index, bool enter)
{
    index %= 2;
    if (enter) 
    {
      switch (index)
      { 
        case 0:
          menuFunction = &showTest; // aqui va la funcion del siguiente menu si pulsas en la opcion 0 de este menu
          break;
        case 1:
          menuFunction = &showConfigMenu;
          break;
      }
      (*menuFunction)(index, false);
      return;
    }
    
    lcd.backlight(); 
    char cursor[] = "=>";
    char n[] = "";
    lcd.clear();
    lcd.setCursor(0,0);
delay(10);
    lcd.print("Temp: ");
    lcd.setCursor(7,0);
delay(10);
    lcd.print(dht.readTemperature());
    lcd.setCursor(0,1);
    lcd.print("Hum: ");
delay(10);
    lcd.setCursor(7,1);
    delay(10);
    lcd.print(dht.readHumidity());
delay(10);
    lcd.setCursor(0,2);
    lcd.print(strcat(index==0?cursor:n, "TEST"));
delay(10);
    lcd.setCursor(0,3);
    lcd.print(strcat(index==1?cursor:n, "CONFIG"));
delay(10);
}

void showTest(int index, bool enter)
{
    lcd.clear();
    lcd.backlight(); 
    lcd.print("Efectuando pruebas!");
    digitalWrite(FLASH, HIGH);
    delay(500);
    digitalWrite(FLASH, LOW);
    digitalWrite(FAN, HIGH);
    delay(500);
    digitalWrite(FLASH, HIGH);
    digitalWrite(HEATER, HIGH);
    delay(2000);
    digitalWrite(FLASH, LOW);
    delay(10000);
    digitalWrite(FLASH, HIGH);
    delay(500);
    digitalWrite(FLASH, LOW);
    delay(500);
    digitalWrite(HEATER, LOW);
    digitalWrite(FAN, LOW);
    menuFunction = &showMainMenu;
    showMainMenu(index, enter);
}

void showTempMenu(int index, bool enter)
{
    if (enter) 
    {
      switch (index)
      { 
        case 0:
          //menuFunction = &showTemp; // aqui va la funcion del siguiente menu si pulsas en la opcion 0 de este menu
          break;
        case 1:
          //menuFunction = &showTemp;
          break;
        case 2:
          //menuFunction = &showTemp;
          break;
        case 3:
          //menuFunction = &showTemp;
          break;
      }
      (*menuFunction)(index, false);
      return;
    }
    
    char cursor[] = "=>";
    char n[] = "";
    lcd.clear();
    lcd.backlight(); 
    lcd.setCursor(0,0);
    lcd.print(strcat(index==0?cursor:n, "Max temp"));
    lcd.setCursor(0,1);
    lcd.print(strcat(index==1?cursor:n, "Min temp"));
    lcd.setCursor(0,2);
    lcd.print(strcat(index==2?cursor:n, "Nocturno/Diurno"));
    lcd.setCursor(0,3);
    lcd.print(strcat(index==3?cursor:n, "Back"));
}

void showHumMenu(int index, bool enter)
{
    if (enter) 
    {
      switch (index)
      { 
        case 0:
          //menuFunction = &showTemp; // aqui va la funcion del siguiente menu si pulsas en la opcion 0 de este menu
          break;
        case 1:
          //menuFunction = &showTemp;
          break;
        case 2:
          //menuFunction = &showTemp;
          break;
        case 3:
          //menuFunction = &showTemp;
          break;
      }
      (*menuFunction)(index, false);
      return;
    }
    
    char cursor[] = "=>";
    char n[] = "";
    lcd.clear();
    lcd.backlight(); 
    lcd.setCursor(0,0);
    lcd.print(strcat(index==0?cursor:n, "Max Hum"));
    lcd.setCursor(0,1);
    lcd.print(strcat(index==1?cursor:n, "Min Hum"));
    lcd.setCursor(0,2);
    lcd.print(strcat(index==2?cursor:n, "Nocturno/Diurno"));
    lcd.setCursor(0,3);
    lcd.print(strcat(index==3?cursor:n, "Back"));
}

void showComidaMenu(int index, bool enter)
{
    if (enter) 
    {
      switch (index)
      { 
        case 0:
          //menuFunction = &showTemp; // aqui va la funcion del siguiente menu si pulsas en la opcion 0 de este menu
          break;
        case 1:
          //menuFunction = &showTemp;
          break;
        case 2:
          //menuFunction = &showTemp;
          break;
        case 3:
          //menuFunction = &showTemp;
          break;
      }
      (*menuFunction)(index, false);
      return;
    }
    
    char cursor[] = "=>";
    char n[] = "";
    lcd.clear();
    lcd.backlight(); 
    lcd.setCursor(0,0);
    lcd.print(strcat(index==0?cursor:n, "N de comidas/dia"));
    lcd.setCursor(0,1);
    lcd.print(strcat(index==1?cursor:n, "Cantidad comida"));
    lcd.setCursor(0,2);
    lcd.print(strcat(index==2?cursor:n, "Twitter ON/OFF"));
    lcd.setCursor(0,3);
    lcd.print(strcat(index==3?cursor:n, "Back"));
}

void showConfigMenu(int index, bool enter)
{
    if (enter) 
    {
      switch (index)
      { 
        case 0:
          menuFunction = &showTempMenu;
          break;
        case 1:
          menuFunction = &showHumMenu;
          break;
        case 2:
          menuFunction = &showComidaMenu;
          break;
        case 3:
          menuFunction = &showMainMenu;
          break;
      }
      (*menuFunction)(index, false);
      return;
    }
    
    char cursor[] = "=>";
    char n[] = "";
    lcd.clear();
    lcd.backlight(); 
    lcd.setCursor(0,0);
    lcd.print(strcat(index==0?cursor:n, "Temp"));
    lcd.setCursor(0,1);
    lcd.print(strcat(index==1?cursor:n, "Hum"));
    lcd.setCursor(0,2);
    lcd.print(strcat(index==2?cursor:n, "Comida"));
    lcd.setCursor(0,3);
    lcd.print(strcat(index==3?cursor:n, "Back"));
}

void setup() {
  Serial.begin(9600);  // Used to type in characters
  dht.begin();
  comedero.attach(12);
  digitalWrite(HEATER, HIGH);
  comedero.write(95);
  
  pinMode(HEATER, OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(FEED, OUTPUT);
  pinMode(FLASH, OUTPUT);

  lcd.begin(20,4);// initialize the lcd for 20 chars 4 lines

  for(int i = 0; i< 2; i++)
  {
    lcd.backlight();
    delay(100);
    lcd.noBacklight();
    delay(100);
  }
  
  lcd.noBacklight(); // finish with backlight off
  digitalWrite(FLASH, LOW);
  digitalWrite(FEED, LOW);
  digitalWrite(FAN, LOW);
  digitalWrite(HEATER, LOW);
  
  menuFunction = &showMainMenu; // el primer menu es mainMenu
}


/*************/
/* Main loop */
/*************/
void loop()
{         

  // comedero
  botonComedero = 0; //analogRead(feedInterval);
  if (millis() - lastComedero > feedInterval || botonComedero > 800)
  {
    lastComedero = millis();
    feed();
    delay(180);
  }
  
  // ventiladores
  if (!ventilando && millis() - descansando > tiempoDescanso && (millis() - lastVentilacion > fanInterval || dht.readHumidity() > HUM+3))
  {
    ventilando = true;
    lastVentilacion=millis();
    digitalWrite(FAN, HIGH);
  }
  
  if (ventilando)
  {
    if (millis() - lastVentilacion > 3*minutes && dht.readHumidity() < HUM) 
    {
      ventilando = false;
      digitalWrite(FAN, LOW);
    } 
    else if (millis() - lastVentilacion > maxTempVentilando) 
    {
      ventilando = false;
      descansando = millis();
      digitalWrite(FAN, LOW);
    }
  }
  
  if (dht.readTemperature() < minTemp) {
    digitalWrite(HEATER, HIGH);
  }
  if (dht.readTemperature() > maxTemp) {
    digitalWrite(HEATER, LOW);
  }
 
  if (millis() - lastRefrescoPantalla > refrescoPantalla)
  {
    lastRefrescoPantalla = millis();  
    //lcd.noBacklight();
    //(*menuFunction)(menuIndex, false);
  }
  if (millis()-180 > KPA)
  {
    LBP=0;
    botonera = analogRead(A1);
  }
      
  if (botonera > 200 && botonera < 300)
  {
    BP=1;

    if (BP != LBP)
    {
      LBP=BP;
      KPA=millis();        
      Serial.println("UP");
      lastRefrescoPantalla = millis(); 
      menuIndex = menuIndex<1?0:menuIndex-1;
      (*menuFunction)(menuIndex, false);
    }
  }
      
  if (botonera > 450 && botonera < 600)
  {
    BP=2;

    if (BP != LBP)
    {
      LBP=BP;
      KPA=millis();        
      Serial.println("DOWN"); 
      lastRefrescoPantalla = millis(); 
      menuIndex = menuIndex>3?0:menuIndex+1;
      (*menuFunction)(menuIndex, false);
    }
  }
    
  if (botonera > 700 && botonera < 900)
  {
    BP=3;

    if (BP != LBP)
    {
      LBP=BP;
      KPA=millis()+30;        
      Serial.println("ENTER");
      lastRefrescoPantalla = millis(); 
      menuIndex = 0;
      (*menuFunction)(menuIndex, true);
    }
  }
}
