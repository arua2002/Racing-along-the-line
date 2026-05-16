#include "WiFiPIDTuner.h"

static float _Kp = 0.8;
static float _Ki = 0.0;
static float _Kd = 0.3;
static WebServer* _globalServer = nullptr;

WiFiPIDTuner::WiFiPIDTuner(const char* password) {
    _password = password;
    _ssid = "PID_Tuner";
    _server = new WebServer(80);
    _globalServer = _server;
}

WiFiPIDTuner::~WiFiPIDTuner() {
    delete _server;
}

void WiFiPIDTuner::begin() {
    // Отключаем WiFi перед настройкой
    WiFi.disconnect(true);
    delay(100);
    
    // Устанавливаем режим точки доступа
    WiFi.mode(WIFI_AP);
    delay(100);
    
    // Настройки для стабильности
    WiFi.softAPConfig(
        IPAddress(192, 168, 4, 1),
        IPAddress(192, 168, 4, 1),
        IPAddress(255, 255, 255, 0)
    );
    
    // Создаём точку доступа
    WiFi.softAP(_ssid.c_str(), _password, 1, 0, 1);
    delay(500);
    
    // Настройка обработчиков
    _server->on("/", std::bind(&WiFiPIDTuner::handleRoot, this));
    _server->on("/set", std::bind(&WiFiPIDTuner::handleSet, this));
    _server->on("/get", std::bind(&WiFiPIDTuner::handleGet, this));
    
    _server->begin();
}

void WiFiPIDTuner::handleClient() {
    _server->handleClient();
}

void WiFiPIDTuner::handleRoot() {
    _server->send(200, "text/html", _htmlPage);
}

void WiFiPIDTuner::handleSet() {
    if (_server->hasArg("kp")) _Kp = _server->arg("kp").toFloat();
    if (_server->hasArg("ki")) _Ki = _server->arg("ki").toFloat();
    if (_server->hasArg("kd")) _Kd = _server->arg("kd").toFloat();
    
    String message = "PID updated: Kp=" + String(_Kp, 2) + 
                    ", Ki=" + String(_Ki, 3) + 
                    ", Kd=" + String(_Kd, 2);
    _server->send(200, "text/plain", message);
}

void WiFiPIDTuner::handleGet() {
    String json = "{\"kp\":" + String(_Kp, 2) + 
                  ",\"ki\":" + String(_Ki, 3) + 
                  ",\"kd\":" + String(_Kd, 2) + "}";
    _server->send(200, "application/json", json);
}

void WiFiPIDTuner::getPID(float &kp, float &ki, float &kd) {
    kp = _Kp;
    ki = _Ki;
    kd = _Kd;
}

void WiFiPIDTuner::printInfo() {
    Serial.println("\n========================================");
    Serial.println("     WiFi PID Tuner (Core 1)");
    Serial.println("========================================");
    Serial.print("SSID: "); Serial.println(_ssid);
    Serial.print("Password: "); Serial.println(_password);
    Serial.print("IP: "); Serial.println(WiFi.softAPIP());
    Serial.println("========================================\n");
}