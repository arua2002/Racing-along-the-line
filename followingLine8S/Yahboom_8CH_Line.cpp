#include "Yahboom_8CH_Line.hpp"
#include <stdio.h>

// Для ESP32 используем HardwareSerial вместо SoftwareSerial
// Подключите модуль: RX -> GPIO16, TX -> GPIO17
#define IR_SERIAL Serial2

#define Package_size 100
#define printSerial Serial

char send_buf[35] = {0};
uint8_t rx_buff[Package_size];
uint8_t new_package[Package_size];
uint8_t g_new_package_flag = 0;

uint8_t IR_Data_number[IR_Num];
uint16_t IR_Data_Anglo[IR_Num];
uint8_t g_Amode_Data = 0;
uint8_t g_Dmode_Data = 0;

void serial_init(void) {
    // Для ESP32: Serial2.begin(baud, config, RX, TX)
    IR_SERIAL.begin(115200, SERIAL_8N1, 16, 17);  // RX=16, TX=17
    printSerial.begin(115200);
    delay(100);
}

void recv_data(void) {
    char strr;
    if (IR_SERIAL.available()) {
        strr = char(IR_SERIAL.read());
        Data_Deal(strr);
    }
}

void SET_Eight_Mode(uint8_t adjust, uint8_t ao, uint8_t num) {
    sprintf(send_buf, "$%d,%d,%d#", adjust, ao, num);
    IR_SERIAL.print(send_buf);
    memset(send_buf, 0, sizeof(send_buf));
    delay(300);
}

void Data_Deal(char rxtemp) {
    static uint8_t g_start = 0;
    static uint8_t step = 0;
    
    if (rxtemp == '$') {
        g_start = 1;
        rx_buff[step] = rxtemp;
        step++;
    } else {
        if (g_start == 0) {
            return;
        } else {
            rx_buff[step] = rxtemp;
            step++;
            if (rxtemp == '#') {
                g_start = 0;
                step = 0;
                memcpy(new_package, rx_buff, Package_size);
                g_new_package_flag = 1;
                memset(rx_buff, 0, Package_size);
            }
            
            if (step >= Package_size) {
                g_start = 0;
                step = 0;
                memset(rx_buff, 0, Package_size);
            }
        }
    }
}

void Deal_Usart_Data(void) {
    if (new_package[1] != 'D') {
        return;
    }
    
    for (uint8_t i = 0; i < IR_Num; i++) {
        IR_Data_number[i] = (new_package[6 + i * 5] - '0');
    }
    
    // Отладочный вывод (закомментируйте для экономии скорости)
    // sprintf(send_buf, "x1:%d x2:%d x3:%d x4:%d x5:%d x6:%d x7:%d x8:%d\r\n",
    //         IR_Data_number[0], IR_Data_number[1], IR_Data_number[2], IR_Data_number[3],
    //         IR_Data_number[4], IR_Data_number[5], IR_Data_number[6], IR_Data_number[7]);
    // printSerial.print(send_buf);
    
    memset(new_package, 0, Package_size);
}

void splitString(char* mystrArray[], char *str, const char *delimiter) {
    char *token = strtok(str, delimiter);
    int i = 0;
    while (token != NULL) {
        token = strtok(NULL, delimiter);
        mystrArray[i] = token;
        i++;
    }
}

void Deal_Usart_AData(void) {
    if (new_package[1] != 'A') {
        return;
    }
    
    char* strArray[10];
    char* strArraytemp[2];
    char str_temp[Package_size] = {'\0'};
    char mystr_temp[8][10] = {'\0'};
    
    strncpy(str_temp, (char*)new_package, strlen((char*)new_package) - 1);
    splitString(strArray, str_temp, ", ");
    
    for (int i = 0; i < IR_Num; i++) {
        if (strArray[i] != NULL) {
            strcpy(mystr_temp[i], strArray[i]);
            splitString(strArraytemp, mystr_temp[i], ": ");
            if (strArraytemp[0] != NULL) {
                IR_Data_Anglo[i] = atoi(strArraytemp[0]);
            }
        }
    }
    
    memset(new_package, 0, Package_size);
    memset(strArray, 0, sizeof(strArray));
    memset(strArraytemp, 0, sizeof(strArraytemp));
}