#include "MySHT31.h"


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//================ Begin: ĐỌC CẢM BIẾN NHIỆT ĐỘ & ĐỘ ẨM ===================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void SHT3x::KhoiTaoSHT31(void) {
  // Cảm biến SHT31 đọc nhiệt độ và độ ẩm.
  // Chú ý: Khai báo sau các khai báo pinMode.
  //---------------------------------------------------------------
  sht31.begin();
  //---------------------------------------------------------------
}
void SHT3x::DocCamBienNhietDoVaDoAmSHT31() {
  sht31.read();
  NhietDo = sht31.getTemperature();
  NhietDo = double(round(NhietDo * 100) / 100);  // will round to 2 decimal places i.e. 0.00
  DoAm = sht31.getHumidity();
  DoAm = double(round(DoAm * 100) / 100);  // will round to 2 decimal places i.e. 0.00
  // Trường hợp không có kết nối cảm biến thì giá trị trả về là 
  // nhiệt độ = 130 (hoặc -45) & // độ ẩm = 100, lúc này trả về -1 
  // cho cả 2 thông số nhiệt độ & độ ẩm để trên app biết mà hiển thị 
  // trạng thái không có cảm biến kết nối với board để cho user biết.
  if (NhietDo >= 130 || NhietDo <= -45) {
    NhietDo = -1;
    DoAm = -1;
  }


#ifdef debug
  Serial.print("Nhiệt độ: ");
  Serial.println(NhietDo);
  Serial.print("Độ ẩm: ");
  Serial.println(DoAm);
#endif
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//================ End: ĐỌC CẢM BIẾN NHIỆT ĐỘ & ĐỘ ẨM =====================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#ifndef _RTC_h
#define _RTC_h


#include "IoTVision.h"  // Có define debug để bật/tắt các debug ra Serial.
#include <DS3231.h>      // Thư viện lấy thời gian thực (RTC)
#include <HTTPClient.h>  // Cho phép khai báo kiểu HTTPClient


typedef struct {
  int Thu;
  int Ngay;
  int Thang;
  int Nam;
  int Gio;
  int Phut;
  int Giay;
} ThoiGian;


class RTC {
private:
  HTTPClient http;
  DS3231 ds;


public:
  ThoiGian dt;


public:
  void KhoiTaoServerNTP(void);


private:
  // Lấy RTC từ nguồn server NTP hay IoTVision
  bool LayRTCTuServerNTP(void);
  bool LayRTCTuServerIoTVision(void);
  bool LayRTCTuServer(void);


public:
  void CaiDatRTCChoDS3231(void);
  void LayRTCTuServerCaiDatChoDS3231(void);
  void LayRTCTuDS3231(void);
  String ChuanHoaChuoiRTCDeGuiVeServer(void);
};


#endif


