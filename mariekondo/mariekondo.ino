#include <ESP8266WiFi.h>
#include "user_interface.h"
#define MOTIONSENSOR_PIN D7

#define LED_PIN 0
#include "AniMatrix.h"
AniMatrix matrix(64, 8, LED_PIN, 90.0); // 50fps is pushing it
uint16_t textColor;

#include "slogans.h"

void setup() {
  WiFi.forceSleepBegin();
  Serial.begin(115200);
  Serial.println();
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();
  gpio_pin_wakeup_enable(MOTIONSENSOR_PIN, GPIO_PIN_INTR_HILEVEL);
}

byte lastScene;
bool running = false;

void loop() {
  // TODO if matrix.nScenes == 0 but we're awake here -> queue new one
  if (!running) {
    Serial.println("New slogan");
    textColor = matrix.Color(80, 0, 0);
    lastScene = (lastScene + random(1, N - 1)) % N;
    matrix.queueScene(new TextScene(&matrix, slogans[lastScene], textColor, NULL));
    running = true;
  }
  Scene *expired = matrix.drawFrame();
  if (expired) {
    Serial.print("Slogan finished, pausing...");
    delete expired;
    running = false;
    // wait for at least 5 seconds
    delay(5000);
    // only sleep if the motion sensor is low+
    if (digitalRead(MOTIONSENSOR_PIN) == LOW) {
      Serial.println("sleeping.");
      wifi_fpm_do_sleep(0xFFFFFFF);
      delay(10);
    }
  }
}
