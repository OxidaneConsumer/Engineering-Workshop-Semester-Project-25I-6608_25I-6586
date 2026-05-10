#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <IRremote.hpp>

#define SS_PIN 10     
#define RST_PIN 9     
const int trigpin = 5; 
const int echopin = 4; 
const int servopin = 6;
const int redLed = 2;    
const int greenLed = 3; 
const int boo = 8;
const int irPin = 7;

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo myservo;

int doorOpen = 0; 
bool carWaiting = false;
bool adminMode = false;
unsigned long clearPathStartTime = 0;

byte authorizedUID[] = {0x34, 0xC0, 0x31, 0x03}; 
#define BUTTON_1 0xF30CFF00
#define BUTTON_0 0xE916FF00

void setup() {
  Serial.begin(9600);
  SPI.begin();           
  mfrc522.PCD_Init();    
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print("RFID Status: ");
  if (v == 0x00 || v == 0xFF) Serial.println("NOT CONNECTED / WIRING ERROR");
  else Serial.println("ONLINE");

  IrReceiver.begin(irPin, ENABLE_LED_FEEDBACK);

  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(boo, OUTPUT);
  myservo.attach(servopin);
  myservo.write(0); 
  digitalWrite(redLed, HIGH);
  digitalWrite(greenLed, LOW);
  Serial.println("--- System Standby ---");
}

void loop() {
  if (IrReceiver.decode()) {
    unsigned long irCode = IrReceiver.decodedIRData.decodedRawData;
    if (irCode == BUTTON_1) {
      Serial.println("Admin mode activated");
      adminFlash();
      adminMode = true;
      if (doorOpen == 0) openGate();
    }
    else if (irCode == BUTTON_0) {
      Serial.println("Admin mode deactivated");
      adminFlash();
      adminMode = false;
      if (doorOpen == 1) closeGate();
    }
    IrReceiver.resume();
  }

  if (adminMode) {
    delay(50);
    return;
  }

  int distance = readDistance();

  if (distance > 0 && distance <= 6 && !carWaiting && doorOpen == 0) {
    Serial.println("Welcome! Please scan authorized RFID card.");
    carWaiting = true; 
  } 
  else if ((distance > 6 || distance == 0) && carWaiting && doorOpen == 0) {
    carWaiting = false;
    Serial.println("Car left. Returning to standby.");
  }

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    if (checkUID(mfrc522.uid.uidByte, mfrc522.uid.size)) {
      Serial.println("Access Granted!");
      if (doorOpen == 0) openGate();
      carWaiting = false;
      clearPathStartTime = 0;
    } else {
      Serial.println("Access Denied!");
      denyAccessFlash();
    }
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }

  if (doorOpen == 1) {
    if (distance > 6 || distance == 0) {
      if (clearPathStartTime == 0) clearPathStartTime = millis();
      if (millis() - clearPathStartTime >= 1000) {
        closeGate();
        Serial.println("--- System Standby ---");
      }
    } else {
      clearPathStartTime = 0;
    }
  }
  delay(50);
}