#include <Wire.h>
#include "DHT.h"
#include <Servo.h> 
 
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
const long hours = 3.6e6;
const long minutes = 60000;
const long seconds = 1000;

const long COMEDERO = 12*hours;
const long VENTILACION = 3*hours;
long apagarVentilacion = 0; 
long lastComedero = 0;
long lastVentilacion = VENTILACION;
Servo comedero;

void setup() {
  Serial.begin(9600);  // Used to type in characters
  dht.begin();
  comedero.attach(12);
  
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);

  digitalWrite(13, LOW);
}

void loop()
{
  
  int botonComedero = analogRead(A0);
  if (botonComedero > 800)
  {
    lastComedero = millis();
    digitalWrite(11, HIGH);
    for (int i=95; i > 30;i-=15) {
      comedero.write(i);
      delay(5);
    }
    for (int i=29; i <= 95 ;i+=15) {
      comedero.write(i);
      delay(5);
    }
    delay(100);
    digitalWrite(11, LOW);
  }
         

  if (millis() - lastComedero > COMEDERO)
  {
    lastComedero = millis();
    digitalWrite(11, HIGH);
    for (int i=95; i > 30;i-=15) {
      comedero.write(i);
      delay(5);
    }
    for (int i=29; i <= 95 ;i+=15) {
      comedero.write(i);
      delay(5);
    }
    delay(100);
    digitalWrite(11, LOW);
  }
  
  if (millis() - lastVentilacion > VENTILACION)
  {
    lastVentilacion=millis();
    digitalWrite(10, HIGH);
    apagarVentilacion = millis() + 3*minutes;
  }
  
  if (millis() > apagarVentilacion)
  {
    digitalWrite(10, LOW);
  }
  
  Serial.print(dht.readHumidity());
  Serial.print("   ");
//  delay(30);
  Serial.println(dht.readTemperature());
//  delay(2000);
}
