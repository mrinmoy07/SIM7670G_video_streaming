#include "esp_camera.h"
#include <WiFi.h>

// Replace with your Wi-Fi credentials
const char* ssid = "CFCASSAM1";
const char* password = "90909090";

WiFiServer server(80);

// Camera pin definitions for ESP32-S3
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     34
#define SIOD_GPIO_NUM     15
#define SIOC_GPIO_NUM     16
#define Y9_GPIO_NUM       14
#define Y8_GPIO_NUM       13
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       11
#define Y5_GPIO_NUM       10
#define Y4_GPIO_NUM       9
#define Y3_GPIO_NUM       8
#define Y2_GPIO_NUM       7
#define VSYNC_GPIO_NUM    36
#define HREF_GPIO_NUM     35
#define PCLK_GPIO_NUM     37

void startCameraServer();

void setup() {
  Serial.begin(115200);
  Serial.println("\n[INFO] Starting ESP32-S3 Camera Initialization...");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("[INFO] Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[SUCCESS] Connected to Wi-Fi!");
  Serial.print("[INFO] ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure Camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 8000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("[ERROR] Camera init failed! Error code: 0x%x\n", err);
    return;
  }
  Serial.println("[SUCCESS] Camera initialized!");
  
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("[INFO] Client connected!");
    String request = client.readStringUntil('\r');
    client.flush();

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("[ERROR] Camera capture failed!");
      client.stop();
      return;
    }

    // Send HTTP Response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: image/jpeg");
    client.println("Content-Length: " + String(fb->len));
    client.println();
    client.write(fb->buf, fb->len);
    esp_camera_fb_return(fb);
    client.stop();
    Serial.println("[INFO] Image sent to client!");
  }
}

