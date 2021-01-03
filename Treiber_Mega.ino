//Check der SDkarte an richtiger Stelle
//Befehl manuelle Regeneration
//anpassen werteausgabe (SD-karte)
//Fehlerausgabe Motorfunktion
//anpassen Relaisschaltung elektrolyse
//Meldung über erfolgreiches Speichern + Namenseingabe
//Zeitrigger und weitere autolog Funktionen
//verschiedene Ordner für verschiedene Messfunktionen

#include <SoftwareSerial.h>
#include "RTClib.h"
#include <Wire.h>
#include <INA219_WE.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>

#define L635 30
#define L595 31
#define L520 32
#define L490 33
#define L445 34
#define L430 35
#define L390 36
#define L275 37
#define L281 22
#define L365 23
#define S1Z1 24
#define S2Z1 25
#define S1Z2 26
#define S2Z2 27
#define SIna 28
#define SGuva 29
#define A 38 //1N1
#define B 39 //1N2
#define C 40 //1N3
#define D 41 //1N4
#define S1 5
#define S2 6
#define bus 4

RTC_DS3231 rtc;
INA219_WE ina1(0x40);
INA219_WE ina2(0x41);
OneWire oneWire(bus);
DallasTemperature sensors(&oneWire);
DeviceAddress tc;
File Data;

int steps = 1000;  //Schriette pro Position
float vol = 0.25; //Volumenverhältnis innen/außen

boolean dataErr;
boolean waterErr;
boolean exitE;
boolean lightTest;
boolean ok = 0;  //Wenn Fehler war --> ok = 0 falls neuinitialisierung nötig!
boolean error = 0;
boolean start;
boolean meas;
boolean sw1;
boolean sw2;
boolean cal = 0;
boolean spec = 0;
int currentPos = 0;
int sollPos;
int currentStep = 1;
int i;
int j;
int index;
int lnge;
int timeE;
float CM0;
float Cu0;
float FeII0;
float FeIII0;
float Ca0;
float Mg0;
float Nitrat0;
float Nitrit0;
float CM1;
float Cu1;
float FeII1;
float FeIII1;
float Ca1;
float Mg1;
float Nitrat1;
float Nitrit1;
float light;
float light0;
float I1;
float I2;
float I3;
float I4;
float I5;
float I6;
float I7;
float I8;
float temp;
float conz;
double timeS1;
double timeS2;
unsigned long time0;
unsigned long time1;
unsigned long time2;
unsigned long time3;
volatile unsigned long counter1 = 0;
volatile unsigned long counter2 = 0;
String input;
String fileName;
String output;

void count1() {
  counter1 += 1;
}

void count2() {
  counter2 += 1;
}

void motor() {  //Fehlerausgabe bei Endpunkttouch!
  if (sollPos == 0) {
    Serial.println("sollpos0");
    sw1 = digitalRead(S1);
    Serial.println(sw1);
    while (sw1 == 1) {  //0 oder 1?
      if (currentStep == 1) {
        digitalWrite(A,1);
        digitalWrite(B,0);
        digitalWrite(C,0);
        digitalWrite(D,0);
        currentStep = 2;
        delay(1);
      } else if (currentStep == 2) {
        digitalWrite(A,1);
        digitalWrite(B,1);
        digitalWrite(C,0);
        digitalWrite(D,0);
        currentStep = 3;
        delay(1);
      } else if (currentStep == 3) {
        digitalWrite(A,0);
        digitalWrite(B,1);
        digitalWrite(C,0);
        digitalWrite(D,0);
        currentStep = 4;
        delay(1);
      } else if (currentStep == 4) {
        digitalWrite(A,0);
        digitalWrite(B,1);
        digitalWrite(C,1);
        digitalWrite(D,0);  
        currentStep = 5;
        delay(1);
      } else if (currentStep == 5) {
        digitalWrite(A,0);
        digitalWrite(B,0);
        digitalWrite(C,1);
        digitalWrite(D,0);  
        currentStep = 6;
        delay(1);
      } else if (currentStep == 6) {
        digitalWrite(A,0);
        digitalWrite(B,0);
        digitalWrite(C,1);
        digitalWrite(D,1);  
        currentStep = 7;
        delay(1);
      } else if (currentStep == 7) {
        digitalWrite(A,0);
        digitalWrite(B,0);
        digitalWrite(C,0);
        digitalWrite(D,1);  
        currentStep = 8;
        delay(1);
      } else if (currentStep == 8) {
        digitalWrite(A,1);
        digitalWrite(B,0);
        digitalWrite(C,0);
        digitalWrite(D,1);  
        currentStep = 1;
        delay(1);
      } 
      sw1 = digitalRead(S1);
      Serial.println(sw1);
    }
    digitalWrite(A,0);
    digitalWrite(B,0);
    digitalWrite(C,0);
    digitalWrite(D,0); 
    currentStep = 1; 
    currentPos = 0;
  } 
  if (sollPos < currentPos) {
    sw1 = digitalRead(S1);
    sw2 = digitalRead(S2);
    while (((sw1 == 1) && (sw2 == 1)) && (sollPos != currentPos)) {  //0 oder 1?
      currentPos -= 1;
      //Serial.println(currentPos); //debug
      for (i=0; i<steps; i++) {
        if (currentStep == 1) {
          digitalWrite(A,1);
          digitalWrite(B,0);
          digitalWrite(C,0);
          digitalWrite(D,0);
          currentStep = 2;
          delay(1);
        } else if (currentStep == 2) {
          digitalWrite(A,1);
          digitalWrite(B,1);
          digitalWrite(C,0);
          digitalWrite(D,0);
          currentStep = 3;
          delay(1);
        } else if (currentStep == 3) {
          digitalWrite(A,0);
          digitalWrite(B,1);
          digitalWrite(C,0);
          digitalWrite(D,0);
          currentStep = 4;
          delay(1);
        } else if (currentStep == 4) {
          digitalWrite(A,0);
          digitalWrite(B,1);
          digitalWrite(C,1);
          digitalWrite(D,0);  
          currentStep = 5;
          delay(1);
        } else if (currentStep == 5) {
          digitalWrite(A,0);
          digitalWrite(B,0);
          digitalWrite(C,1);
          digitalWrite(D,0);  
          currentStep = 6;
          delay(1);
        } else if (currentStep == 6) {
          digitalWrite(A,0);
          digitalWrite(B,0);
          digitalWrite(C,1);
          digitalWrite(D,1);  
          currentStep = 7;
          delay(1);
        } else if (currentStep == 7) {
          digitalWrite(A,0);
          digitalWrite(B,0);
          digitalWrite(C,0);
          digitalWrite(D,1);  
          currentStep = 8;
          delay(1);
        } else if (currentStep == 8) {
          digitalWrite(A,1);
          digitalWrite(B,0);
          digitalWrite(C,0);
          digitalWrite(D,1);  
          currentStep = 1;
          delay(1);
        }
      }  
      sw1 = digitalRead(S1);
      sw2 = digitalRead(S2);
    }
    digitalWrite(A,0);
    digitalWrite(B,0);
    digitalWrite(C,0);
    digitalWrite(D,0); 
    currentStep = 1; 
  }
  if (sollPos > currentPos) {
    sw1 = digitalRead(S1);
    sw2 = digitalRead(S2);
    //Serial.println("moin"); //debug
    while (((sw1 == 1) && (sw2 == 1)) && (sollPos != currentPos)) {  //0 oder 1?
      currentPos += 1;
      Serial.println(currentPos);  //debug
      for (i=0; i<steps; i++) {
        if (currentStep == 1) {
          digitalWrite(A,1);
          digitalWrite(B,0);
          digitalWrite(C,0);
          digitalWrite(D,0);
          currentStep = 2;
          delay(1);
        } else if (currentStep == 2) {
          digitalWrite(A,1);
          digitalWrite(B,0);
          digitalWrite(C,0);
          digitalWrite(D,1);
          currentStep = 3;
          delay(1);
        } else if (currentStep == 3) {
          digitalWrite(A,0);
          digitalWrite(B,0);
          digitalWrite(C,0);
          digitalWrite(D,1);
          currentStep = 4;
          delay(1);
        } else if (currentStep == 4) {
          digitalWrite(A,0);
          digitalWrite(B,0);
          digitalWrite(C,1);
          digitalWrite(D,1);  
          currentStep = 5;
          delay(1);
        } else if (currentStep == 5) {
          digitalWrite(A,0);
          digitalWrite(B,0);
          digitalWrite(C,1);
          digitalWrite(D,0);  
          currentStep = 6;
          delay(1);
        } else if (currentStep == 6) {
          digitalWrite(A,0);
          digitalWrite(B,1);
          digitalWrite(C,1);
          digitalWrite(D,0);  
          currentStep = 7;
          delay(1);
        } else if (currentStep == 7) {
          digitalWrite(A,0);
          digitalWrite(B,1);
          digitalWrite(C,0);
          digitalWrite(D,0);  
          currentStep = 8;
          delay(1);
        } else if (currentStep == 8) {
          digitalWrite(A,1);
          digitalWrite(B,1);
          digitalWrite(C,0);
          digitalWrite(D,0);  
          currentStep = 1;
          delay(1);
        }
      }  
      sw1 = digitalRead(S1);
      sw2 = digitalRead(S2);
    }
    digitalWrite(A,0);
    digitalWrite(B,0);
    digitalWrite(C,0);
    digitalWrite(D,0);  
    currentStep = 1;
  }    
}

void setup() {
  Serial.begin(9600);  //debug
  Serial1.begin(38400);
  //Bibliotheken etc -->andere codes
  Wire.begin();
  attachInterrupt(0,count1,RISING);
  attachInterrupt(1,count2,RISING);
  pinMode(L635,OUTPUT);
  pinMode(L595,OUTPUT);
  pinMode(L520,OUTPUT);
  pinMode(L490,OUTPUT);
  pinMode(L445,OUTPUT);
  pinMode(L430,OUTPUT);
  pinMode(L390,OUTPUT);
  pinMode(L275,OUTPUT);
  pinMode(L281,OUTPUT);
  pinMode(L365,OUTPUT);
  pinMode(S1Z1,OUTPUT);
  pinMode(S2Z1,OUTPUT);
  pinMode(S1Z2,OUTPUT);
  pinMode(S2Z2,OUTPUT);
  pinMode(SIna,OUTPUT);
  pinMode(SGuva,OUTPUT);
  pinMode(A,OUTPUT);
  pinMode(B,OUTPUT);
  pinMode(C,OUTPUT);
  pinMode(D,OUTPUT);
  pinMode(S1,INPUT_PULLUP);
  pinMode(S2,INPUT_PULLUP);
  digitalWrite(L635,1);
  digitalWrite(L595,1);
  digitalWrite(L520,1);
  digitalWrite(L490,1);
  digitalWrite(L445,1);
  digitalWrite(L430,1);
  digitalWrite(L390,1);
  digitalWrite(L275,1);
  digitalWrite(L281,1);
  digitalWrite(L365,1);
  digitalWrite(S1Z1,1);
  digitalWrite(S2Z1,1);
  digitalWrite(S1Z2,1);
  digitalWrite(S2Z2,1);
  digitalWrite(SIna,1);
  digitalWrite(SGuva,1);
}

void loop() {
  if (Serial1.available()) {
    input = Serial1.readString();
    if (input.startsWith("init")) {
      start = 1;
    } else if (input.startsWith("cal")) {
      if (ok == 1) {
        cal = 1;  
      } else {
        Serial1.println("Fehler, keine Initialisierung");
      }
    } else if (input.startsWith("spec_")) {
      if (ok == 1){
        spec = 1;
        input.remove(0,5);
        fileName = input;
        index = fileName.indexOf("_");
        lnge = fileName.length();
        lnge = lnge - index;  //werte korrigieren
        fileName.remove(index,lnge);
        Serial.println(input);
        index += 1;
        input.remove(0,index);
        Serial.println(input);
        conz = float(input.toInt());  //anpassen für Dezimalwerte
        if (((fileName.indexOf(".") != -1) || (fileName.indexOf("/") != -1)) || (fileName.indexOf("txt") != -1)) {  //evtl. erweitern
          spec = 0;
          Serial1.println("Fehler, verbotenes Zeichen gefunden");
        } 
        if (fileName.length() > 8) {
          spec = 0;
          Serial1.println("Fehler, maximal 8 Zeichen erlaubt");
        }
        if (spec == 1) {
          output = "meas/" + fileName;
          output = output + ".txt";
          if (SD.exists(output)) {
            spec = 0;
            Serial1.println("Fehler, Dateiname bereits vergeben");
          }
        }   
      } else {
        Serial1.println("Fehler, keine Initialisierung!"); 
      }
    } else if (input.startsWith("meas_")) {
      if (ok == 1) {  
        meas = 1;
        input.remove(0,5);
        fileName = input;
        if (((fileName.indexOf(".") != -1) || (fileName.indexOf("/") != -1)) || (fileName.indexOf("txt") != -1)) {  //evtl. erweitern
          meas = 0;
          Serial1.println("Fehler, verbotenes Zeichen gefunden");
        } 
        if (fileName.length() > 8) {
          meas = 0;
          Serial1.println("Fehler, maximal 8 Zeichen erlaubt");
        }
        if (meas == 1) {
          output = "meas/" + fileName;
          output = output + ".txt";
          if (SD.exists(output)) {
            meas = 0;
            Serial1.println("Fehler, Dateiname bereits vergeben");
          }
        }
      } else {
        Serial1.println("Fehler, keine Initialisierung!"); 
      }
    }
  }
  if (spec == 1) {
    spec = 0;
    error = 0;
    waterErr = 0;
    Serial.println("Kontrolle Kalibrierung");
    if (!SD.exists("arduino/water/i1.txt")) {
      waterErr = 1;
    }
    if (!SD.exists("arduino/water/i2.txt")) {
      waterErr = 1;
    }
    if (!SD.exists("arduino/water/i3.txt")) {
      waterErr = 1;
    }
    if (!SD.exists("arduino/water/i4.txt")) {
      waterErr = 1;
    }
    if (!SD.exists("arduino/water/i5.txt")) {
      waterErr = 1;
    }
    if (!SD.exists("arduino/water/i6.txt")) {
      waterErr = 1;
    }
    if (!SD.exists("arduino/water/i7.txt")) {
      waterErr = 1;
    }
    if (!SD.exists("arduino/water/i8.txt")) {
      waterErr = 1;
    }
    if (waterErr == 1) {
      error = 1;
      Serial1.println("Wasserreferenz nicht gemessen");
    }
    if (error == 0) {
      output = "Start Messung '" + fileName;
      output = output + "'";
      Serial1.println(output);
      for (j=0; j<8; j++) {
        Serial.print(j);
        if (j == 0) {  //motor!! --> auch wieder zurück! Errorausgabe motor
          sollPos = 1;
          motor();
          digitalWrite(L635,0);
        } else if (j == 1) {
          sollPos = 2;
          motor();
          digitalWrite(L595,0);
        } else if (j == 2) {
          sollPos = 3;
          motor();
          digitalWrite(L520,0);
        } else if (j == 3) {
          sollPos = 4;
          motor();
          digitalWrite(L445,0);
        } else if (j == 4) {
          sollPos = 5;
          motor();
          digitalWrite(L390,0);
        } else if (j == 5) {
          sollPos = 6;
          motor();
          digitalWrite(L365,0);
        } else if (j == 6) {
          sollPos = 7;
          motor();
          digitalWrite(L365,0);
        } else if (j == 7) {
          sollPos = 8;
          motor();
          digitalWrite(L281,0);
        }
        delay(500);  //Schaltzeit Relais, anpassen (auch unten)
        if (j < 6) {
          time0 = millis();
          counter1 = 0;
          delay(500);
          light = counter1;
          time1 = millis();
          light = light / (float(time1 - time0) / 1000);      
        } else {
          light = ina2.getBusVoltage_V();
        }
        if (j == 0) {  //motor!! --> auch wieder zurück! Errorausgabe motor
          I1 = light;
          digitalWrite(L635,1);
        } else if (j == 1) {
          I2 = light;
          digitalWrite(L595,1);
        } else if (j == 2) {
          I3 = light;
          digitalWrite(L520,1);
        } else if (j == 3) {
          I4 = light;
          digitalWrite(L445,1);
        } else if (j == 4) {
          I5 = light;
          digitalWrite(L390,1);
        } else if (j == 5) {
          I6 = light;
          digitalWrite(L365,1);
        } else if (j == 6) {
          I7 = light;
          digitalWrite(L365,1);
        } else if (j == 7) {
          I8 = light;
          digitalWrite(L281,1);
        }
        if (error == 1) {
          Serial1.println("Motorfehler");
          j = 10;
        }
        delay(100);
      }
      if (error == 0) {
        Serial1.println("Messung abgeschlossen");
        Serial.println("moiiin");
      } else {
        Serial1.println("Messung abgebrochen");
      }
      Serial.println("Motor");
      sollPos = 0;
      motor(); 
    }
    if (error == 0) {
      Serial1.println("Berechnung Ergebnis");
      
      Data = SD.open(F("arduino/water/i1.txt"));  //Berechnung Absorbanz aus Intensität
      input = Data.readString();
      I1 = log(float(input.toInt()) / I8);
      Data.close();
      Data = SD.open(F("arduino/water/i2.txt"));
      input = Data.readString();
      I2 = log(float(input.toInt()) / I2);
      Data.close();
      Data = SD.open(F("arduino/water/i3.txt"));
      input = Data.readString();
      I3 = log(float(input.toInt()) / I3);
      Data.close();
      Data = SD.open(F("arduino/water/i4.txt"));
      input = Data.readString();
      I4 = log(float(input.toInt()) / I4);
      Data.close();
      Data = SD.open(F("arduino/water/i5.txt"));
      input = Data.readString();
      I5 = log(float(input.toInt()) / I5);
      Data.close();
      Data = SD.open(F("arduino/water/i6.txt"));
      input = Data.readString();
      I6 = log(float(input.toInt()) / I6);
      Data.close();
      Data = SD.open(F("arduino/water/i7.txt"));
      input = Data.readString();
      I7 = log(float(input.toInt()) / I7);
      Data.close();
      Data = SD.open(F("arduino/water/i8.txt"));
      input = Data.readString();
      I8 = log(float(input.toInt()) / I8);
      Data.close();

      temp = sensors.getTempC(tc);
      Serial1.println("Messergebnisse:");
      Serial1.print("   Temperatur [°C]: ");
      Serial1.println(temp);
      Serial1.print("   Konzentration [mol/l]: ");
      Serial1.println(conz);   
      Serial1.println("   Absorbanzen (Kationen):"); //evtl. mmol/l
      Serial1.print("       635nm: ");
      Serial1.println(I1);
      Serial1.print("       595nm: ");
      Serial1.println(I2);
      Serial1.print("       520nm: ");
      Serial1.println(I3);
      Serial1.print("       445nm: ");
      Serial1.println(I4);
      Serial1.print("       390nm: ");
      Serial1.println(I5);  //Härtegrad/Temperatur
      Serial1.print("       365nm: ");  //angabe anpassen
      Serial1.println(I6);
      Serial1.println("   Absorbanzen (Anionen):"); //evtl. mmol/l
      Serial1.print("       365nm: ");
      Serial1.println(I7);
      Serial1.print("       281nm: ");
      Serial1.println(I8);
      if (!SD.exists("meas")) {
        SD.mkdir("meas");
        Serial.println("nicht vorhanden");
      } else {  //debug
        Serial.println("vorhanden");
      }
      output = "meas/" + fileName;
      output = output + ".txt";
      Serial.println(output);
      Data = SD.open(output,FILE_WRITE);
      DateTime now = rtc.now();
      Data.print("Datum: ");
      Data.print(now.day());
      Data.print(".");
      Data.print(now.month());
      Data.print(".");
      Data.println(now.year());
      Data.print("Zeit: ");
      Data.print(now.hour());
      Data.print(":");
      Data.print(now.minute());
      Data.print(":");
      Data.println(now.second());
      Data.println("Messergebnisse:");
      Data.print("  Temperatur [°C]: ");
      Data.println(temp);
      Data.print("  Konzentration [mol/l]: ");
      Data.println(conz);
      Data.println("  Absorbanz (Kationen):"); //evtl. mmol/l
      Data.print("    635nm: ");
      Data.println(I1);
      Data.print("    595nm: ");
      Data.println(I2);
      Data.print("    520nm: ");
      Data.println(I3);
      Data.print("    445nm: ");
      Data.println(I4);
      Data.print("    390nm: ");
      Data.println(I5);  //Härtegrad/Temperatur
      Data.print("    365nm: ");
      Data.println(I6);
      Data.println("  Absorbanz (Anionen):"); //evtl. mmol/l
      Data.print("    365nm: ");
      Data.println(I7);
      Data.print("    281nm: ");
      Data.println(I8);
      Data.close();
      output = "Messung als '";
      output = output + fileName;
      output = output + "' gespeichert";
      Serial1.println(output);
    }
///////////777777777777777777777777777777777777777777777777777Berechnung koeffizienten
    if (error == 1) {
      Serial1.println("Messung abgebrochen");
    }
  }
  if (cal == 1) {
    cal = 0;
    Serial1.println("messe Wasserreferenz");
    for (j=0; j<8; j++) {
      Serial.print(j);
      if (j == 0) {  //motor!! --> auch wieder zurück! Errorausgabe motor
        sollPos = 1;
        motor();
        digitalWrite(L635,0);
      } else if (j == 1) {
        sollPos = 2;
        motor();
        digitalWrite(L595,0);
      } else if (j == 2) {
        sollPos = 3;
        motor();
        digitalWrite(L520,0);
      } else if (j == 3) {
        sollPos = 4;
        motor();
        digitalWrite(L445,0);
      } else if (j == 4) {
        sollPos = 5;
        motor();
        digitalWrite(L390,0);
      } else if (j == 5) {
        sollPos = 6;
        motor();
        digitalWrite(L365,0);
      } else if (j == 6) {
        sollPos = 7;
        motor();
        digitalWrite(L365,0);
      } else if (j == 7) {
        sollPos = 8;
        motor();
        digitalWrite(L281,0);
      }
      delay(500);  //Schaltzeit Relais, anpassen (auch unten)
      if (j < 6) {
        time0 = millis();
        counter1 = 0;
        delay(500);
        light = counter1;
        time1 = millis();
        light = light / (float(time1 - time0) / 1000);      
      } else {
        light = ina2.getBusVoltage_V();
      }
      if (j == 0) {  //motor!! --> auch wieder zurück! Errorausgabe motor
        I1 = light;
        digitalWrite(L635,1);
      } else if (j == 1) {
        I2 = light;
        digitalWrite(L595,1);
      } else if (j == 2) {
        I3 = light;
        digitalWrite(L520,1);
      } else if (j == 3) {
        I4 = light;
        digitalWrite(L445,1);
      } else if (j == 4) {
        I5 = light;
        digitalWrite(L390,1);
      } else if (j == 5) {
        I6 = light;
        digitalWrite(L365,1);
      } else if (j == 6) {
        I7 = light;
        digitalWrite(L365,1);
      } else if (j == 7) {
        I8 = light;
        digitalWrite(L281,1);
      }
      if (error == 1) {
        Serial1.println("Motorfehler");
        j = 10;
      }
      delay(100);
    }
    if (error == 0) {
      Serial1.println("Messung abgeschlossen");
      Serial.println("moiiin");
    } else {
      Serial1.println("Messung abgebrochen");
    }
    Serial.println("Motor");
    sollPos = 0;
    motor(); 
    if (error == 0) {
      if (!SD.exists("arduino")) {
        SD.mkdir("arduino");
      }
      if (!SD.exists("arduino/water")) {
        SD.mkdir("arduino/water");
      }
      SD.remove("arduino/water/i1.txt");
      SD.remove("arduino/water/i2.txt");
      SD.remove("arduino/water/i3.txt");
      SD.remove("arduino/water/i4.txt");
      SD.remove("arduino/water/i5.txt");
      SD.remove("arduino/water/i6.txt");
      SD.remove("arduino/water/i7.txt");
      SD.remove("arduino/water/i8.txt");
      Data = SD.open("arduino/water/i1.txt",FILE_WRITE);
      Data.print(I1);
      Data.close();
      Data = SD.open("arduino/water/i2.txt",FILE_WRITE);
      Data.print(I2);
      Data.close();
      Data = SD.open("arduino/water/i3.txt",FILE_WRITE);
      Data.print(I3);
      Data.close();
      Data = SD.open("arduino/water/i4.txt",FILE_WRITE);
      Data.print(I4);
      Data.close();
      Data = SD.open("arduino/water/i5.txt",FILE_WRITE);
      Data.print(I5);
      Data.close();
      Data = SD.open("arduino/water/i6.txt",FILE_WRITE);
      Data.print(I6);
      Data.close();
      Data = SD.open("arduino/water/i7.txt",FILE_WRITE);
      Data.print(I7);
      Data.close();
      Data = SD.open("arduino/water/i8.txt",FILE_WRITE);
      Data.print(I8);
      Data.close();
    }
    if (error == 0) {
      Serial1.println("Referenz erfolgreich gespeichert");
    } else {
      Serial1.println("Referenzmessung abgebrochen");
    }
  }
  if (start == 1) {
    Serial1.println("Initialisierung startet");
    error = 0;
    start = 0;
    sollPos = 0;
    motor();
    if (!ina1.init()) {
      Serial1.println(F("INA1 nicht verbunden"));
      error = 1;
    }
    if (!ina2.init()) {
      Serial1.println(F("INA2 nicht verbunden"));
      error = 1;
    }
    sensors.begin();
    if (!sensors.getAddress(tc,0)) {
      Serial1.println(F("Temperatursensor nicht gefunden"));
      error = 1;
    }
    sensors.setResolution(tc, 12);
    if (!rtc.begin()) {    //Starten der Uhr
      Serial1.println(F("Uhr startet nicht"));
      error = 1;
    }
    for (i=0; i<5; i++) {
      if (SD.begin(53)) {
        i = 30;
      }
    }
    if (i < 30) {
      Serial1.println(F("keine SD-Karte gefunden"));
      error = 1;
    }
    if (error == 1) {
      Serial1.println("Initialiserungsfehler, bitte Wiederholen!");
      ok = 0;
    } else {
      Serial1.println("Initialisierung erfolgreich");
      ok = 1;
    }
  }
  if (meas == 1) {
    error = 0;
    meas = 0;
    waterErr = 0;
    dataErr = 0;
    Serial.println("Kontrolle Kalibrierung");
    if (!SD.exists("arduino/water/i1.txt")) {
      waterErr = 1;
    }
    if (!SD.exists("arduino/water/i2.txt")) {
      waterErr = 1;
    }
    if (!SD.exists("arduino/water/i3.txt")) {
      waterErr = 1;
    }
    if (!SD.exists("arduino/water/i4.txt")) {
      waterErr = 1;
    }
    if (!SD.exists("arduino/water/i5.txt")) {
      waterErr = 1;
    }
    if (!SD.exists("arduino/water/i6.txt")) {
      waterErr = 1;
    }
    if (!SD.exists("arduino/water/i7.txt")) {
      waterErr = 1;
    }
    if (!SD.exists("arduino/water/i8.txt")) {
      waterErr = 1;
    }
    if (!SD.exists("arduino/lgs/teiler/t1.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/teiler/t2.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/teiler/t3.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/teiler/t4.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/teiler/t5.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/teiler/t6.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/teiler/t7.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/teiler/t8.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/teiler/t9.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/teiler/t10.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/teiler/t11.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/teiler/t12.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/teiler/t13.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/teiler/t14.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/teiler/t15.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f6_6.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f5_6.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f5_5.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f4_6.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f4_5.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f4_4.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f3_6.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f3_5.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f3_4.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f3_3.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f2_6.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f2_5.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f2_4.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f2_3.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f2_2.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f1_6.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f1_5.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f1_4.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f1_3.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f1_2.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/lgs/faktoren/f1_1.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/equi/cu.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/equi/feii.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/equi/feiii.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/equi/ca.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/equi/mg.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/membr/cu.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/membr/feii.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/membr/feiii.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/membr/ca.txt")) {
      dataErr = 1;
    }
    if (!SD.exists("arduino/membr/mg.txt")) {
      dataErr = 1;
    }
    if (waterErr == 1) {
      Serial1.println("Wasserreferenz nicht gemessen");
      error = 1;
    }
    if (dataErr == 1) {
      Serial1.println("Keine Kalibrierung gefunden");
      error = 1;
    }
    if (error == 0) {
      output = "Start Messung '" + fileName;
      output = output + "'";
      Serial1.println(output);
      Serial1.println("Vorregeneration");  //überspannungs/Stromschutz!
      digitalWrite(S1Z1,0);
      digitalWrite(SIna,0);
      DateTime now = rtc.now();
      timeS1 = now.hour();
      timeS1 = timeS1 * 60;
      timeS1 = timeS1 + now.minute();
      timeS1 = timeS1 * 60;
      timeS1 = timeS1 + now.second();
      timeS1 = timeS1 + 1; //3Minuten (wieder 180) !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      do {
        DateTime now = rtc.now();
        timeS2 = now.hour();
        timeS2 = timeS2 * 60;
        timeS2 = timeS2 + now.minute();
        timeS2 = timeS2 * 60;
        timeS2 = timeS2 + now.second();
        if (5 < ina1.getCurrent_mA()) {  //werte anpassen
          error = 1;
          timeS2 += 300;
          Serial1.print("Überstrom, Strom [mA]: ");
          Serial1.println(ina1.getCurrent_mA());
        } else if ((2.9 > ina1.getBusVoltage_V()) || (5.1 < ina1.getBusVoltage_V())) {  //Werte anpassen
          error = 1;
          timeS2 += 300;
          Serial1.print("Spannungsfehler, Spannung [V]: ");
          Serial1.println(ina1.getBusVoltage_V());
        }
        Serial.println(timeS2);
        delay(10);
      } while(timeS2 < timeS1);
      digitalWrite(S1Z1,1);
      digitalWrite(SIna,1);
      if (error == 0) {
        Serial1.println("Vorregeneration abgeschlossen");
      } else {
        Serial1.println("Vorregeneration abgebrochen");
      }
    }
    if (error == 0) {
      Serial1.println("Messung1");
      for (j=0; j<8; j++) {
        Serial.print(j);
        if (j == 0) {  //motor!! --> auch wieder zurück! Errorausgabe motor
          sollPos = 1;
          motor();
          digitalWrite(L635,0);
        } else if (j == 1) {
          sollPos = 2;
          motor();
          digitalWrite(L595,0);
        } else if (j == 2) {
          sollPos = 3;
          motor();
          digitalWrite(L520,0);
        } else if (j == 3) {
          sollPos = 4;
          motor();
          digitalWrite(L445,0);
        } else if (j == 4) {
          sollPos = 5;
          motor();
          digitalWrite(L390,0);
        } else if (j == 5) {
          sollPos = 6;
          motor();
          digitalWrite(L365,0);
        } else if (j == 6) {
          sollPos = 7;
          motor();
          digitalWrite(L365,0);
        } else if (j == 7) {
          sollPos = 8;
          motor();
          digitalWrite(L281,0);
        }
        delay(500);  //Schaltzeit Relais, anpassen (auch unten)
        if (j < 6) {
          time0 = millis();
          counter1 = 0;
          delay(500);
          light = counter1;
          time1 = millis();
          light = light / (float(time1 - time0) / 1000);      
        } else {
          light = ina2.getBusVoltage_V();
        }
        if (j == 0) {  //motor!! --> auch wieder zurück! Errorausgabe motor
          I1 = light;
          digitalWrite(L635,1);
        } else if (j == 1) {
          I2 = light;
          digitalWrite(L595,1);
        } else if (j == 2) {
          I3 = light;
          digitalWrite(L520,1);
        } else if (j == 3) {
          I4 = light;
          digitalWrite(L445,1);
        } else if (j == 4) {
          I5 = light;
          digitalWrite(L390,1);
        } else if (j == 5) {
          I6 = light;
          digitalWrite(L365,1);
        } else if (j == 6) {
          I7 = light;
          digitalWrite(L365,1);
        } else if (j == 7) {
          I8 = light;
          digitalWrite(L281,1);
        }
        if (error == 1) {
          Serial1.println("Motorfehler");
          j = 10;
        }
        delay(100);
      }
      if (error == 0) {
        Serial1.println("Messung1 abgeschlossen");
        Serial.println("moiiin");
      } else {
        Serial1.println("Messung1 abgebrochen");
      }
      Serial.println("Motor");
      sollPos = 0;
      motor(); 
    }
    if (error == 0) {
      Serial1.println("Berechnung Zwischenwerte");
      
      Data = SD.open(F("arduino/water/i1.txt"));  //Berechnung Absorbanz aus Intensität
      input = Data.readString();
      I1 = log(float(input.toInt()) / I8);
      Data.close();
      Data = SD.open(F("arduino/water/i2.txt"));
      input = Data.readString();
      I2 = log(float(input.toInt()) / I2);
      Data.close();
      Data = SD.open(F("arduino/water/i3.txt"));
      input = Data.readString();
      I3 = log(float(input.toInt()) / I3);
      Data.close();
      Data = SD.open(F("arduino/water/i4.txt"));
      input = Data.readString();
      I4 = log(float(input.toInt()) / I4);
      Data.close();
      Data = SD.open(F("arduino/water/i5.txt"));
      input = Data.readString();
      I5 = log(float(input.toInt()) / I5);
      Data.close();
      Data = SD.open(F("arduino/water/i6.txt"));
      input = Data.readString();
      I6 = log(float(input.toInt()) / I6);
      Data.close();
      Data = SD.open(F("arduino/water/i7.txt"));
      input = Data.readString();
      I7 = log(float(input.toInt()) / I7);
      Data.close();
      Data = SD.open(F("arduino/water/i8.txt"));
      input = Data.readString();
      I8 = log(float(input.toInt()) / I8);
      Data.close();

      Data = SD.open(F("arduino/lgs/teiler/t1.txt"));  //Zeile 1
      input = Data.readString();
      I2 = I2 + (I2 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t2.txt"));  
      input = Data.readString();
      I3 = I3 + (I3 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t3.txt"));  
      input = Data.readString();
      I4 = I4 + (I4 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t4.txt"));  
      input = Data.readString();
      I5 = I5 + (I5 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t5.txt"));  
      input = Data.readString();
      I6 = I6 + (I6 * float(input.toInt()));
      Data.close();

      Data = SD.open(F("arduino/lgs/teiler/t6.txt"));  //Zeile 2
      input = Data.readString();
      I3 = I3 + (I3 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t7.txt"));  
      input = Data.readString();
      I4 = I4 + (I4 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t8.txt"));  
      input = Data.readString();
      I5 = I5 + (I5 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t9.txt"));  
      input = Data.readString();
      I6 = I6 + (I6 * float(input.toInt()));
      Data.close();

      Data = SD.open(F("arduino/lgs/teiler/t10.txt"));  //Zeile 3
      input = Data.readString();
      I4 = I4 + (I4 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t11.txt"));  
      input = Data.readString();
      I5 = I5 + (I5 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t12.txt"));  
      input = Data.readString();
      I6 = I6 + (I6 * float(input.toInt()));
      Data.close();

      Data = SD.open(F("arduino/lgs/teiler/t13.txt"));  //Zeile 4
      input = Data.readString();
      I5 = I5 + (I5 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t14.txt"));  
      input = Data.readString();
      I6 = I6 + (I6 * float(input.toInt()));
      Data.close();

      Data = SD.open(F("arduino/lgs/teiler/t15.txt"));  //Zeile 5
      input = Data.readString();
      I6 = I6 + (I6 * float(input.toInt()));
      Data.close();
      
      Data = SD.open(F("arduino/lgs/faktoren/f6_6.txt"));  //Bestimmung CM
      input = Data.readString();
      CM0 = I6 / float(input.toInt());
      Data.close();

      Data = SD.open(F("arduino/lgs/faktoren/f5_6.txt"));  //Bestimmung Cu
      input = Data.readString();
      I5 = I5 - (CM0 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f5_5.txt"));
      input = Data.readString();
      Cu0 = I5 / float(input.toInt());
      Data.close();

      Data = SD.open(F("arduino/lgs/faktoren/f4_6.txt"));  //Bestimmung FeII
      input = Data.readString();
      I4 = I4 - (CM0 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f4_5.txt"));
      input = Data.readString();
      I4 = I4 - (Cu0 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f4_4.txt"));
      input = Data.readString();
      FeII0 = I4 / float(input.toInt());
      Data.close();

      Data = SD.open(F("arduino/lgs/faktoren/f3_6.txt"));  //Bestimmung FeIII
      input = Data.readString();
      I3 = I3 - (CM0 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f3_5.txt"));
      input = Data.readString();
      I3 = I3 - (Cu0 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f3_4.txt"));
      input = Data.readString();
      I3 = I3 - (FeII0 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f3_3.txt"));
      input = Data.readString();
      FeIII0 = I3 / float(input.toInt());
      Data.close();

      Data = SD.open(F("arduino/lgs/faktoren/f2_6.txt"));  //Bestimmung Ca
      input = Data.readString();
      I2 = I2 - (CM0 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f2_5.txt"));
      input = Data.readString();
      I2 = I2 - (Cu0 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f2_4.txt"));
      input = Data.readString();
      I2 = I2 - (FeII0 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f2_3.txt"));
      input = Data.readString();
      I2 = I2 - (FeIII0 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f2_2.txt"));
      input = Data.readString();
      Ca0 = I2 / float(input.toInt());
      Data.close();

      Data = SD.open(F("arduino/lgs/faktoren/f1_6.txt"));  //Bestimmung Cu
      input = Data.readString();
      I1 = I1 - (CM0 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f1_5.txt"));
      input = Data.readString();
      I1 = I1 - (Cu0 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f1_4.txt"));
      input = Data.readString();
      I1 = I1 - (FeII0 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f1_3.txt"));
      input = Data.readString();
      I1 = I1 - (FeIII0 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f1_2.txt"));
      input = Data.readString();
      I1 = I1 - (Ca0 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f1_1.txt"));
      input = Data.readString();
      Mg0 = I1 / float(input.toInt());
      Data.close();

      Data = SD.open(F("arduino/equi/cu.txt"));  //Bestimmung Realkonzentration Zelle
      input = Data.readString();
      Cu0 = ((Cu0 / CM0) * pow(10,input.toInt())) + Cu0; 
      Data.close();
      Data = SD.open(F("arduino/equi/feii.txt"));
      input = Data.readString();
      FeII0 = ((FeII0 / CM0) * pow(10,input.toInt())) + FeII0; 
      Data.close();
      Data = SD.open(F("arduino/equi/feiii.txt"));
      input = Data.readString();
      FeIII0 = ((FeIII0 / CM0) * pow(10,input.toInt())) + FeIII0; 
      Data.close();
      Data = SD.open(F("arduino/equi/ca.txt"));
      input = Data.readString();
      Ca0 = ((Ca0 / CM0) * pow(10,input.toInt())) + Ca0; 
      Data.close();
      Data = SD.open(F("arduino/equi/mg.txt"));
      input = Data.readString();
      Mg0 = ((Mg0 / CM0) * pow(10,input.toInt())) + Mg0; 
      Data.close(); 
    }
    if (error == 0) {
      Serial1.println("Elektrolyse");  //überspannungs/Stromschutz!
      digitalWrite(S1Z1,0);
      digitalWrite(SIna,0);
      DateTime now = rtc.now();
      timeS1 = now.hour();
      timeS1 = timeS1 * 60;
      timeS1 = timeS1 + now.minute();
      timeS1 = timeS1 * 60;
      timeS1 = timeS1 + now.second();
      time2 = millis();
      exitE = 0;
      lightTest = 0;
      do {
        time3 = millis();
        if ((time3 - time2) > 10000) {
          time2 = millis();
          digitalWrite(365,0);  //LED anpassen
          delay(200);
          time0 = millis();
          counter1 = 0;
          delay(500);
          light = counter1;
          time1 = millis();
          light = light / (float(time1 - time0) / 1000); 
          digitalWrite(365,1);
          Data = SD.open("arduino/water/i1.txt");  //muss an LED angepasst sein!!
          input = Data.readString();
          light = log(float(input.toInt() / light));  
          if ((light < 5) || (((light - 5) < light0) && (lightTest == 1))) {  //5 nur Platzhalter!
            Serial1.println("Detektor stark gesättigt");
            exitE = 1;
          }
          light0 = light;
          lightTest = 1;
        }
        DateTime now = rtc.now();
        timeS2 = now.hour();
        timeS2 = timeS2 * 60;
        timeS2 = timeS2 + now.minute();
        timeS2 = timeS2 * 60;
        timeS2 = timeS2 + now.second();
        if (5 < ina1.getCurrent_mA()) {  //werte anpassen
          error = 1;
          exitE = 1;
          Serial1.print("Überstrom, Strom [mA]: ");
          Serial1.println(ina1.getCurrent_mA());
        } else if ((2.9 > ina1.getBusVoltage_V()) || (5.1 < ina1.getBusVoltage_V())) {  //Werte anpassen
          error = 1;
          exitE = 1;
          Serial1.print("Spannungsfehler, Spannung [V]: ");
          Serial1.println(ina1.getBusVoltage_V());
        }
        Serial.println(timeS2);
        delay(10);
        if ((timeS2 - timeS1) > 300) {  //elektrolysezeit, anpassen (wichtige Variablen oben definieren!!!)
          exitE = 1;
        }
      } while(exitE == 0);
      digitalWrite(S1Z1,1);
      digitalWrite(SIna,1);
      now = rtc.now();
      timeS2 = now.hour();
      timeS2 = timeS2 * 60;
      timeS2 = timeS2 + now.minute();
      timeS2 = timeS2 * 60;
      timeS2 = timeS2 + now.second();
      timeE = timeS2 - timeS1;
      if (error == 0) {
        Serial1.println("Elektrolyse abgeschlossen");
      } else {
        Serial1.println("Elektrolyse abgebrochen");
      }
    }
    if (error == 0) {
      Serial1.println("Messung2");
      for (j=0; j<8; j++) {
        if (j == 0) {  //motor!! --> auch wieder zurück! Errorausgabe motor
          sollPos = 1;
          motor();
          digitalWrite(L635,0);
        } else if (j == 1) {
          sollPos = 2;
          motor();
          digitalWrite(L595,0);
        } else if (j == 2) {
          sollPos = 3;
          motor();
          digitalWrite(L520,0);
        } else if (j == 3) {
          sollPos = 4;
          motor();
          digitalWrite(L445,0);
        } else if (j == 4) {
          sollPos = 5;
          motor();
          digitalWrite(L390,0);
        } else if (j == 5) {
          sollPos = 6;
          motor();
          digitalWrite(L365,0);
        } else if (j == 6) {
          sollPos = 7;
          motor();
          digitalWrite(L365,0);
        } else if (j == 7) {
          sollPos = 8;
          motor();
          digitalWrite(L281,0);
        }
        delay(500);  //Schaltzeit Relais, anpassen (auch unten)
        if (j < 6) {
          time0 = millis();
          counter1 = 0;
          delay(500);
          light = counter1;
          time1 = millis();
          light = light / (float(time1 - time0) / 1000);      
        } else {
          light = ina2.getBusVoltage_V();
        }
        if (j == 0) {  //motor!! --> auch wieder zurück! Errorausgabe motor
          I1 = light;
          digitalWrite(L635,1);
        } else if (j == 1) {
          I2 = light;
          digitalWrite(L595,1);
        } else if (j == 2) {
          I3 = light;
          digitalWrite(L520,1);
        } else if (j == 3) {
          I4 = light;
          digitalWrite(L445,1);
        } else if (j == 4) {
          I5 = light;
          digitalWrite(L390,1);
        } else if (j == 5) {
          I6 = light;
          digitalWrite(L365,1);
        } else if (j == 6) {
          I7 = light;
          digitalWrite(L365,1);
        } else if (j == 7) {
          I8 = light;
          digitalWrite(L281,1);
        }
        if (error == 1) {
          Serial1.println("Motorfehler");
          j = 10;
        }
        delay(100);
      }
      if (error == 0) {
        Serial1.println("Messung2 abgeschlossen");
      } else {
        Serial1.println("Messung2 abgebrochen");
      }
      sollPos = 0;
      motor(); 
    }
    if (error == 0) {
      Serial1.println("Berechnung Ergebnisse");
      
      Data = SD.open(F("arduino/water/i1.txt"));  //Berechnung Absorbanz aus Intensität
      input = Data.readString();
      I1 = log(float(input.toInt()) / I8);
      Data.close();
      Data = SD.open(F("arduino/water/i2.txt"));
      input = Data.readString();
      I2 = log(float(input.toInt()) / I2);
      Data.close();
      Data = SD.open(F("arduino/water/i3.txt"));
      input = Data.readString();
      I3 = log(float(input.toInt()) / I3);
      Data.close();
      Data = SD.open(F("arduino/water/i4.txt"));
      input = Data.readString();
      I4 = log(float(input.toInt()) / I4);
      Data.close();
      Data = SD.open(F("arduino/water/i5.txt"));
      input = Data.readString();
      I5 = log(float(input.toInt()) / I5);
      Data.close();
      Data = SD.open(F("arduino/water/i6.txt"));
      input = Data.readString();
      I6 = log(float(input.toInt()) / I6);
      Data.close();
      Data = SD.open(F("arduino/water/i7.txt"));
      input = Data.readString();
      I7 = log(float(input.toInt()) / I7);
      Data.close();
      Data = SD.open(F("arduino/water/i8.txt"));
      input = Data.readString();
      I8 = log(float(input.toInt()) / I8);
      Data.close();

      Data = SD.open(F("arduino/lgs/teiler/t1.txt"));  //Zeile 1
      input = Data.readString();
      I2 = I2 + (I2 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t2.txt"));  
      input = Data.readString();
      I3 = I3 + (I3 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t3.txt"));  
      input = Data.readString();
      I4 = I4 + (I4 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t4.txt"));  
      input = Data.readString();
      I5 = I5 + (I5 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t5.txt"));  
      input = Data.readString();
      I6 = I6 + (I6 * float(input.toInt()));
      Data.close();

      Data = SD.open(F("arduino/lgs/teiler/t6.txt"));  //Zeile 2
      input = Data.readString();
      I3 = I3 + (I3 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t7.txt"));  
      input = Data.readString();
      I4 = I4 + (I4 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t8.txt"));  
      input = Data.readString();
      I5 = I5 + (I5 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t9.txt"));  
      input = Data.readString();
      I6 = I6 + (I6 * float(input.toInt()));
      Data.close();

      Data = SD.open(F("arduino/lgs/teiler/t10.txt"));  //Zeile 3
      input = Data.readString();
      I4 = I4 + (I4 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t11.txt"));  
      input = Data.readString();
      I5 = I5 + (I5 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t12.txt"));  
      input = Data.readString();
      I6 = I6 + (I6 * float(input.toInt()));
      Data.close();

      Data = SD.open(F("arduino/lgs/teiler/t13.txt"));  //Zeile 4
      input = Data.readString();
      I5 = I5 + (I5 * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/teiler/t14.txt"));  
      input = Data.readString();
      I6 = I6 + (I6 * float(input.toInt()));
      Data.close();

      Data = SD.open(F("arduino/lgs/teiler/t14.txt"));  //Zeile 5
      input = Data.readString();
      I6 = I6 + (I6 * float(input.toInt()));
      Data.close();
      
      Data = SD.open(F("arduino/lgs/faktoren/f6_6.txt"));  //Bestimmung CM
      input = Data.readString();
      CM1  = I6 / float(input.toInt());
      Data.close();

      Data = SD.open(F("arduino/lgs/faktoren/f5_6.txt"));  //Bestimmung Cu
      input = Data.readString();
      I5 = I5 - (CM1  * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f5_5.txt"));
      input = Data.readString();
      Cu1  = I5 / float(input.toInt());
      Data.close();

      Data = SD.open(F("arduino/lgs/faktoren/f4_6.txt"));  //Bestimmung FeII
      input = Data.readString();
      I4 = I4 - (CM1  * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f4_5.txt"));
      input = Data.readString();
      I4 = I4 - (Cu1  * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f4_4.txt"));
      input = Data.readString();
      FeII1  = I4 / float(input.toInt());
      Data.close();

      Data = SD.open(F("arduino/lgs/faktoren/f3_6.txt"));  //Bestimmung FeIII
      input = Data.readString();
      I3 = I3 - (CM1  * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f3_5.txt"));
      input = Data.readString();
      I3 = I3 - (Cu1  * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f3_4.txt"));
      input = Data.readString();
      I3 = I3 - (FeII1  * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f3_3.txt"));
      input = Data.readString();
      FeIII1  = I3 / float(input.toInt());
      Data.close();

      Data = SD.open(F("arduino/lgs/faktoren/f2_6.txt"));  //Bestimmung Ca
      input = Data.readString();
      I2 = I2 - (CM1  * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f2_5.txt"));
      input = Data.readString();
      I2 = I2 - (Cu1  * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f2_4.txt"));
      input = Data.readString();
      I2 = I2 - (FeII1  * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f2_3.txt"));
      input = Data.readString();
      I2 = I2 - (FeIII1  * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f2_2.txt"));
      input = Data.readString();
      Ca1  = I2 / float(input.toInt());
      Data.close();

      Data = SD.open(F("arduino/lgs/faktoren/f1_6.txt"));  //Bestimmung Cu
      input = Data.readString();
      I1 = I1 - (CM1  * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f1_5.txt"));
      input = Data.readString();
      I1 = I1 - (Cu1  * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f1_4.txt"));
      input = Data.readString();
      I1 = I1 - (FeII1  * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f1_3.txt"));
      input = Data.readString();
      I1 = I1 - (FeIII1  * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f1_2.txt"));
      input = Data.readString();
      I1 = I1 - (Ca1  * float(input.toInt()));
      Data.close();
      Data = SD.open(F("arduino/lgs/faktoren/f1_1.txt"));
      input = Data.readString();
      Mg1  = I1 / float(input.toInt());
      Data.close();

      Data = SD.open(F("arduino/equi/cu.txt"));  //Bestimmung Realkonzentration Zelle
      input = Data.readString();
      Cu1  = ((Cu1  / CM1) * pow(10,input.toInt())) + Cu0; 
      Data.close();
      Data = SD.open(F("arduino/equi/feii.txt"));
      input = Data.readString();
      FeII1  = ((FeII1  / CM1) * pow(10,input.toInt())) + FeII0; 
      Data.close();
      Data = SD.open(F("arduino/equi/feiii.txt"));
      input = Data.readString();
      FeIII1  = ((FeIII1  / CM1) * pow(10,input.toInt())) + FeIII0; 
      Data.close();
      Data = SD.open(F("arduino/equi/ca.txt"));
      input = Data.readString();
      Ca1  = ((Ca1  / CM1) * pow(10,input.toInt())) + Ca0; 
      Data.close();
      Data = SD.open(F("arduino/equi/mg.txt"));
      input = Data.readString();
      Mg1  = ((Mg1  / CM1) * pow(10,input.toInt())) + Mg0; 
      Data.close(); 

      
      Data = SD.open("arduino/membr/cu.txt");
      input = Data.readString();
      Cu0 = ((Cu1 - Cu0) * vol) / (1 - exp(-1 * (float(input.toInt()) * timeE))); 
      Data.close();
      Data = SD.open("arduino/membr/feii.txt");
      input = Data.readString();
      FeII0 = ((FeII1 - FeII0) * vol) / (1 - exp(-1 * (float(input.toInt()) * timeE))); 
      Data.close();
      Data = SD.open("arduino/membr/feiii.txt");
      input = Data.readString();
      FeIII0 = ((FeIII1 - FeIII0) * vol) / (1 - exp(-1 * (float(input.toInt()) * timeE))); 
      Data.close();
      Data = SD.open("arduino/membr/ca.txt");
      input = Data.readString();
      Ca0 = ((Ca1 - Ca0) * vol) / (1 - exp(-1 * (float(input.toInt()) * timeE))); 
      Data.close();
      Data = SD.open("arduino/membr/mg.txt");
      input = Data.readString();
      Mg0 = ((Mg1 - Mg0) * vol) / (1 - exp(-1 * (float(input.toInt()) * timeE))); 
      Data.close();
      CM0 = CM0 - CM1; 
      temp = sensors.getTempC(tc);
      Serial1.println("Messergebnisse:");
      Serial1.print("   Messzeit [s]: ");
      Serial1.println(timeE);
      Serial1.print("   Temperatur [°C]: ");
      Serial1.println(temp);
      Serial1.println("   Konzentrationen [mol/l]:"); //evtl. mmol/l
      Serial1.print("       Cu: ");
      Serial1.println(Cu0);
      Serial1.print("       FeII: ");
      Serial1.println(FeII0);
      Serial1.print("       FeIII: ");
      Serial1.println(FeIII0);
      Serial1.print("       Ca: ");
      Serial1.println(Ca0);
      Serial1.print("       Mg: ");
      Serial1.println(Mg0);  //Härtegrad/Temperatur
      Serial1.print("       Calmagitverbrauch: ");  //angabe anpassen
      Serial1.println(CM0);
      if (!SD.exists("meas")) {
        SD.mkdir("meas");
        Serial.println("nicht vorhanden");
      } else {  //debug
        Serial.println("vorhanden");
      }
      output = "meas/" + fileName;
      output = output + ".txt";
      Serial.println(output);
      Data = SD.open(output,FILE_WRITE);
      DateTime now = rtc.now();
      Data.print("Datum: ");
      Data.print(now.day());
      Data.print(".");
      Data.print(now.month());
      Data.print(".");
      Data.println(now.year());
      Data.print("Zeit: ");
      Data.print(now.hour());
      Data.print(":");
      Data.print(now.minute());
      Data.print(":");
      Data.println(now.second());
      Data.println("Messergebnisse:");
      Data.print("  Messzeit [s]: ");
      Data.println(timeE);
      Data.print("  Temperatur [°C]: ");
      Data.println(temp);
      Data.println("  Konzentrationen [mol/l]:"); //evtl. mmol/l
      Data.print("    Cu: ");
      Data.println(Cu0);
      Data.print("    FeII: ");
      Data.println(FeII0);
      Data.print("    FeIII: ");
      Data.println(FeIII0);
      Data.print("    Ca: ");
      Data.println(Ca0);
      Data.print("    Mg: ");
      Data.println(Mg0);  //Härtegrad/Temperatur
      Data.print("    Calmagitverbrauch: ");
      Data.println(CM0);
      Data.close();
      output = "Messung als '";
      output = output + fileName;
      output = output + "' gespeichert";
      Serial1.println(output);
    }
    if (error == 1) {
      Serial1.println("Messung abgebrochen");
    }
  }
}
