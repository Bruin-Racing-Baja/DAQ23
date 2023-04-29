#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

#include <Wire.h>



const int MPU = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

const int buttonpin=32;
const int LED_pin=2;
const int wheelSpeedPin=11;
int wl_count=0;

unsigned long lastDebounceTime=0;
unsigned long debounceTime=0;
unsigned long debounceDelay=0;

const int chipSelect = BUILTIN_SDCARD;
File dataFile;

int thermistor1;
int thermistor2;
int thermistor3;

volatile bool file_open;

void setup()
{
    SD.begin(chipSelect);
    dataFile = SD.open("sunset_car_april9.txt", FILE_WRITE);
    file_open = true;
    
    attachInterrupt(digitalPinToInterrupt(buttonpin), openORclosefile, FALLING);
    attachInterrupt(digitalPinToInterrupt(wheelSpeedPin),[](){++wl_count;},RISING);

    Serial.begin(9600);

    Wire2.begin();
    Wire2.beginTransmission(MPU);
    Wire2.write(0x6B);
    Wire2.write(0);
    Wire2.endTransmission(true);
}

void openORclosefile()
{
    if (file_open == true)
    {
        dataFile.println("break");
        dataFile.close();
        file_open = false;
    }
    else
    {
        dataFile = SD.open("sunset_car_april9.txt", FILE_WRITE);
        file_open = true;
        digitalWrite(LED_pin, HIGH);
    }
}

void loop()
{
    if (file_open == true)
    { 
        thermistor1=analogRead(A7);
        thermistor2=analogRead(A8);
        thermistor3=analogRead(A9);
        /*
       strainValue1 = analogRead(A0);
       strainValue3 = analogRead(A2);
       strainValueAA = analogRead(A1);
       strainValueTR = analogRead(A9);
       */

        Wire2.beginTransmission(MPU);
        Wire2.write(0x3B);
        Wire2.endTransmission(false);
        Wire2.requestFrom(MPU, 12, true);
        AcX = Wire2.read() << 8 | Wire2.read();
        AcY = Wire2.read() << 8 | Wire2.read();
        AcZ = Wire2.read() << 8 | Wire2.read();
        GyX = Wire2.read() << 8 | Wire2.read();
        GyY = Wire2.read() << 8 | Wire2.read();
        GyZ = Wire2.read() << 8 | Wire2.read();

        dataFile.println(String(micros()) + "," /*+ String(strainValue1) + "," + String(strainValue3) + "," + String(strainValueAA)  + "," + String(strainValueTR)*/ + "," + String(AcX) + "," + String(AcY) + "," + String(AcZ) + "," + String(GyX) + "," + String(GyY) + "," + String(GyZ));
    }

    delay(100);
}