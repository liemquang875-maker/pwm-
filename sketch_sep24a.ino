#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

// Thông tin Wi-Fi
const char* WIFI_SSID = "Long Map 1";
const char* WIFI_PASSWORD = "ngophulong181001";

// Thông tin Firebase của bạn
#define FIREBASE_HOST "https://pwm-757e4-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "YOUR_DATABASE_SECRET" // Thay secret key tại đây

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

const int ledPin = 4; // GPIO4 (D2)
int lastPwmValue = -1;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, 0);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");

  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Lắng nghe nút 'pwmValue' trên Database
  if (!Firebase.beginStream(firebaseData, "/pwmValue")) {
    Serial.println("Stream error: " + firebaseData.errorReason());
  }
}

void loop() {
  if (Firebase.readStream(firebaseData)) {
    if (firebaseData.streamAvailable() && firebaseData.dataType() == "int") {
      int sliderVal = firebaseData.intData(); // Nhận giá trị 0-255
      
      if (sliderVal != lastPwmValue) {
        lastPwmValue = sliderVal;
        int brightness = map(sliderVal, 0, 255, 0, 1023); // Ánh xạ sang thang 0-1023 của ESP
        analogWrite(ledPin, brightness);
        Serial.printf("PWM: %d -> Duty: %d\n", sliderVal, brightness);
      }
    }
  }
  
  if (firebaseData.streamTimeout()) {
    Serial.println("Stream timeout, retrying...");
  }
}