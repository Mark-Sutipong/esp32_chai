import network
import time
from umqtt.simple import MQTTClient
from machine import ADC, Pin

# Wi-Fi และ NETPIE Configurations
WIFI_SSID = "YOUR_WIFI_SSID"
WIFI_PASSWORD = "YOUR_WIFI_PASSWORD"
MQTT_BROKER = "broker.netpie.io"
MQTT_CLIENT_ID = "YOUR_MQTT_CLIENT_ID"  # Client ID จาก NETPIE
MQTT_USER = "YOUR_MQTT_USERNAME"       # Token Key
MQTT_PASSWORD = "YOUR_MQTT_PASSWORD"   # Token Secret
MQTT_TOPIC = "@shadow/data/update"     # Topic ที่จะส่งข้อมูล

# Pin Configuration
piezo_pin = ADC(Pin(34))  # กำหนดขา ADC สำหรับ Piezo (เช่น GPIO34 บน ESP32)
piezo_pin.width(ADC.WIDTH_12BIT)       # กำหนดความละเอียด ADC (12 บิต)
piezo_pin.atten(ADC.ATTN_11DB)         # ช่วงแรงดัน 0-3.6V

# ฟังก์ชันเชื่อมต่อ Wi-Fi
def connect_to_wifi():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(WIFI_SSID, WIFI_PASSWORD)
    print("Connecting to Wi-Fi", end="")
    while not wlan.isconnected():
        print(".", end="")
        time.sleep(1)
    print("\nWi-Fi connected!")
    print("IP Address:", wlan.ifconfig()[0])

# ฟังก์ชันเชื่อมต่อ MQTT
def connect_to_mqtt():
    client = MQTTClient(MQTT_CLIENT_ID, MQTT_BROKER, user=MQTT_USER, password=MQTT_PASSWORD)
    client.connect()
    print("Connected to MQTT Broker!")
    return client

# ฟังก์ชันหลัก
def main():
    # เชื่อมต่อ Wi-Fi
    connect_to_wifi()

    # เชื่อมต่อ MQTT
    client = connect_to_mqtt()

    while True:
        # อ่านค่าจากเซ็นเซอร์ Piezoelectric
        piezo_value = piezo_pin.read()
        print("Piezoelectric Value:", piezo_value)

        # สร้าง Payload และส่งข้อมูลไปยัง NETPIE
        payload = '{"piezoValue": %d}' % piezo_value
        client.publish(MQTT_TOPIC, payload)
        print("Published:", payload)

        time.sleep(1)  # หน่วงเวลา 1 วินาที

# เริ่มโปรแกรม
if __name__ == "__main__":
    main()
