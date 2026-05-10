int readDistance() {
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);
  long duration = pulseIn(echopin, HIGH, 25000); 
  return duration * 0.034 / 2;
}

bool checkUID(byte *scan, byte size) {
  for (byte i = 0; i < size; i++) {
    if (scan[i] != authorizedUID[i]) return false;
  }
  return true;
}

void openGate() {
  digitalWrite(redLed, LOW);    
  digitalWrite(greenLed, HIGH); 
  digitalWrite(boo, HIGH);
  delay(500);
  digitalWrite(boo, LOW);
  for (int pos = 0; pos <= 90; pos += 2) {
    myservo.write(pos);
    delay(15);
  }
  doorOpen = 1;
}

void closeGate() {
  digitalWrite(greenLed, LOW); 
  digitalWrite(redLed, HIGH);  
  for (int pos = 90; pos >= 0; pos -= 2) {
    myservo.write(pos);
    delay(15);
  }
  doorOpen = 0;
}

void denyAccessFlash() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(redLed, LOW);
    digitalWrite(boo, HIGH);
    delay(100);
    digitalWrite(boo, LOW);
    digitalWrite(redLed, HIGH);
    delay(100);
  }
}

void adminFlash() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, HIGH);
    digitalWrite(boo, HIGH);
    delay(150);
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(boo, LOW);
    delay(150);
  }
  if (doorOpen == 0) {
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
  } else {
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
  }
}
