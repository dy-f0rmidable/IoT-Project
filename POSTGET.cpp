#include "POSTGET.h"
#include <WiFi.h>  // Thư viện WiFi của ESP32.


#define URL_POSTDuLieuTuBoardLenServer "http://App.IoTVision.vn/api/BoardSTR423_DuLieu"


// Thiết lập thời gian timeout.
#define timeout 1500 // 1,5s (có thể thay đổi con số này)
//===================================================================
// Ví dụ, DuLieu = "11100;23.5;82.9;19:53:13 02/08/2023"
// 11100 => K1 = 1, K2 = 1, K3 = 1, K4 = 0, MODE = 0 (MAN)
// Nhiệt độ = 23.5
// Độ ẩm = 82.9
// HH:MM:SS DD/MM/YYYY = 19:53:13 02/08/2023
//===================================================================
bool POSTGET::POSTDuLieuBoard(String ID, String DuLieu) {
  if (WiFi.status() == WL_CONNECTED) {
    try {
    if (WiFi.status() == WL_CONNECTED) {
      this->http.setTimeout(timeout);  // Thiết lập thời gian timeout.
      this->http.begin(URL_POSTDuLieuTuBoardLenServer);
      this->http.addHeader("Content-Type", "application/json");
      String data = "{\"ID\":\"" + ID + "\",\"S\":\"" + DuLieu + "\"}";
      this->http.POST(data);  //Send the request
      this->http.end();


      return 1;
    } else
      return 0;
  } catch (String error) {
    return 0;
  }
}
}
