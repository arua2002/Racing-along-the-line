#include "Yahboom_8CH_Line.hpp"

// Пины моторов
const int AIN1 = 19, AIN2 = 18, PWMA = 21;
const int BIN1 = 4, BIN2 = 2, PWMB = 15;
const int STBY = 5;

int base_speed = 150; // базовая скорость
const int Kp = 20; // коэффициент пропорциональности для корректировки

void setup() {
  // Настройка моторов
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  digitalWrite(STBY, HIGH); // Включаем драйвер

  Serial.begin(115200);

  // Инициализация связи с датчиками
  serial_init();

  // Можно вызвать калибровку
  // SET_Eight_Mode(1, 0, 0);
}

void loop() {
  // Обработка данных с датчиков
  recv_data();
  if (g_new_package_flag) {
    Deal_Usart_Data(); // Обновляем IR_Data_number
    g_new_package_flag = 0;

    followLine(); // следование по линии
  }
}

void followLine() {
  int position = 0;
  int total_weight = 0;

  // Подсчет положения линии
  for (int i = 0; i < IR_Num; i++) {
    if (IR_Data_number[i] == 1) {
      position += i;
      total_weight++;
    }
  }

  if (total_weight == 0) {
    // Потеряли линию, можно остановиться или искать линию
    stopRobot();
    Serial.println("Линия потеряна");
    return;
  }

  float line_center = (float)position / total_weight;
  float error = line_center - 3.5; // центр для 8 датчиков

  // Пропорциональное управление
  int correction = error * Kp;
  int left_speed = base_speed + correction;
  int right_speed = base_speed - correction;

  // Ограничение скоростей
  left_speed = constrain(left_speed, 0, 255);
  right_speed = constrain(right_speed, 0, 255);

  // Управление моторами
  analogWrite(PWMA, left_speed);
  analogWrite(PWMB, right_speed);

  // Вперед
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
}

void stopRobot() {
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
}
