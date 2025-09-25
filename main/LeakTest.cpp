#include "LeakTest.h"

#include "IpsDisplay.h"
#include "ESPRotary.h"
#include "logdefnone.h"

#define LOOPS 150


void LeakTest::start(PressureSensor* bmpBA, PressureSensor* bmpTE, AirspeedSensor* asSensor) {
	ESP_LOGI(FNAME, "Starting Leak test");

	// Display initialization
	xSemaphoreTake(display_mutex, portMAX_DELAY);
	Display->clear();
	Display->writeText(1, "** Leak Test **");
	xSemaphoreGive(display_mutex);

	// Constants for thresholds
	constexpr float ST_THRESHOLD   = 0.1f;   // %
	constexpr float TE_THRESHOLD   = 0.1f;   // %
	constexpr float SPEED_THRESHOLD = 1.0f;  // %
	constexpr float MIN_SPEED      = 10.0f;  // Pa
	constexpr int   MAX_ITER       = 24;     // 24 * 5s = 120s
	constexpr float INV_LOOPS      = 1.0f / LOOPS;

	// Baseline values
	float sba = 0.0f, ste = 0.0f, sspeed = 0.0f;
	bool failed = false;

	for (int i = 0; i < MAX_ITER; i++) {
		float ba = 0.0f, te = 0.0f, speed = 0.0f;
		int loops_run = 0;

		// Collect readings
		for (int j = 0; j < LOOPS; j++) {
			bool okSpeed = false;
			if (asSensor) {
				float s = asSensor->readPascal(5, okSpeed);
				if (okSpeed) {
					speed += s;
					loops_run++;
				}
			}
			bool ok1 = false, ok2 = false;
			ba += bmpBA->readPressure(ok1);
			te += bmpTE->readPressure(ok2);
			delay(33);
		}

		ba *= INV_LOOPS;
		te *= INV_LOOPS;
		if (loops_run > 0) speed /= loops_run;

		// Initialize baselines
		if (i == 0) {
			sba = ba;
			ste = te;
			sspeed = speed;
			ESP_LOGI(FNAME, "Baseline -> ST %.2f  TE %.2f  PI %.2f", ba, te, speed);
		}

		// Display sensor values
		char buf[40];
		xSemaphoreTake(display_mutex, portMAX_DELAY);
		snprintf(buf, sizeof(buf), "ST P: %3.2f hPa", ba);
		Display->writeText(2, buf);
		snprintf(buf, sizeof(buf), "TE P: %3.2f hPa", te);
		Display->writeText(3, buf);
		snprintf(buf, sizeof(buf), "PI P: %3.2f Pa", speed);
		Display->writeText(4, buf);

		if (i >= 1) {
			float bad = 100.0f * (ba - sba) / sba;
			float ted = 100.0f * (te - ste) / ste;
			float speedd = (fabs(sspeed) > 1e-6f) ?
					(100.0f * (speed - sspeed) / sspeed) : 0.0f;

			snprintf(buf, sizeof(buf), "ST delta: %2.3f %%", bad);
			Display->writeText(5, buf);
			ESP_LOGI(FNAME, "%s", buf);

			snprintf(buf, sizeof(buf), "TE delta: %2.3f %%", ted);
			Display->writeText(6, buf);
			ESP_LOGI(FNAME, "%s", buf);

			snprintf(buf, sizeof(buf), "PI delta: %2.2f %%", speedd);
			Display->writeText(7, buf);
			ESP_LOGI(FNAME, "%s", buf);

			// Stopping condition
			if ((fabs(bad) > ST_THRESHOLD) ||
					(fabs(ted) > TE_THRESHOLD) ||
					((sspeed > MIN_SPEED) && (fabs(speedd) > SPEED_THRESHOLD))) {
				failed = true;
				xSemaphoreGive(display_mutex);
				break;
			}
		}

		snprintf(buf, sizeof(buf), "Seconds: %d", (i * 5) + 5);
		Display->writeText(8, buf);
		xSemaphoreGive(display_mutex);
	}

	// Final result
	xSemaphoreTake(display_mutex, portMAX_DELAY);
	if (failed) {
		Display->writeText(9, "Test FAILED");
		ESP_LOGI(FNAME, "FAILED");
	} else {
		Display->writeText(9, "Test PASSED");
		ESP_LOGI(FNAME, "PASSED");
	}
	xSemaphoreGive(display_mutex);

	// Wait for rotary press
	int switchState = Rotary->readBootupStatus();
	while (!switchState) {
		delay(100);
		switchState = Rotary->readBootupStatus();
		ESP_LOGI(FNAME, "Read Rotary: %d", switchState);
	}

	ESP_LOGI(FNAME, "Read Rotary: %d", switchState);

	// Clear display
	xSemaphoreTake(display_mutex, portMAX_DELAY);
	Display->clear();
	xSemaphoreGive(display_mutex);
}

