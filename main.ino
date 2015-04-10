#include <Wire.h>
#include "DHT.h"
#include <Servo.h> 
 
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
const long hours = 3.6e6;
const long minutes = 60000;
const long seconds = 1000;

const int minTemp = 19;
const int maxTemp = 23;
const int HUM = 62;

const long COMEDERO = 12*hours;
const long VENTILACION = 3*hours;
long apagarVentilacion = 0; 
long lastComedero = 0;
long lastVentilacion = VENTILACION;
Servo comedero;

int CALORET=9;
int FANS=10;
int ZAMPA=11;
int FLASH=13;

#include <LiquidCrystal_I2C.h>


//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address


void setup() {
  

  
  Serial.begin(9600);  // Used to type in characters
  dht.begin();
  comedero.attach(12);
  
  pinMode(CALORET, OUTPUT);
  pinMode(FANS, OUTPUT);
  pinMode(ZAMPA, OUTPUT);
  pinMode(FLASH, OUTPUT);



lcd.begin(20,4);         // initialize the lcd for 20 chars 4 lines, turn on backlight


  for(int i = 0; i< 3; i++)
  {
    lcd.backlight();
    delay(250);
    lcd.noBacklight();
    delay(250);
  }
  lcd.backlight(); // finish with backlight on

//-------- Write characters on the display ------------------

  lcd.setCursor(0,0);
  lcd.print("Hola, perrito!");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("Esto es una prueba");
  delay(1000);
  lcd.setCursor(0,2);
  lcd.print("de la pantallita");
  lcd.setCursor(0,3);
  delay(2000);
  lcd.print("Yupi?");
  delay(8000);
  lcd.clear();

  digitalWrite(FLASH, LOW);
}

void loop()
{
  
  
  
  int botonComedero = analogRead(A0);
  if (botonComedero > 800)
  {
    lastComedero = millis();
    digitalWrite(ZAMPA, HIGH);
    for (int i=95; i > 30;i-=15) {
      comedero.write(i);
      delay(5);
    }
    for (int i=29; i <= 95 ;i+=15) {
      comedero.write(i);
      delay(5);
    }
    delay(100);
    digitalWrite(ZAMPA, LOW);
  }
         

  if (millis() - lastComedero > COMEDERO)
  {
    lastComedero = millis();
    digitalWrite(ZAMPA, HIGH);
    for (int i=95; i > 30;i-=15) {
      comedero.write(i);
      delay(5);
    }
    for (int i=29; i <= 95 ;i+=15) {
      comedero.write(i);
      delay(5);
    }
    delay(100);
    digitalWrite(ZAMPA, LOW);
  }
  
  if (millis() - lastVentilacion > VENTILACION)
  {
    lastVentilacion=millis();
    digitalWrite(FANS, HIGH);
    apagarVentilacion = millis() + 3*minutes;
  }
  
 /* if (millis() > apagarVentilacion)
  {
    digitalWrite(FANS, LOW);
  }*/
  
  Serial.print(dht.readHumidity());
  Serial.print("   ");
//  delay(30);
  Serial.println(dht.readTemperature());
 
     if (dht.readHumidity() > HUM + 2) {
  digitalWrite(FANS, HIGH);
  }
 
      if (dht.readHumidity() < HUM ) {
  digitalWrite(FANS, LOW);
  }
  
    if (dht.readTemperature() < minTemp) {
  digitalWrite(CALORET, HIGH);
  }
    if (dht.readTemperature() > maxTemp) {
  digitalWrite(CALORET, LOW);
  }
  
//  delay(2000);
}
