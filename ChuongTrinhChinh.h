#ifndef _ChuongTrinhChinh_h
#define _ChuongTrinhChinh_h


#include "IoTVision.h"  // Có define debug để bật/tắt các debug ra Serial.
#include <Arduino.h>
#include <Arduino_JSON.h>  // Thư viện xử lý dữ liệu kiểu JSON
#include <Wire.h>          // Để kết nối I2C với mô-đun RTC (thời gian thực),
                           // mô-đun đọc cảm biến nhiệt độ & độ ẩm SHT3x.


#define pinSCL 18 // Chân SCL của I2C nối cảm biến nhiệt độ & độ ẩm SHT3x
#define pinSDA 19 // Chân SDA của I2C nối cảm biến nhiệt độ & độ ẩm SHT3x


void KhoiTao(void);               
void ChayChuongTrinhChinh(void);  
void POSTDuLieuVeCloudDeHienThiTrenAPP(void);


#endif
