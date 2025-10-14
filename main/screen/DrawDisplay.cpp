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

static float stall_alarm_off_kmh = 0.;
static uint16_t stall_alarm_off_holddown = 0;
static uint16_t gear_warning_holdoff = 0;
static unsigned long int flarm_alarm_holdtime = 0;


void drawDisplay(void *arg)
{
    ESPRotary &knob = *static_cast<ESPRotary *>(arg);

    stall_alarm_off_kmh = stall_speed.get()/3;

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
			ESP_LOGI(FNAME, "Event param %x", eparam);
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
                if (detail == ScreenEvent::MSG_BOX) {
                    if (MBOX->draw()) { // time triggered mbox update
                        // mbox finish, time to refresh the bottom line of the screen
                        Display->setBottomDirty();
                    }
                }
                else if (detail == ScreenEvent::BOOT_SCREEN) {
                    BootUpScreen::draw(); // time triggered boot screen update
                }
                // else if ( detail == ScreenEvent::FLARM_ALARM ) {
                // }
                else if (detail == ScreenEvent::QNH_ADJUST) {
                    Menu->begin(SetupMenu::createQNHMenu());
                }
                else if (detail == ScreenEvent::VOLT_ADJUST) {
                    Menu->begin(SetupMenu::createVoltmeterAdjustMenu());
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

        // TickType_t dLastWakeTime = xTaskGetTickCount();
        if (gflags.inSetup != true)
        {
            float t = OAT.get();
            if (gflags.validTemperature == false) {
                t = DEVICE_DISCONNECTED_C;
            }
            float airspeed = 0;
            if (airspeed_mode.get() == MODE_IAS) {
                airspeed = ias.get();
            }
            else if (airspeed_mode.get() == MODE_TAS) {
                airspeed = tas;
            } else if (airspeed_mode.get() == MODE_CAS) {
                airspeed = cas;
            } else {
                airspeed = ias.get();
            }

            // Stall Warning Screen
			if( stall_warning.get() && screen_gmeter.get() != SCREEN_PRIMARY ){  // In aerobatics stall warning is contra productive, we concentrate on G-Load Display if permanent enabled
				if( gflags.stall_warning_armed ){
					float acceleration=IMU::getGliderAccelZ();
					if( acceleration < 0.3 )
						acceleration = 0.3;  // limit acceleration effect to minimum 30% of 1g
					float acc_stall= stall_speed.get() * sqrt( acceleration + ( ballast.get()/100));  // accelerated and ballast(ed) stall speed
					if( ias.get() < acc_stall && ias.get() > acc_stall*0.7 ){
						if( !gflags.stall_warning_active ){
							AUDIO->alarm(AUDIO_ALARM_STALL);
							MBOX->newMessage(4, "! STALL !");
							gflags.stall_warning_active = true;
						}
					}
					else{
						if( gflags.stall_warning_active ){
							MBOX->recallAlarm();
							gflags.stall_warning_active = false;
						}
					}
					if( ias.get() < stall_alarm_off_kmh ){
						stall_alarm_off_holddown++;
						if( stall_alarm_off_holddown > 1200 ){  // ~30 seconds holddown
							gflags.stall_warning_armed = false;
							stall_alarm_off_holddown=0;
						}
					}
					else{
						stall_alarm_off_holddown=0;
					}
				}
				else{
					if( ias.get() > stall_speed.get() ){
						gflags.stall_warning_armed = true;
						stall_alarm_off_holddown=0;
					}
				}
			}
			if( gear_warning.get() ){
				if( !gear_warning_holdoff ){
					int gw = 0;
					if( gear_warning.get() == GW_EXTERNAL ){
						gw = gflags.gear_warn_external;
					}else{
						gw = gpio_get_level( SetupMenu::getGearWarningIO() );
						if( gear_warning.get() == GW_FLAP_SENSOR_INV || gear_warning.get() == GW_S2_RS232_RX_INV ){
							gw = !gw;
						}
					}
					if( gw ){
						if( Rotary->readBootupStatus() ){   // Acknowledge Warning -> Warning OFF
							gear_warning_holdoff = 25000;  // ~500 sec
							// AUDIO->alarm( false );
							Display->clear();
							gflags.gear_warning_active = false;
						}
						else if( !gflags.gear_warning_active && !gflags.stall_warning_active ){
							AUDIO->alarm(AUDIO_ALARM_STALL);
							MBOX->newMessage(4, "! GEAR !");
							gflags.gear_warning_active = true;
						}
					}
					else{
						if( gflags.gear_warning_active ){
							// AUDIO->alarm( false );
							MBOX->recallAlarm();
							gflags.gear_warning_active = false;
						}
					}
				}
				else{
					gear_warning_holdoff--;
				}
			}

			// Flarm Warning Screen
			if( flarm_warning.get() && !gflags.stall_warning_active && Flarm::alarmLevel() >= flarm_warning.get() ){ // 0 -> Disable
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
					// AUDIO->alarm( false );
				}
			}
			if( gflags.flarmWarning )
				Flarm::drawFlarmWarning();
			// G-Load Display
			// ESP_LOGI(FNAME,"Active Screen = %d", active_screen );
			if( ((IMU::getGliderAccelZ() > gload_pos_thresh.get() || IMU::getGliderAccelZ() < gload_neg_thresh.get()) && screen_gmeter.get() == SCREEN_DYNAMIC ) ||
					( screen_gmeter.get() == SCREEN_PRIMARY ) || (Menu->getActiveScreen() == SCREEN_GMETER)  )
			{
				if( !gflags.gLoadDisplay ){
					gflags.gLoadDisplay = true;
				}
			}
			else{
				if( gflags.gLoadDisplay ) {
					gflags.gLoadDisplay = false;
				}
			}
			if( gflags.gLoadDisplay ) {
				Display->drawLoadDisplay( IMU::getGliderAccelZ() );
			}
			if( Menu->getActiveScreen() == SCREEN_HORIZON ) {
				float roll =  IMU::getRollRad();
				float pitch = IMU::getPitchRad();
				Display->drawHorizon( pitch, roll, 0 );
				gflags.horizon = true;
			}
			else{
				gflags.horizon = false;
			}
			// G-Load Alarm when limits reached
			if( screen_gmeter.get() != SCREEN_OFF  ){
				if( IMU::getGliderAccelZ() > gload_pos_limit.get() || IMU::getGliderAccelZ() < gload_neg_limit.get()  ){
					if( !gflags.gload_alarm ) {
						AUDIO->alarm(AUDIO_ALARM_STALL);
						gflags.gload_alarm = true;
					}
				}else
				{
					if( gflags.gload_alarm ) {
						// AUDIO->alarm( false );
						gflags.gload_alarm = false;
					}
				}
			}
			// Vario Screen
			if( !(gflags.stall_warning_active || gflags.gear_warning_active || gflags.flarmWarning || gflags.gLoadDisplay || gflags.horizon )  ) {
				// ESP_LOGI(FNAME,"TE=%2.3f", te_vario.get() );
				Display->drawDisplay( Units::AirspeedRounded(airspeed), te_vario.get(), aTE, polar_sink, altitude.get(), t, batteryVoltage, s2f_delta, as2f, average_climb.get(), VCMode.getCMode(), gflags.standard_setting, flap_pos.get() );
			}
			if( theCenteraid ){
				theCenteraid->tick();
			}
		}
		// esp_task_wdt_reset();
		if( uxTaskGetStackHighWaterMark(NULL) < 512  ) {
			ESP_LOGW(FNAME,"Warning drawDisplay stack low: %d bytes", uxTaskGetStackHighWaterMark(NULL) );
		}
    }
}
