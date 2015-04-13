#include <Wire.h>
#include "DHT.h"
#include <Servo.h> 
#include <LiquidCrystal_I2C.h>
#include "pitches.h"

 
#define DHTPIN 8
#define FEEDBUTTON A0
#define HEATER 9
#define FAN 10
#define FEED 11
#define FLASH 13

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


const long refrescoPantalla = 1*seconds;
const long refrescoBotonera = 200;
long lastRefrescoPantalla = 0;
long lastRefrescoBotonera = 0;

Servo comedero;

int botonComedero= 0;
int botonera = 0;

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


void setup() {
  Serial.begin(9600);  // Used to type in characters
  dht.begin();
  comedero.attach(12);
  
  pinMode(FEEDBUTTON, INPUT);
  pinMode(botonera, INPUT);
  
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
}


/*************/
/* Main loop */
/*************/
void loop()
{         

  // comedero
  botonComedero = analogRead(feedInterval);
  if (millis() - lastComedero > feedInterval || botonComedero > 800)
  {
    lastComedero = millis();
    feed();
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
 
  
//  Serial.print(dht.readHumidity());
//  Serial.print("   ");
//  delay(30);
//  Serial.println(dht.readTemperature())

  
  if (dht.readTemperature() < minTemp) {
    digitalWrite(HEATER, HIGH);
  }
  if (dht.readTemperature() > maxTemp) {
    digitalWrite(HEATER, LOW);
  }
 
  if (millis() - lastRefrescoPantalla > refrescoPantalla)
  {
    lastRefrescoPantalla = millis();  
    lcd.backlight();
    lcd.clear();  
    lcd.setCursor(0,0);
    lcd.print("Temperatura:");
    delay(1);
    lcd.setCursor(0,1);
    lcd.print(dht.readTemperature());
    delay(1);
    lcd.setCursor(0,2);
    lcd.print("Humedad:");
    lcd.setCursor(9,2);
    //lcd.print(modo[indice]);
    lcd.print("modo");
    lcd.setCursor(0,3);
    delay(1);
    lcd.print(dht.readHumidity());
    delay(1);
  }
  
  
  
   if (millis() - lastRefrescoBotonera > refrescoBotonera) 
   {
     lastRefrescoBotonera = millis();
     
      botonera = analogRead(A1);
      delay(1);
      //Serial.println(botonera);
      
      if (botonera > 200 && botonera < 300)
      {
        Serial.println("UP");  
      }
      
      if (botonera > 450 && botonera < 600)
      {
        Serial.println("DOWN");
      }
      
      if (botonera > 700 && botonera < 900)
      {
        Serial.println("ENTER");
      }
   }
  
/*
  


 #include "pitches.h"

// notes in the melody:

int led = 13;
int piloto = 3;

int melody[] = {
  NOTE_A7,0, NOTE_A7,0,NOTE_A7,0,NOTE_A7,0,NOTE_A7,0,NOTE_A7,0,NOTE_A7,0};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
   4, 2, 4, 2, 4, 8, 4, 8, 4, 8, 4, 8, 2, 4 };

void setup() {
  
  pinMode(led,OUTPUT);
  pinMode(piloto,OUTPUT);
  Serial.begin(9600);
  
  // iterate over the notes of the melody:

}





void loop () {
  if (Serial.available()) { //Si está disponible
    char c = Serial.read(); //Guardamos la lectura en una variable char
    if (c == 'H') { //Si es una 'H', enciendo el LED
      
        for (int thisNote = 0; thisNote < 14; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(8, melody[thisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
    
  }
      digitalWrite(led, HIGH);
    } 
    if (c == 'L') { //Si es una 'L', apago el LED
      digitalWrite(led, LOW);
    }
    if (c == 'W') { //Si es una 'W', enciendo calefaccion y piloto indicativo
      digitalWrite(piloto, HIGH);
    }
    if (c == 'S') { //Si es una 'S', apago la calefaccion
      digitalWrite(piloto, LOW);
    }
  }
}

  
  
 */   
}
