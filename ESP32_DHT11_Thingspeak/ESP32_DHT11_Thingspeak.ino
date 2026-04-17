#include <WiFi.h>
#include <HTTPClient.h>
#include <SimpleDHT.h>

/* =========================
   WiFi 設定
   ========================= */
char ssid[] = "wifi";
char password[] = "password";

/* =========================
   ThingSpeak API 設定
   請填入你的 Write API Key
   ========================= */
String url = "http://api.thingspeak.com/update?api_key=API_KEY";

/* =========================
   DHT11 腳位設定
   ========================= */
int pinDHT11 = 13;   // DHT11 接 GPIO13

/* 建立 DHT11 物件 */
SimpleDHT11 dht11(pinDHT11);


/* ==================================================
   初始化
   ================================================== */
void setup() {
  Serial.begin(115200);

  Serial.print("開始連線到 WiFi：");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println();
  Serial.println("WiFi 連線成功！");
}


/* ==================================================
   主程式循環
   ================================================== */
void loop() {

  Serial.print("目前核心編號：");
  Serial.println(xPortGetCoreID());

  /* -------- 讀取溫濕度 -------- */
  byte temperature = 0;
  byte humidity = 0;

  int err = dht11.read(&temperature, &humidity, NULL);

  if (err != SimpleDHTErrSuccess) {
    Serial.print("DHT11 讀取失敗，錯誤碼：");
    Serial.println(err);
    delay(1000);
    return;
  }

  /* -------- 顯示資料 -------- */
  Serial.print("溫度：");
  Serial.print((int)temperature);
  Serial.print(" °C   ");

  Serial.print("濕度：");
  Serial.print((int)humidity);
  Serial.println(" %");


  /* -------- 上傳到 ThingSpeak -------- */
  Serial.println("開始上傳資料...");

  HTTPClient http;

  String url1 = url +
                "&field1=" + String((int)temperature) +
                "&field2=" + String((int)humidity);

  http.begin(url1);

  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();

    Serial.print("上傳成功，回傳內容：");
    Serial.println(payload);
  } 
  else {
    Serial.print("上傳失敗，錯誤碼：");
    Serial.println(httpCode);
  }

  http.end();

  /* -------- 等待 5 分鐘 -------- */
  delay(300000);
}