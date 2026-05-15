#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "AiEsp32RotaryEncoder.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Пины энкодера
#define ROTARY_ENCODER_A_PIN 27
#define ROTARY_ENCODER_B_PIN 25
#define ROTARY_ENCODER_BUTTON_PIN 26
#define ROTARY_ENCODER_VCC_PIN -1
#define ROTARY_ENCODER_STEPS 4

// Создаём объект энкодера
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(
  ROTARY_ENCODER_A_PIN, 
  ROTARY_ENCODER_B_PIN, 
  ROTARY_ENCODER_BUTTON_PIN, 
  ROTARY_ENCODER_VCC_PIN, 
  ROTARY_ENCODER_STEPS
);

// Меню
int menuItem = 0;
const char* menuItems[] = {"Start", "Settings", "Info"};
int totalItems = 3;

// Функция обработки прерывания
void IRAM_ATTR readEncoderISR() {
  rotaryEncoder.readEncoder_ISR();
}

// Функция при нажатии кнопки
void onButtonClick() {
  static unsigned long lastTimePressed = 0;
  if (millis() - lastTimePressed < 500) return;
  lastTimePressed = millis();
  
  Serial.print("Selected: ");
  Serial.println(menuItems[menuItem]);
  
  // Показываем выбор на дисплее
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 25);
  display.print(menuItems[menuItem]);
  display.display();
  delay(300);
  drawMenu();
}

void drawMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("===== MENU =====");
  
  for (int i = 0; i < totalItems; i++) {
    if (i == menuItem) {
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }
    display.setCursor(20, 18 + i * 15);
    display.print(menuItems[i]);
  }
  
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 56);
  display.print("Rotate -> Select");
  display.display();
}

void setup() {
  Serial.begin(115200);
  
  // Инициализация OLED
  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!");
    while (true);
  }
  
  // Инициализация энкодера
  pinMode(ROTARY_ENCODER_A_PIN, INPUT_PULLUP);
  pinMode(ROTARY_ENCODER_B_PIN, INPUT_PULLUP);
  pinMode(ROTARY_ENCODER_BUTTON_PIN, INPUT_PULLUP);
  
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(0, totalItems - 1, true); // Зацикливание меню
  rotaryEncoder.setAcceleration(0); // Для меню ускорение не нужно
  rotaryEncoder.disableAcceleration();
  
  drawMenu();
}

void loop() {
  // Обработка вращения энкодера
  if (rotaryEncoder.encoderChanged()) {
    menuItem = rotaryEncoder.readEncoder();
    drawMenu();
    Serial.print("Menu item: ");
    Serial.println(menuItem);
  }
  
  // Обработка нажатия кнопки
  if (rotaryEncoder.isEncoderButtonClicked()) {
    onButtonClick();
  }
  
  delay(5);
}