#include <Arduino.h>
#include "M5Cardputer.h"
#include <utility/Keyboard/KeyboardReader/TCA8418.h>
#include "logic.h"
#include "ui.h"
#include "words.h"

TaskHandle_t handleUITask = NULL;
TaskHandle_t handleAudioTask = NULL;

void Task_TFT(void *pvParameters);
void Task_Audio(void *pvParameters);

void setup() {
    m5::M5Unified::config_t cfg = M5.config();
    cfg.serial_baudrate = 115200;
    cfg.internal_mic = false;
    cfg.internal_spk = false;

    M5Cardputer.begin(cfg, true);
    Serial.println("Configuring TCA8418 keyboard driver");
    std::unique_ptr<KeyboardReader> reader(new TCA8418KeyboardReader());
    M5Cardputer.Keyboard.begin(std::move(reader));


    xTaskCreatePinnedToCore(Task_TFT, "Task_TFT", 20480, NULL, 2, &handleUITask, 0);
    xTaskCreatePinnedToCore(Task_Logic, "Task_Logic", 12288, NULL, 1, NULL, 1);
}

void loop() {
    
}
