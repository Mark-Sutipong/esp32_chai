#include <MD_MAX72xx.h>
#include <SPI.h>
#include <WiFi.h>
#include <TM1637Display.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <LiquidCrystal.h>
#include <AccelStepper.h>
#include <Adafruit_NeoPixel.h>

// ขาพินเชื่อมต่อ TM1637
#define CLK 22
#define DIO 21

// ขาพินสำหรับ MD_MAX72XX
#define DATA_PIN   23
#define CLK_PIN    18
#define CS_PIN     5
#define MAX_DEVICES 8

// ขาพินสำหรับ HC-SR04
#define trigPin 17
#define echoPin 16

// กำหนดขาเชื่อมต่อ
#define IN1 18
#define IN2 19
#define IN3 21
#define IN4 22
#define NUM_LEDS 4
#define LED_DATA_PIN 5

// ข้อมูลการเชื่อมต่อ Wi-Fi
const char* ssid = "ESP32_AP";
const char* password = "12345678";

// การเลื่อนเวลาตามโซนเวลา UTC+7
const long utcOffsetInSeconds = 7 * 3600;

TM1637Display display(CLK, DIO);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds, 60000);
MD_MAX72XX mx = MD_MAX72XX(MD_MAX72XX::FC16_HW, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
WiFiServer server(80);

AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN3, IN2, IN4);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);

// ประกาศ LCD
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

void setup() {
  Serial.begin(115200);
  pinMode(CS_PIN, OUTPUT);
  pinMode(CLK_PIN, INPUT);

  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  server.begin();

  mx.begin();
  mx.clear();
  display.setBrightness(0x0f);

  timeClient.begin();

  lcd.begin(16, 2);

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);
  
  strip.begin();
  strip.show(); // ตั้งค่าเริ่มต้นเป็นไฟดับ
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("Hello from ESP32 Server!");
          }
          currentLine = "";
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }

  timeClient.update();

  int hour = timeClient.getHours();
  int minute = timeClient.getMinutes();
  int displayTime = hour * 100 + minute;
  display.showNumberDec(displayTime, false);
}
