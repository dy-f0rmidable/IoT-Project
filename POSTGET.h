#ifndef _POSTGET_h
#define _POSTGET_h


#include <Arduino.h>      // Để khai báo kiểu String
#include <HTTPClient.h>   // Cho phép khai báo kiểu HTTPClient


class POSTGET {
private:
  HTTPClient http;


public:
  bool POSTDuLieuBoard(String ID, String DuLieu);
};
#endif