#ifndef WIFI_PID_TUNER_H
#define WIFI_PID_TUNER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

class WiFiPIDTuner {
private:
    WebServer* _server;
    const char* _password;
    String _ssid;
    
    // HTML страница
    const char* _htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PID Tuner</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            margin: 0;
            padding: 20px;
            min-height: 100vh;
        }
        .container {
            max-width: 500px;
            margin: 0 auto;
            background: rgba(255,255,255,0.1);
            padding: 30px;
            border-radius: 20px;
            backdrop-filter: blur(10px);
        }
        h1 {
            text-align: center;
            margin-bottom: 30px;
        }
        .param {
            margin-bottom: 25px;
        }
        label {
            display: block;
            font-size: 18px;
            margin-bottom: 10px;
        }
        .value-display {
            float: right;
            font-weight: bold;
        }
        input[type="range"] {
            width: 100%;
            height: 8px;
            border-radius: 5px;
            background: rgba(255,255,255,0.3);
            outline: none;
            -webkit-appearance: none;
        }
        input[type="range"]::-webkit-slider-thumb {
            -webkit-appearance: none;
            width: 25px;
            height: 25px;
            border-radius: 50%;
            background: white;
            cursor: pointer;
            box-shadow: 0 2px 10px rgba(0,0,0,0.2);
        }
        .presets {
            display: flex;
            gap: 10px;
            margin: 20px 0;
            flex-wrap: wrap;
        }
        button {
            background: rgba(255,255,255,0.2);
            color: white;
            border: 2px solid white;
            padding: 12px 24px;
            border-radius: 10px;
            font-size: 16px;
            cursor: pointer;
            transition: all 0.3s;
            flex: 1;
            min-width: 100px;
        }
        button:hover {
            background: white;
            color: #667eea;
        }
        .apply-btn {
            background: #4CAF50;
            border-color: #4CAF50;
            font-size: 18px;
            font-weight: bold;
        }
        .apply-btn:hover {
            background: #45a049;
            color: white;
        }
        .status {
            text-align: center;
            margin-top: 20px;
            padding: 10px;
            border-radius: 10px;
            background: rgba(0,0,0,0.2);
        }
        .current-values {
            text-align: center;
            padding: 15px;
            background: rgba(0,0,0,0.3);
            border-radius: 10px;
            margin-top: 20px;
        }
        .value {
            font-size: 24px;
            font-weight: bold;
            font-family: monospace;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🚗 PID Tuner</h1>
        
        <div class="param">
            <label>
                Kp (Пропорциональный)
                <span class="value-display" id="kp-value">0.80</span>
            </label>
            <input type="range" id="kp" min="0" max="2" step="0.01" value="0.80">
        </div>
        
        <div class="param">
            <label>
                Ki (Интегральный)
                <span class="value-display" id="ki-value">0.000</span>
            </label>
            <input type="range" id="ki" min="0" max="0.5" step="0.001" value="0.000">
        </div>
        
        <div class="param">
            <label>
                Kd (Дифференциальный)
                <span class="value-display" id="kd-value">0.30</span>
            </label>
            <input type="range" id="kd" min="0" max="1" step="0.01" value="0.30">
        </div>
        
        <div class="presets">
            <button onclick="setPreset('slow')">🐢 Медленно</button>
            <button onclick="setPreset('medium')">⚡ Средне</button>
            <button onclick="setPreset('fast')">🚀 Быстро</button>
        </div>
        
        <button class="apply-btn" onclick="applyValues()">✅ ПРИМЕНИТЬ</button>
        
        <div class="current-values">
            <div style="margin-bottom: 10px;">📊 Текущие значения:</div>
            <div style="display: flex; justify-content: space-around;">
                <div>Kp: <span id="current-kp" class="value">0.80</span></div>
                <div>Ki: <span id="current-ki" class="value">0.000</span></div>
                <div>Kd: <span id="current-kd" class="value">0.30</span></div>
            </div>
        </div>
        
        <div class="status" id="status">
            ⏺️ Готов к настройке
        </div>
    </div>
    
    <script>
        document.getElementById('kp').addEventListener('input', function() {
            document.getElementById('kp-value').textContent = this.value;
        });
        
        document.getElementById('ki').addEventListener('input', function() {
            document.getElementById('ki-value').textContent = parseFloat(this.value).toFixed(3);
        });
        
        document.getElementById('kd').addEventListener('input', function() {
            document.getElementById('kd-value').textContent = this.value;
        });
        
        function loadCurrentValues() {
            fetch('/get')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('current-kp').textContent = data.kp.toFixed(2);
                    document.getElementById('current-ki').textContent = data.ki.toFixed(3);
                    document.getElementById('current-kd').textContent = data.kd.toFixed(2);
                    
                    document.getElementById('kp').value = data.kp;
                    document.getElementById('ki').value = data.ki;
                    document.getElementById('kd').value = data.kd;
                    
                    document.getElementById('kp-value').textContent = data.kp.toFixed(2);
                    document.getElementById('ki-value').textContent = data.ki.toFixed(3);
                    document.getElementById('kd-value').textContent = data.kd.toFixed(2);
                });
        }
        
        function applyValues() {
            const kp = document.getElementById('kp').value;
            const ki = document.getElementById('ki').value;
            const kd = document.getElementById('kd').value;
            
            fetch(`/set?kp=${kp}&ki=${ki}&kd=${kd}`)
                .then(response => response.text())
                .then(data => {
                    document.getElementById('status').innerHTML = '✅ ' + data;
                    loadCurrentValues();
                });
        }
        
        function setPreset(preset) {
            if (preset === 'slow') {
                document.getElementById('kp').value = '0.40';
                document.getElementById('ki').value = '0.000';
                document.getElementById('kd').value = '0.20';
            } else if (preset === 'medium') {
                document.getElementById('kp').value = '0.80';
                document.getElementById('ki').value = '0.010';
                document.getElementById('kd').value = '0.30';
            } else if (preset === 'fast') {
                document.getElementById('kp').value = '1.20';
                document.getElementById('ki').value = '0.020';
                document.getElementById('kd').value = '0.50';
            }
            
            document.getElementById('kp-value').textContent = document.getElementById('kp').value;
            document.getElementById('ki-value').textContent = parseFloat(document.getElementById('ki').value).toFixed(3);
            document.getElementById('kd-value').textContent = document.getElementById('kd').value;
        }
        
        loadCurrentValues();
        setInterval(loadCurrentValues, 2000);
    </script>
</body>
</html>
)rawliteral";

    // Обработчики запросов
    void handleRoot();
    void handleSet();
    void handleGet();
    
public:
    WiFiPIDTuner(const char* password = "12345678");
    ~WiFiPIDTuner();
    
    void begin();
    void handleClient();
    void getPID(float &kp, float &ki, float &kd);
    void printInfo();
};

#endif