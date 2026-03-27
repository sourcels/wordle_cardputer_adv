#include <Arduino.h>
#include "M5Cardputer.h"
#include "logic.h"
#include "ui.h"
#include "words.h"

#ifdef CARDPUTER_ADV
#include <utility/Keyboard/KeyboardReader/TCA8418.h>
#endif

TaskHandle_t handleUITask = NULL;
TaskHandle_t handleLogicTask = NULL;

void Task_TFT(void *pvParameters);
void Task_Logic(void *pvParameters);

void setup() {
    m5::M5Unified::config_t cfg = M5.config();
    cfg.serial_baudrate = 115200;
    cfg.internal_mic = false;
    cfg.internal_spk = true;

    M5Cardputer.begin(cfg, true);
#ifdef CARDPUTER_ADV
    std::unique_ptr<KeyboardReader> reader(new TCA8418KeyboardReader());
    M5Cardputer.Keyboard.begin(std::move(reader));
#endif


    xTaskCreatePinnedToCore(Task_TFT, "Task_TFT", 20480, NULL, 2, &handleUITask, 0);
    xTaskCreatePinnedToCore(Task_Logic, "Task_Logic", 12288, NULL, 1, &handleLogicTask, 1);
}

void loop() {
    
}