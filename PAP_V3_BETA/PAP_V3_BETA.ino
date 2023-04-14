

/*
Data logger, recuperación de Variables climatológicas

Author: Tsípini Franco Valencia
Version 3.0 BETA

-SD Card Fixed
-DHT22 Fixed
-Soil Fixed
-.csv - Working
-RTC - Need to be on time

On coming on next version:
-LCD
-Buttons
-Battery Level

MEnu

*/
/*
  SD card datalogger

  The circuit:
 ** SDO - pin 11
 ** SDI - pin 12
 ** CLK - pin 13
 ** CS - pin 10

A part of SD code, take reference from: Tom Igoe, example code is in the public domain.
  created  24 Nov 2010
  modified  24 July 2020

*/

#include "DHT.h"      /*Library for the DTH humidity and temperature sensor*/
#include <Wire.h>     /*Library for comunication with RTC module*/
#include "ds3231.h"   /*Library for DS3231 RTC module*/
#include <SPI.h>      /*Library for SPI SD Card Reader*/
#include <SD.h>       /*Library for SD Card Reader*/

int loop_flag = 0;
int time_flag = 0;

/*SD*/
const int chipSelect = 10;   //SD CS pin

File myFile;

/*RTC*/
#define BUFF_MAX 128
uint8_t time[8];
char recv[BUFF_MAX];
unsigned int recv_size = 0;
unsigned long prev, interval = 5000;

/*DHT22*/

// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 2
// Dependiendo del tipo de sensor
#define DHTTYPE DHT22
// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

float* humedad = nullptr;
float* temperatura = nullptr;


/*YL-69 Soil humidity sensor*/
int SoilPin = A0;
volatile int SoilMoisture = 0;


/*System Setup*/
void setup() {
   Serial.begin(9600);                       //Open serial
pinMode(chipSelect, OUTPUT);
  pinMode(SoilPin, INPUT);      //Soil sensor Pin config
  dht.begin();
  
 
  Serial.println("DHT init OK");
  Serial.println("SERIAL init OK");
  //while (!Serial);                          //Wait for Serial Monitor to connect


   Wire.begin();                            //RTC comunication init
   delay(100);
   Serial.println("RTC COM OK");              //Print on serial
   DS3231_init(DS3231_CONTROL_INTCN);       //RTC init
   delay(100);
   Serial.println("RTC init OK");              //Print on serial


  while (!SD.begin(chipSelect)) {
    Serial.println(" SD failed.");
    delay(1000);
    //while (true);
  }

  Serial.println("SD initialization OK.");
  
myFile = SD.open("datalog.txt", FILE_WRITE);
delay(100);
if (myFile) {
  
  delay(100);
  Serial.println("File Open OK");
}
else
{
   Serial.println("Error Opening File.txt");
}
myFile.close();

Serial.println("Setup OK");
}

                  /*Main program Super loop*/
void loop() {


if(0 == loop_flag){
Serial.println("Loop BEGIN");
loop_flag = 1;
}    

    char buff[BUFF_MAX];
    unsigned long now = millis();
    struct ts t;

DS3231_get(&t);
snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d", t.year,t.mon, t.mday, t.hour, t.min, t.sec);



if(0 == time_flag){
Serial.println("Time OK");
time_flag = 1;
}
/*DHT22 Measure*/

// Leemos la humedad relativa
  float hum = dht.readHumidity();
  float tmp = dht.readTemperature();
  /*Leemos la temperatura en grados centígrados (por defecto)*/

humedad = &hum;
temperatura = &tmp;

/* 
  Serial.print("Humedad: ");
  Serial.print(hum);
   Serial.print(" Temperatura: ");
  Serial.println(tmp); 
*/

delay(1000);

/*Soil Measure*/

  SoilMoisture=analogRead(SoilPin);
  SoilMoisture=(SoilMoisture-1020)/-6.8;

//String for assembling for data log:
    String dataString = buff;
           dataString += ", ";
           dataString += String(*temperatura);
           dataString += ", ";
           dataString += String(*humedad);
           dataString += ", ";
           dataString += String(SoilMoisture);

Serial.println(myFile);




myFile = SD.open("datalog.txt", FILE_WRITE);

if (myFile) {
Serial.println("Open success");
myFile.println(dataString);
Serial.println(dataString);
delay(5000);
}
else{
  Serial.println("Error opening file.txt");
}
myFile.close();
/*
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
*/

  
}
