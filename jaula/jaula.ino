#include <Wire.h>
#include "DHT.h"
#include <Servo.h> 
#include <LiquidCrystal_I2C.h>
 
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
  
  
  
   //if (millis() - lastRefrescoBotonera > refrescoBotonera) 
 /*  {
     lastRefrescoBotonera = millis();
     
      botonera = analogRead(A1);
  */  //  delay(1);
      //Serial.println(botonera);
      
      
         if (millis()-180 > KPA)
        {
        LBP=0;
        botonera = analogRead(A1);  }
      
      if (botonera > 200 && botonera < 300)
      {
        BP=1;
        
          if (BP != LBP)
          {
          LBP=BP;
          KPA=millis();        
          Serial.println("UP");  
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
          }
      }
   //}
   

    
}
