#include "ChuongTrinhChinh.h"
#include "WIFI.h"
#include "POSTGET.h"
#include "RTC.h"
#include "MySHT31.h"
#include "Relay.h"


#define CoKetNoiServer // Khai báo để dùng tính năng POST/GET với server.
#define xTaskPOSTDuLieuVeCloud // Để thực thi task POST dữ liệu 
//------------------------------------------------------------------------
#define CORE_1 1     // core 1 
#define CORE_0 0     // core 0
#define TASK_POST 1  //
#define TASK_GET 0   //
//------------------------------------------------------------------------
WIFI _WiFi;       // Kết nối WiFi cho board.
POSTGET _POSTGET; // Các hàm thực thi POST - GET giữa device và serve.
RTC _RTC;         // Mô-đun thời gian thực DS3231.
SHT3x _SHT31;     // Cảm biến nhiệt độ và độ ẩm SHT21.
Relay _Relay;     // Điều khiển rơ-le
//------------------------------------------------------------------------
String _ID; // Số ID của ESP32, đây là số IMEI của board.
TaskHandle_t _POST, _GET;  // Xác định địa chỉ task để thao tác như xóa task.
SemaphoreHandle_t _mutexPostGet; // Khóa không cho post get cùng lúc.
unsigned long _tickPOSTDuLieu = 0; // Đếm để xác định thời điểm gọi hàm POSTDuLieuVeCloudDeHienThiTrenAPP()
//------------------------------------------------------------------------


#pragma region Tasks
//========================================================================
#pragma region TaskPOSTDuLieuVeCloud
#ifdef xTaskPOSTDuLieuVeCloud
void TaskPOSTDuLieuVeCloud(void* pvParameter) {
  for (;;) {
    if (xSemaphoreTake(_mutexPostGet, portMAX_DELAY) == pdTRUE) {
      // Nếu đếm đủ thời điểm đã thiết lập cho phép POST dữ liệu 
      // lên cloud (ex: 60000 => 1 phút)
      if (_tickPOSTDuLieu >= 6000) {
        _tickPOSTDuLieu = 0;
        POSTDuLieuVeCloudDeHienThiTrenAPP();
      }
      _tickPOSTDuLieu++;


      xSemaphoreGive(_mutexPostGet);
      vTaskDelay(pdMS_TO_TICKS(1));  // 1ms

#ifdef debug
Serial.printf("Kích thước task POST %u\n", uxTaskGetStackHighWaterMark(NULL));
      Serial.printf("_tickPOSTDuLieu %u\n", _tickPOSTDuLieu);
#endif
    }
  }
}
#endif
#pragma endregion TaskPOSTDuLieuVeCloud
//========================================================================
#pragma endregion Tasks


void KhoiTao(void) {
#pragma region KhoiTao
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#pragma region Các khởi tạo cơ bản của máy      
  // Trước tiên cần phải khởi tạo các PINs.
  _Relay.KhoiTaoCacChan();
  //---------------------------------------------------------------
  // Khi vừa cấp nguồn cho board thì tất cả rơle điều khiển các
  // thiết bị trong hệ thống đều phải OFF để đảm bảo các thiết bị
  // OFF ngay thời điểm mở máy lên.
  _Relay.OFFCacRole();


  _Relay.MODE = _MANUAL; // Với board ví dụ này thì MODE là MANUAL.
  //---------------------------------------------------------------


  //---------------------------------------------------------------
  // Khởi tạo giao tiếp SERIAL
  Serial.begin(115200);
  //Serial.begin(9600);
  Serial.println("");


  //---------------------------------------------------------------
  // Để có thể kết nối với các mô-đun đọc cảm biến SHT3x,
  // mô-đun thời gian thực, mô-đun mở rộng port PCF8574
  // thì trước tiên cần phải khởi tạo giao tiếp I2C
  // (Start the I2C interface firtly).
  Wire.setPins(pinSDA, pinSCL);
  Wire.begin();
  //---------------------------------------------------------------


  // Số ID (ID) của ESP32.
  _ID = _WiFi.LaySoMAC(); 


#ifdef debug
  Serial.print("ID: ");
  Serial.println(_ID);
#endif


//----------------------------------------------------------------
  // Cảm biến SHT31 đọc nhiệt độ và độ ẩm.
  // Chú ý: Khai báo sau các khai báo pinMode ở trên.
  _SHT31.KhoiTaoSHT31();
  //----------------------------------------------------------------


  //---------------------------------------------------------------
#pragma endregion Các khởi tạo cơ bản của máy

#ifdef CoKetNoiServer
#pragma region Khởi tạo WIFI
  //======================================================================
  //------ Begin: Khởi tạo để có thể cấu hình kết nối WiFi tự động -----//
  //======================================================================
  // Dành 10s để kết nối WiFI
  // Lưu ý: Phải có thời gian chờ cho việc kết nối WIFI nếu không sẽ
  // gây ra tình trạng board bị reset và không thể phát access point (AP).
  // Hàm thực hiện kết nối WiFi đồng thời khởi động chạy Task để sau mỗi
  // chu kỳ t(ms) thì sẽ gọi hàm KiemTraKetNoiWiFi một lần.
  _WiFi.KetNoiWiFi(10);
  //======================================================================
  //------ End: Khởi tạo để có thể cấu hình kết nối WiFi tự động -------//
  //======================================================================
#pragma endregion Khởi tạo WIFI
#endif


#pragma region TaskMultiCore
  // khởi tạo khóa không cho post get cùng lúc
  _mutexPostGet = xSemaphoreCreateMutex();  
  delay(100);


//--------------------------------------------------------------------------
#ifdef xTaskPOSTDuLieuVeCloud
  xTaskCreatePinnedToCore(
    TaskPOSTDuLieuVeCloud,                    /* Task function. */
    "Task POSTDuLieuVeCloudDeHienThiTrenAPP", /* name of task. */
    10000,                                    /* Stack size of task */
    NULL,                                     /* parameter of the task */
    6,                                        /* priority of the task */
    &_POST,                                   /* Task handle to keep track of created task */
    CORE_1);                                  /* pin task to core 1 */
  delay(500);
#endif
//--------------------------------------------------------------------------


#pragma endregion TaskMultiCore


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#pragma endregion KhoiTao
}


void ChayChuongTrinhChinh(void) {
  delay(1);
}

#ifdef xTaskPOSTDuLieuVeCloud
// Cập nhật dữ liệu về cloud để hiển thị trên APP.
void POSTDuLieuVeCloudDeHienThiTrenAPP(void) {
#pragma region POSTDuLieuVeCloudDeHienThiTrenAPP


#ifdef CoKetNoiServer
  if (WiFi.status() == WL_CONNECTED) {
#pragma region Chuỗi dữ liệu sẽ gửi về server
    // Lấy thời gian thực trên board.
    //_RTC.LayRTCTuDS3231();
    _RTC.LayRTCTuServerCaiDatChoDS3231();


    // Đọc giá trị cảm biến nhiệt độ và độ ẩm SHT31.
    _SHT31.DocCamBienNhietDoVaDoAmSHT31();


    //===================================================================
    // Chuỗi dulieu sẽ gồm: K1K2K3K4MODERSSI;NhietDo;DoAm;RTC
    // Ví dụ: 1000077;23.5;82.9;12:26:13 12/04/2024
    // Nghĩa là:
    //  K1 = 1, K2 = 0, K3 = 0, K4 = 0, MODE = 0 (MAN)
    // RSSI = 77
    // Nhiệt độ = 23,5
    // Độ ẩm = 82,9
    // RTC = 12/04/2024 12:26:13
    //===================================================================
    //===================================================================
    // s00: K1 = 1 : Relay 1 điều khiển ON/OFF K1 đang ON
    // s01: K2 = 0 : Relay 2 điều khiển ON/OFF K2 đang OFF
    // s02: K3 = 0 : Relay 2 điều khiển ON/OFF K3 đang OFF
    // s03: K4 = 0 : Relay 2 điều khiển ON/OFF K4 đang OFF
    // s04: MODE = 0 : Chế độ điều khiển đang là MANUAL (bằng tay).
    // s05: Độ mạnh WiFI = 77%
    // s1: Nhiệt độ = 23.5 (độ C)
    // s2: Độ ẩm = 82.9 (%RH)
    // s3: Thời gian thực RTC trên board = 12:26:13 12/04/2024
    String s00 = String(_Relay.K1);
    String s01 = String(_Relay.K2);
    String s02 = "0"; // Do board này không có relay K3
    String s03 = "0"; // Do board này không có relay K4
    String s04 = String(_Relay.MODE);
    String s05 = String(_WiFi.TinhDoManhCuaWiFi());
    String s1 = String(_SHT31.NhietDo);
    String s2 = String(_SHT31.DoAm);
    String s3 = _RTC.ChuanHoaChuoiRTCDeGuiVeServer()      ;
    String data = s00+s01+s02+s03+s04+s05+";"+s1+";"+s2+";"+s3;
    //-----------------------------------------------------------------------------------------
#pragma endregion Chuỗi dữ liệu sẽ gửi về server


#ifdef debug
    Serial.println("Dữ liệu POST lên server:");
    Serial.println(data);
#endif


    _POSTGET.POSTDuLieuBoard(_ID, data);
  } else {
#ifdef debug
    Serial.println("Không có WIFI để POST dữ liệu lên server...");
#endif
  }
#endif  // CoKetNoiServer


#pragma endregion POSTDuLieuVeCloudDeHienThiTrenAPP
}
#endif  // xTaskPOSTDuLieuVeCloud


