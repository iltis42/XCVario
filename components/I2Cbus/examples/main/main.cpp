/* =========================================================================
The MIT License (MIT)

Copyright 2017 Natanael Josue Rabello. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
 ========================================================================= */

#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "I2Cbus.hpp"



extern "C" void app_main() {
    printf(">I2Cbus Example \n");
    fflush(stdout);

    I2C_t& myI2C = i2c0;  // i2c0 and i2c1 are the default objects
    
    ESP_ERROR_CHECK( myI2C.begin(GPIO_NUM_21, GPIO_NUM_22, 400000));
    myI2C.setTimeout(10);
    myI2C.scanner();

    myI2C.writeBit(0x69, 0x6B, 6, 0);
    
    uint8_t buffer[6];
    while (1) {
        myI2C.readBytes(0x68, 0x6F, 6, buffer);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
    myI2C.close();
    vTaskDelay(portMAX_DELAY);
}
