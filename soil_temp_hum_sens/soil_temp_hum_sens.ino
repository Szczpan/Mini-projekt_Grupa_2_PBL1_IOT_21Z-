#include "DHT.h"
#include <math.h>

const int soil_moist_sens = A0;
const int hum_temp_sens = 2;
int minADC = 0;                       // replace with min ADC value read in air
int maxADC = 600;                     // replace with max ADC value read fully submerged in water
bool is1V1Output = false;             // obczaj czy ta wersja co mamy sensora gleby ma 1.1 czy 3V na outpucie 
int moistureValue, mappedValue;
int U = 10;                           // prędkość wiatru, ale nie mamy sensora prędkości wiatru, więc ustawiam na średnią prędkość wiatru w Polsce
float DF, H, T, FFDI;                   // Drought Factor, Humidity, Temperature, Forest Fire Danger Index
const int red = 3, yellow = 4, green = 5; // digital pins for LEDs
const int MQ2 = A1;
float MQ2value;
int MQ2_trigger_value = 800;

DHT dht(hum_temp_sens, DHT22);

void setup() 
{
  Serial.begin(9600);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  delay(20000);
}

void loop() 
{
  MQ2value = analogRead(MQ2);
  Serial.println(MQ2value);
  if(MQ2value >= MQ2_trigger_value)
  {
    digitalWrite(green, 0);
    digitalWrite(yellow, 0);
    digitalWrite(red, 0);
    delay(500); 
  }
  if(dht.read() == DHT_OK)
  {
    moistureValue = analogRead(soil_moist_sens);
    DF = map(moistureValue, minADC, maxADC, 10, 0);
    H = dht.humidity;
    T = dht.temperature;
    FFDI = 2 * exp( -0.45 + 0.987 * log( DF ) + 0.0338*T - 0.0345*H + 0.0234*U );
    Serial.println(FFDI);
    if(FFDI <= 5)  // Fire Danger Classification based on Table 3 from "Environmental Modelling & Software 24 (2009) 764–774" J.J. Sharples, R.H.D. McRae, R.O. Weber, A.M. Gill
    {
      digitalWrite(green, 1);
      digitalWrite(yellow, 0);
      digitalWrite(red, 0);
    }
    else if( FFDI <= 12 )
    {
      digitalWrite(green, 1);
      digitalWrite(yellow, 1);
      digitalWrite(red, 0);
    }
    else if( FFDI <= 24 )
    {
      digitalWrite(green, 0);
      digitalWrite(yellow, 1);
      digitalWrite(red, 0);
    }
    else if( FFDI <= 50 )
    {
      digitalWrite(green, 0);
      digitalWrite(yellow, 1);
      digitalWrite(red, 1);
    }
    else if( FFDI <= 100 )
    {
      digitalWrite(green, 0);
      digitalWrite(yellow, 0);
      digitalWrite(red, 1);
    }
  }
  delay(2000);
}
