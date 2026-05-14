#include "Yahboom_8CH_Line.hpp"
// === ПИНЫ МОТОРОВ ===
const int AIN1 = 19, AIN2 = 18, PWMA = 21;
const int BIN1 = 4, BIN2 = 2, PWMB = 15;
const int STBY = 5;
// === НАСТРОЙКИ ===
int base_speed = 100;
float Kp = 0.5;
float Kd = 0.2;
// === ПЕРЕМЕННЫЕ ===
float prev_error = 0;
float position = 0;
float weights[8] = {-3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5};
// === SETUP ===
void setup() {
    Serial.begin(115200);
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(PWMA, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    pinMode(PWMB, OUTPUT);
    pinMode(STBY, OUTPUT);
    digitalWrite(STBY, HIGH);
    serial_init();
    delay(500);
    SET_Eight_Mode(0, 1, 1);
    Serial.println("Ready");
}
// === LOOP ===
void loop() {
  readSensors();
  
  if (lineFound()) {
    calcError();
    float correction = Kp * position + Kd * (position - prev_error);
    prev_error = position;
    int left = base_speed + correction;
    int right = base_speed - correction;
    left = constrain(left, 0, 255);
    right = constrain(right, 0, 255);
    drive(left, right);
  } 
  else {
    stop();
    Serial.println("Line lost");
  }
    delay(10);
}
// === ФУНКЦИИ ===
void readSensors() {
  while (Serial2.available()) {
    char c = Serial2.read();
    parseChar(c);
  }
}
void parseChar(char c) {
  static char buffer[100];
  static int index = 0;
    if (c == '$') {
        index = 0;
        buffer[index++] = c;
    } 
   else if (index > 0 && index < 99) {
        buffer[index++] = c;
        if (c == '#') {
            buffer[index] = '\0';
            parseData(buffer);
            index = 0;
        }
    }
}
void parseData(char* data) {
  for (int i = 0; i < 8; i++) {
        char search[5];
        sprintf(search, "x%d:", i + 1);
        char* pos = strstr(data, search);
        if (pos) IR_Data_Anglo[i] = atoi(pos + 3);
            
    }
}
bool lineFound() {
  for (int i = 0; i < 8; i++) {
        if (IR_Data_Anglo[i] > 1500) return true;
    }
  return false;
}
void calcError() {
  float sum = 0;
  int count = 0;
    for (int i = 0; i < 8; i++) {
    if (IR_Data_Anglo[i] > 1500) {
      sum += weights[i];
      count++;
    }
  }
    if (count > 0) {
    position = sum / count;
  }
}
void drive(int left, int right) {
  digitalWrite(AIN1, HIGH); 
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH); 
  digitalWrite(BIN2, LOW);
  analogWrite(PWMA, left);
  analogWrite(PWMB, right);
}
void stop() {
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
}