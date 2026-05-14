#ifndef YAHBOOM_8CH_LINE_HPP
#define YAHBOOM_8CH_LINE_HPP

#include <Arduino.h>

#define IR_Num 8

// Глобальные переменные (объявлены в .cpp)
extern uint8_t IR_Data_number[IR_Num];
extern uint16_t IR_Data_Anglo[IR_Num];
extern uint8_t g_Amode_Data;
extern uint8_t g_Dmode_Data;
extern uint8_t g_new_package_flag;

// Функции для работы с датчиками
void serial_init(void);
void recv_data(void);
void SET_Eight_Mode(uint8_t adjust, uint8_t ao, uint8_t num);
void Data_Deal(char RXdata);
void Deal_Usart_Data(void);
void Deal_Usart_AData(void);
void splitString(char* mystrArray[], char *str, const char *delimiter);

#endif // YAHBOOM_8CH_LINE_HPP