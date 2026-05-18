// Простой тест: моторы едут вперёд 3 секунды

const int AIN1 = 19;
const int AIN2 = 15;
const int PWMA = 23;
const int BIN1 = 5;
const int BIN2 = 4;
const int PWMB = 2;
const int STBY = 18;

void setup() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);
  
  digitalWrite(STBY, HIGH);  // Включаем драйвер
  
  // Вперёд
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  
  analogWrite(PWMA, 50);  // Скорость левого
  analogWrite(PWMB, 50);  // Скорость правого

}

void loop() {
  // Ничего не делаем
}