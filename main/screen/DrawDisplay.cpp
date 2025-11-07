/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "DrawDisplay.h"

#include "CenterAid.h"
#include "IpsDisplay.h"
#include "UiEvents.h"
#include "SetupRoot.h"
#include "MessageBox.h"
#include "BootUpScreen.h"

#include "setup/SetupMenuValFloat.h"
#include "setup/SetupMenuDisplay.h"
#include "setup/SetupMenu.h"
#include "setup/SetupNG.h"
#include "setup/CruiseMode.h"
#include "ESPRotary.h"
#include "OneWireESP32.h"
#include "KalmanMPU6050.h"
#include "ESPAudio.h"
#include "Flarm.h"
#include "S2fSwitch.h"
#include "sensor.h"
#include "protocol/WatchDog.h"
#include "logdef.h"


// The context to serialize all display access.
QueueHandle_t uiEventQueue = nullptr;

static unsigned long int flarm_alarm_holdtime = 0;


void UiEventLoop(void *arg)
{
    ESPRotary &knob = *static_cast<ESPRotary *>(arg);
	float temp = DEVICE_DISCONNECTED_C;
	float airspeed = 0;
    int16_t stall_warning_active = 0;
    int16_t gload_warning_active = 0;
	bool gear_warning_active = false;

    xQueueReset(uiEventQueue);

    // esp_task_wdt_add(NULL); // incompatible with current setup implementation, e.g. all calib procedures would trigger the wd

    while (1)
    {
        // handle button events in this context
        int eparam;
        if (xQueueReceive(uiEventQueue, &eparam, pdMS_TO_TICKS(20)) == pdTRUE)
        {
            UiEvent event(eparam);
            uint8_t detail = event.getUDetail();
            // ESP_LOGI(FNAME, "Event param %x", eparam);
            if (event.isButtonEvent())
            {
                // ESP_LOGI(FNAME, "Button event %x", detail);
                if (detail == ButtonEvent::SHORT_PRESS) {
                    knob.sendPress();
                }
                else if (detail == ButtonEvent::LONG_PRESS) {
                    knob.sendLongPress();
                }
                else if (detail == ButtonEvent::BUTTON_RELEASED) {
                    knob.sendRelease();
                }
                else if (detail == ButtonEvent::ESCAPE) {
                    knob.sendEscape();
                }
            }
            else if (event.isRotaryEvent()) {
                // ESP_LOGI(FNAME, "Rotation step %d", event.getSDetail());
                knob.sendRot(event.getSDetail());
            }
            else if (event.isScreenEvent()) {
                // ESP_LOGI(FNAME, "Screen event %d", detail);
                if (detail == ScreenEvent::MAIN_SCREEN) {
                    if (!gflags.inSetup) {
                        switch (MenuRoot->getActiveScreen()) {
                            case SCREEN_VARIO:
                                Display->drawDisplay(Units::AirspeedRounded(airspeed), te_vario.get(), aTE, 
                                            polar_sink, altitude.get(), temp, batteryVoltage, s2f_delta, 
                                            as2f, average_climb.get(), flap_pos.get());
                                break;
                            case SCREEN_GMETER:
                                Display->drawLoadDisplay( IMU::getGliderAccelZ() );
                                break;
                            case SCREEN_HORIZON:
                                Display->drawHorizon( IMU::getPitchRad(), IMU::getRollRad(), 0 );
                                break;
                        }
                    }
                } else if (detail == ScreenEvent::MSG_BOX) {
                    if (MBOX->draw()) { // time triggered mbox update
                        // mbox finish, time to refresh the bottom line of the screen
                        Display->setBottomDirty();
                    }
                } else if (detail == ScreenEvent::BOOT_SCREEN) {
                    BootUpScreen::draw(); // time triggered boot screen update
                }
                // else if ( detail == ScreenEvent::FLARM_ALARM ) {
                // }
                else if (detail == ScreenEvent::QNH_ADJUST) {
                    MenuRoot->begin(SetupMenu::createQNHMenu());
                }
                else if (detail == ScreenEvent::VOLT_ADJUST) {
                    MenuRoot->begin(SetupMenu::createVoltmeterAdjustMenu());
                }
			}
            else if (event.isModeEvent()) {
                if (detail == ModeEvent::MODE_TOGGLE) {
                    VCMode.setCMode(!VCMode.getCMode());
                }
                else if (detail == ModeEvent::MODE_VARIO || detail == ModeEvent::MODE_S2F) {
                    VCMode.setCMode(detail == ModeEvent::MODE_S2F);
                }
            }
            else {
                // ESP_LOGI(FNAME, "Unknown event %x", event);
            }
            if (uiMonitor) {
                uiMonitor->pet(); // knob ui interaction happened
            }
        }

        if ( ! gflags.inBootUp )
        {
            temp = OAT.get();
            if (gflags.validTemperature == false) {
                temp = DEVICE_DISCONNECTED_C;
            }
            if (airspeed_mode.get() == MODE_IAS) {
                airspeed = ias.get();
            } else if (airspeed_mode.get() == MODE_TAS) {
                airspeed = tas;
            } else if (airspeed_mode.get() == MODE_CAS) {
                airspeed = cas;
            } else {
                airspeed = ias.get();
            }

            // Stall Warning
            if (stall_warning.get() && screen_gmeter.get() != SCREEN_PRIMARY) {
                // In aerobatics stall warning is contra productive, we concentrate on G-Load Display if permanent enabled
                float acceleration = IMU::getGliderAccelZ();
                if (acceleration < 0.3) {
                    acceleration = 0.3; // limit acceleration effect to minimum 30% of 1g
                }
                // accelerated and ballast(ed) stall speed
                float acc_stall = polar_stall_speed.get() * sqrt(acceleration + (ballast.get() / 100));
                if (ias.get() < acc_stall && ias.get() > acc_stall * 0.7) {
                    if (!stall_warning_active) {
                        MBOX->pushMessage(4, "! STALL !", 20); // 20 sec
                    }
                    if (stall_warning_active % 10 == 0) {
                        AUDIO->startSound(AUDIO_ALARM_STALL);
                    }
                    stall_warning_active++;
                }
                else if ( stall_warning_active ) {
                    stall_warning_active = 0;
                    MBOX->popMessage();
                }
            }
            // Gear Warning
            if (gear_warning.get()) {
                int gw = 0;
                if (gear_warning.get() == GW_EXTERNAL) {
                    gw = gflags.gear_warn_external;
                } else {
                    gw = gpio_get_level(SetupMenu::getGearWarningIO());
                    if (gear_warning.get() == GW_FLAP_SENSOR_INV || gear_warning.get() == GW_S2_RS232_RX_INV) {
                        gw = !gw;
                    }
                }
                if (gw) {
                    if (!gear_warning_active && !stall_warning_active) {
                        AUDIO->startSound(AUDIO_ALARM_GEAR);
                        MBOX->pushMessage(4, "! GEAR !", 20);
                        gear_warning_active = true;
                    }
                } else {
                    gear_warning_active = false;
                }
            }

            // Flarm Warning Screen
			if( flarm_warning.get() && !stall_warning_active && Flarm::alarmLevel() >= flarm_warning.get() ){ // 0 -> Disable
				// ESP_LOGI(FNAME,"Flarm::alarmLevel: %d, flarm_warning.get() %d", Flarm::alarmLevel(), flarm_warning.get() );
				if( !gflags.flarmWarning ) {
					gflags.flarmWarning = true;
					delay(100);
					Display->clear();
				}
				flarm_alarm_holdtime = millis()+flarm_alarm_time.get()*1000;
			}
			else{
				if( gflags.flarmWarning && (millis() > flarm_alarm_holdtime) ){
					gflags.flarmWarning = false;
					Display->clear();
				}
			}
			if( gflags.flarmWarning ) {
				Flarm::drawFlarmWarning();
			}
			// G-Load Display fixme
			// ESP_LOGI(FNAME,"Active Screen = %d", active_screen );
			// if( ((IMU::getGliderAccelZ() > gload_pos_thresh.get() || IMU::getGliderAccelZ() < gload_neg_thresh.get()) && screen_gmeter.get() == SCREEN_DYNAMIC ) ||
			// 		( screen_gmeter.get() == SCREEN_PRIMARY ) || (MenuRoot->getActiveScreen() == SCREEN_GMETER)  )
			// {
			// 	if( !gflags.gLoadDisplay ){
			// 		gflags.gLoadDisplay = true;
			// 	}
			// }
			// else{
			// 	if( gflags.gLoadDisplay ) {
			// 		gflags.gLoadDisplay = false;
			// 	}
			// }

            // G-Load alarm when limits reached
            if (screen_gmeter.get() != SCREEN_OFF) {
                if (IMU::getGliderAccelZ() > gload_pos_limit.get() || IMU::getGliderAccelZ() < gload_neg_limit.get()) {
                    if (gload_warning_active % 10 == 0) {
                        AUDIO->startSound(AUDIO_ALARM_GLOAD);
                        gload_warning_active++;
                    }
                }
                else if (gload_warning_active) {
                    gload_warning_active = 0;
                }
            }

            if (theCenteraid) {
				theCenteraid->tick();
			}
		}
		// esp_task_wdt_reset();
		if( uxTaskGetStackHighWaterMark(NULL) < 512  ) {
			ESP_LOGW(FNAME,"Warning UiEventLoop stack low: %d bytes", uxTaskGetStackHighWaterMark(NULL) );
		}
    }
}
