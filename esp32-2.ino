#include <WiFi.h>
#include <WebServer.h>
#include <Keypad.h>

const char* ssid = "Esp32_com1";
const char* password = "080451mark";

WebServer server(80);  // สร้างเซิร์ฟเวอร์บนพอร์ต 80

// กำหนดพินสำหรับเซนเซอร์ HC-SR04
#define TRIG_PIN 32
#define ECHO_PIN 33

#define SHARP_SENSOR_PIN 34
#define LDR_PIN 35

// กำหนดขนาดของ Keypad (4 แถวและ 3 คอลัมน์)
const byte ROWS = 4;
const byte COLS = 3;

// กำหนด layout ของปุ่มกดบน Keypad
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

// กำหนด PIN ของ ESP32S ที่เชื่อมต่อกับแถวและคอลัมน์ของ Keypad
byte rowPins[ROWS] = {13, 12, 14, 27}; // แก้ไขตามการเชื่อมต่อจริง
byte colPins[COLS] = {26, 25, 33}; // แก้ไขตามการเชื่อมต่อจริง

// สร้างตัวแปร Keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ฟังก์ชันสำหรับรับค่าจาก Keypad และส่งกลับไปยัง Client
void handleKey() {
  char key = keypad.getKey();  // รับค่าจาก Keypad
  String response;
  
  if (key) {  // ถ้ามีการกดปุ่ม
    response = "Key: " + String(key);  // สร้างข้อความตอบกลับ
  } else {
    response = "No key pressed";  // กรณีไม่ได้กดปุ่มใด
  }

  server.send(200, "text/plain", response);  // ส่งการตอบกลับไปยัง Client
}

void handleRoot() {
  server.send(200, "text/plain", "Hello from Client ESP32!");  // ส่งการตอบกลับเมื่อได้รับคำขอที่ "/"
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // ตั้งค่าการจัดการคำขอที่ /distance
  server.on("/distance", HTTP_GET, [](){
    long duration;
    float distance;

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH);
    distance = (duration / 2) / 29.1;

    String distanceStr = String(distance);
    server.send(200, "text/plain", distanceStr);
  });

  server.on("/", handleRoot);  // ตั้งค่าหน้าหลัก (root) ให้เรียกใช้ฟังก์ชัน handleRoot
  server.on("/key", handleKey);  // ตั้งค่าการจัดการคำขอที่ /key

  server.begin();  // เริ่มเซิร์ฟเวอร์
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();  // รอรับคำขอจากไคลเอนต์
}
