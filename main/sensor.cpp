
#include "sensor.h"

#include "Cipher.h"
#include "BME280_ESP32_SPI.h"
#include "mcp3221.h"
#include "mcp4018.h"
#include "ESP32NVS.h"
#include "MP5004DP.h"
#include "MS4525DO.h"
#include "ABPMRR.h"
#include "MCPH21.h"
#include "BMPVario.h"
#include "comm/BTspp.h"
#include "BLESender.h"
#include "setup/SetupNG.h"
#include "ESPAudio.h"
#include "setup/SetupMenu.h"
#include "ESPRotary.h"
#include "AnalogInput.h"
#include "Atmosphere.h"
#include "IpsDisplay.h"
#include "S2F.h"
#include "Version.h"
#include "glider/Polars.h"
#include "Flarm.h"
#include "setup/SetupMenuValFloat.h"
#include "setup/SetupMenuDisplay.h"
#include "protocol/Clock.h"
#include "protocol/MagSensBin.h"
#include "protocol/NMEA.h"
#include "protocol/WatchDog.h"
#include "screen/SetupRoot.h"
#include "screen/BootUpScreen.h"
#include "screen/MessageBox.h"
#include "screen/DrawDisplay.h"

#include "math/Quaternion.h"
#include "wmm/geomag.h"
#include "OTA.h"
#include "Switch.h"
#include "AverageVario.h"

#include "MPU.hpp"        // main file, provides the class itself
#include "mpu/math.hpp"   // math helper for dealing with MPU data
#include "mpu/types.hpp"  // MPU data types and definitions
#include "I2Cbus.hpp"
#include "KalmanMPU6050.h"
#include "comm/WifiApSta.h"
#include "LeakTest.h"
#include "Units.h"
#include "Flap.h"
#include "SPL06-007.h"
#include "wind/WindCalcTask.h"
#include "comm/SerialLine.h"
#include "comm/CanBus.h"
#include "comm/DeviceMgr.h"
#include "protocol/TestQuery.h"
#include "AdaptUGC.h"
#include "CenterAid.h"
#include "OneWireESP32.h"
#include "logdef.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <coredump_to_server.h>
#include <driver/spi_master.h>
#include <esp_task_wdt.h>
#include <esp_task.h>
#include <soc/sens_reg.h> // needed for adc pin reset
#include <esp_sleep.h>
#include <esp_system.h>
#include <esp_flash.h>
#include <esp_chip_info.h>
#include <driver/gpio.h>

#include <string>
#include <cstdio>
#include <cstring>


/*
BMP:
    SCK - This is the SPI Clock pin, its an input to the chip
    SDO - this is the Serial Data Out / Master In Slave Out pin (MISO), for data sent from the BMP183 to your processor
    SDI - this is the Serial Data In / Master Out Slave In pin (MOSI), for data sent from your processor to the BME280
    CS - this is the Chip Select pin, drop it low to start an SPI transaction. Its an input to the chip
 */

#define CS_bme280BA GPIO_NUM_26   // before CS pin 33
#define CS_bme280TE GPIO_NUM_33   // before CS pin 26

#define SPL06_007_BARO 0x77
#define SPL06_007_TE   0x76

MCP3221 *MCP=0;
OneWire32  ds18b20( GPIO_NUM_23 );  // GPIO_NUM_23 standard, alternative  GPIO_NUM_17
uint8_t t_devices = 0;
uint64_t t_addr[1];

AirspeedSensor *asSensor=0;

SemaphoreHandle_t xMutex=NULL;
SemaphoreHandle_t spiMutex=NULL;

S2F Speed2Fly;

AnalogInput Battery( (22.0+1.2)/1200, ADC_ATTEN_DB_0, ADC_CHANNEL_7, ADC_UNIT_1 );

TaskHandle_t apid = NULL;
TaskHandle_t bpid = NULL;
TaskHandle_t tpid = NULL;
TaskHandle_t dpid = NULL;

PressureSensor *baroSensor = nullptr;
PressureSensor *teSensor = nullptr;

AdaptUGC *MYUCG = 0;  // ( SPI_DC, CS_Display, RESET_Display );
IpsDisplay *Display = 0;
CenterAid  *centeraid = 0;
SetupRoot  *Menu = nullptr;
WatchDog_C *uiMonitor = nullptr;

// Gyro and acceleration sensor
I2C_t& i2c = i2c1;
MPU_t MPU;         // create an object

// Magnetic sensor / compass
BLESender blesender;
SerialLine *S1 = NULL;
SerialLine *S2 = NULL;
Clock *MY_CLOCK = nullptr;

//boot log
std::string logged_tests;

// global variables
float baroP=0; // barometric pressure
static float teP=0;   // TE pressure
static float temperature=15.0;
static float xcvTemp=15.0;
static unsigned long _millis = 0;
unsigned long _gps_millis = 0;


float batteryVoltage = 0.;
float dynamicP; // Pitot

float slipAngle = 0.0;

// global color variables for adaptable display variant
uint8_t g_col_background=255; // black
uint8_t g_col_highlight=0;
uint8_t g_col_header_r=101+g_col_background/5;
uint8_t g_col_header_g=108+g_col_background/5;
uint8_t g_col_header_b=g_col_highlight;
uint8_t g_col_header_light_r=161-g_col_background/4;
uint8_t g_col_header_light_g=168-g_col_background/3;
uint8_t g_col_header_light_b=g_col_highlight;
uint8_t gyro_flash_savings=0;

// boot with flasg "inSetup":=true and release the screen for other purpouse by setting it false.
t_global_flags gflags = { true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };

int  ccp=60;
float tas = 0;
float cas = 0;
float aTE = 0;
float alt_external;
float altSTD;
float netto = 0;
float as2f = 0;
float s2f_delta = 0;
float polar_sink = 0;

int count=0;

float mpu_target_temp=45.0;

AdaptUGC *egl = 0;

const constexpr char passed_text[] = "PASSED\n";
const constexpr char failed_text[] = "FAILED\n";

const float glider_min_ias = 50.f; // todo replace with speed derived from glider type

int IRAM_ATTR sign(int num) {
    return (num > 0) - (num < 0);
}

AnalogInput* getBattery() { return &Battery;}

float getTAS() { return tas; }


static void grabMPU()
{
	// Automatically trac the gyro bias
	static int32_t cur_gyro_bias[3];
	const int MAXDRIFT         = 2;    // °/s maximum drift that is automatically compensated on ground
	const int NUM_GYRO_SAMPLES = 3000; // 10 per second -> 5 minutes, so T has been settled after power on
	static uint16_t num_gyro_samples = 0;

	// Read the IMU registers and check the output
	if( IMU::MPU6050Read() == ESP_OK )
	{
		// Do the gyro auto bias
		vector_ijk gyroDPS = IMU::getGliderGyro();
		// ESP_LOGI(FNAME,"Gyro:\t%4f\t%4f\t%4f", gyroDPS.a, gyroDPS.b, gyroDPS.c);
		// vector_ijk accl = IMU::getGliderAccel();
		// if (compass != nullptr) {
		// 	ESP_LOGI(FNAME,"Accl:\t%4f\t%4f\t%4f\tL%.2f Gyro:\t%4f\t%4f\t%4f Mag:\t%4f\t%4f\t%4f", accl.a, accl.b, accl.c, accl.get_norm(),
		// 		gyroDPS.a, gyroDPS.b, gyroDPS.c,
		// 		compass->rawX(), compass->rawY(), compass->rawZ());
		// }

		float GS=0; // Autoleveling Gyro feature only with GPS and GS close to zero to avoid triggering at push back taxi with zero AS
		bool gpsOK = Flarm::getGPSknots( GS );
		// ESP_LOGI(FNAME,"GS=%.3f %d", GS, gpsOK );
		if( gpsOK && GS < 2 && ias.get() < 5 ){  // GPS status, groundspeed and airspeed regarded for still stand
			// check low rotation on all 3 axes = on ground
			if( abs( gyroDPS.a ) < MAXDRIFT && abs( gyroDPS.b ) < MAXDRIFT && abs( gyroDPS.c ) < MAXDRIFT ) {
				num_gyro_samples++;
				cur_gyro_bias[0] = IMU::getRawGyroX();
				cur_gyro_bias[1] = IMU::getRawGyroY();
				cur_gyro_bias[2] = IMU::getRawGyroZ();
				if( num_gyro_samples > NUM_GYRO_SAMPLES ) { // every 5 minute (3000 samples) recalculate offset
					mpud::raw_axes_t gb;
					mpud::raw_axes_t gbo = MPU.getGyroOffset();
					for(int i=0; i<3; i++){
						gb[i]  = gbo[i] -(( (cur_gyro_bias)[i]/(NUM_GYRO_SAMPLES*4)) ); // translate to 1000 DPS
						cur_gyro_bias[i] = 0;
					}
					ESP_LOGI(FNAME,"New gyro offset X/Y/Z: OLD:%d/%d/%d NEW:%d/%d/%d", gbo.x, gbo.y, gbo.z, gb.x, gb.y, gb.z );
					if( (abs( gbo.x-gb.x ) > 0) || (abs( gbo.y-gb.y ) > 0) || (abs( gbo.z-gb.z ) > 0)  ){  // any delta is directly set in RAM
						ESP_LOGI(FNAME,"Set new gyro offset X/Y/Z: OLD:%d/%d/%d NEW:%d/%d/%d", gbo.x, gbo.y, gbo.z, gb.x, gb.y, gb.z );
						MPU.setGyroOffset( gb );
					}
					// if we have temperature control, we check if control is locked, otherwise we have no idea but anyway takeover better offset
					if( (HAS_MPU_TEMP_CONTROL && (MPU.getSiliconTempStatus() == MPU_T_LOCKED)) || !HAS_MPU_TEMP_CONTROL ){
						if( (abs( gbo.x-gb.x ) > 1 || abs( gbo.y-gb.y ) > 1 || abs( gbo.z-gb.z ) > 1) && gyro_flash_savings<5 ){ // Set only changes > 1 in Flash and only 5 times per boot
							gyro_bias.set( gb );
							ESP_LOGI(FNAME,"Store the new offset also in Flash, store number: %d", gyro_flash_savings );
							gyro_flash_savings++;
						}
					}
					num_gyro_samples = 0;
				}
			}
		}
		IMU::Process();
	}

}

static void toyFeed()
{
	if ( ToyNmeaPrtcl ) {
		xSemaphoreTake(xMutex,portMAX_DELAY );
		if( ahrs_rpyl_dataset.get() ){
			ToyNmeaPrtcl->sendXcvRPYL(IMU::getRoll(), IMU::getPitch(), IMU::getYaw(), IMU::getGliderAccelZ());
			ToyNmeaPrtcl->sendXcvAPENV1( ias.get(), altitude.get(), te_vario.get() );
		}
		switch( nmea_protocol.get() ) {
		case BORGELT_P:
			ToyNmeaPrtcl->sendBorgelt(te_vario.get(), OAT.get(), ias.get(), tas, MC.get(), bugs.get(), ballast.get(), Switch::getCruiseState(), gflags.validTemperature );
			ToyNmeaPrtcl->sendXcvGeneric(te_vario.get(), altSTD, tas);
			break;
		case OPENVARIO_P:
			ToyNmeaPrtcl->sendOpenVario(baroP, dynamicP, te_vario.get(), OAT.get(), gflags.validTemperature );
			break;
		case CAMBRIDGE_P:
			ToyNmeaPrtcl->sendCambridge(te_vario.get(), tas, MC.get(), bugs.get(), altitude.get());
			break;
		case XCVARIO_P:
			ToyNmeaPrtcl->sendStdXCVario(baroP, dynamicP, te_vario.get(), OAT.get(), ias.get(), tas, MC.get(), bugs.get(), ballast.get(), Switch::getCruiseState(), altitude.get(), gflags.validTemperature,
				IMU::getGliderAccelX(), IMU::getGliderAccelY(), IMU::getGliderAccelZ(), IMU::getGliderGyroX(), IMU::getGliderGyroY(), IMU::getGliderGyroZ() );
			break;
		default:
			ESP_LOGE(FNAME,"Protocol %d not supported error", nmea_protocol.get() );
		}
		xSemaphoreGive(xMutex);
	}
}


void clientLoop(void *pvParameters)
{
	int ccount = 0;
	gflags.validTemperature = true;
	esp_task_wdt_add(NULL);

	while (true)
	{
		TickType_t xLastWakeTime = xTaskGetTickCount();
		ccount++;
		aTE += (te_vario.get() - aTE)* (1/(10*vario_av_delay.get()));
		if( gflags.haveMPU ) {
			grabMPU();
		}
		if( !(ccount%5) )
		{
			double tmpalt = altitude.get(); // get pressure from altitude
			if( (fl_auto_transition.get() == 1) && ((int)( Units::meters2FL( altitude.get() )) + (int)(gflags.standard_setting) > transition_alt.get() ) ) {
				ESP_LOGI(FNAME,"Above transition altitude");
				baroP = baroSensor->calcPressure(1013.25, tmpalt); // above transition altitude
			}
			else {
				baroP = baroSensor->calcPressure( QNH.get(), tmpalt);
			}
			dynamicP = Atmosphere::kmh2pascal(ias.get());
			tas = Atmosphere::TAS2( ias.get(), altitude.get(), OAT.get() );
			if( airspeed_mode.get() == MODE_CAS )
				cas = Atmosphere::CAS( dynamicP );
			if( gflags.haveMPU && HAS_MPU_TEMP_CONTROL ){
				MPU.temp_control( ccount, xcvTemp );
			}
			if( IMU::getGliderAccelZ() > gload_pos_max.get() ){
				gload_pos_max.set( IMU::getGliderAccelZ() );
			}else if( IMU::getGliderAccelZ() < gload_neg_max.get() ){
				gload_neg_max.set( IMU::getGliderAccelZ() );
			}
			toyFeed();
			if( true && !(ccount%5) ) { // todo need a mag_hdm.valid() flag
				NmeaPrtcl *prtcl = static_cast<NmeaPrtcl*>(DEVMAN->getProtocol(NAVI_DEV, XCVARIO_P)); // Todo preliminary solution ..
				if ( prtcl ) {
					if( compass_nmea_hdm.get() ) {
						prtcl->sendXCVNmeaHDM( mag_hdm.get() );
					}

					if( compass_nmea_hdt.get() ) {
						prtcl->sendXCVNmeaHDT( mag_hdt.get() );
					}
				}
			}
			esp_task_wdt_reset();
			if( uxTaskGetStackHighWaterMark( bpid ) < 512 ) {
				ESP_LOGW(FNAME,"Warning client task stack low: %d bytes", uxTaskGetStackHighWaterMark( bpid ) );
			}
		}
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
	}
}

static int client_sync_dataIdx = 10000;
void startClientSync()
{
	// Start the client sync in a moment
	client_sync_dataIdx = 0;
}

void readSensors(void *pvParameters){

	float tasraw = 0;
	esp_task_wdt_add(NULL);

	while (1)
	{
		count++;   // 10x per second
		TickType_t xLastWakeTime = xTaskGetTickCount();
		float T=OAT.get();
		if( !gflags.validTemperature ) {
			T= 15 - ( (altitude.get()/100) * 0.65 );
			// ESP_LOGW(FNAME,"T invalid, using 15 deg");
		}
		// ESP_LOGI(FNAME,"Start");
		if( gflags.haveMPU  )  // 3th Generation HW, MPU6050 avail and feature enabled
		{
			grabMPU();  // IMU
		}
		// ESP_LOGI(FNAME,"IMU");
		if( asSensor ){  // AS differential Sensor
			bool ok=false;
			float p = asSensor->readPascal(60, ok);
			if( ok )
				dynamicP = p;
		}
		_millis=millis();
		struct timeval tv;
		gettimeofday(&tv, NULL);
		// ESP_LOGI(FNAME,"AS");
		xSemaphoreTake(xMutex,portMAX_DELAY );   // Static Pressure
		bool bok=false;
		float bp = baroSensor->readPressure(bok);
		// ESP_LOGI(FNAME,"Baro");
		bool tok=false;
		float tp = teSensor->readPressure(tok);  // TE Pressure
		xSemaphoreGive(xMutex);
		// ESP_LOGI(FNAME,"TE, Delta: %d - log%d", (int)(millis() - _millis));
		if( logging.get() ){
			char log[ProtocolItf::MAX_LEN];
			sprintf( log, "$SENS;");
			int pos = strlen(log);
			long delta = _millis - _gps_millis;
			if( delta < 0 )
				delta += 1000;
			sprintf( log+pos, "%d.%03d,%ld,%.3f,%.3f,%.3f,%.2f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f", (int)(tv.tv_sec%(60*60*24)), (int)(tv.tv_usec / 1000), delta, bp, tp, dynamicP, T, IMU::getGliderAccelX(), IMU::getGliderAccelY(), IMU::getGliderAccelZ(),
					IMU::getGliderNogateGyroX(), IMU::getGliderNogateGyroY(), IMU::getGliderNogateGyroZ() );
			if( theCompass ){
				pos=strlen(log);
				sprintf( log+pos,",%.4f,%.4f,%.4f", theCompass->rawX(), theCompass->rawY(), theCompass->rawZ());
			}
			pos=strlen(log);
			sprintf( log+pos, "\n");
			const NmeaPrtcl *prtcl = DEVMAN->getNMEA(NAVI_DEV); // Todo preliminary solution ..
			if ( prtcl ) {
				prtcl->sendXCV(log);
			}
		}
		if( tok )
			teP = tp;
		if( bok )
			baroP = bp;
		float te = bmpVario.readTE( tasraw, teP );   // TE value caclulation
		if( (int( te_vario.get()*20 +0.5 ) != int( te*20 +0.5)) || !(count%10) ){  // a bit more fine granular updates than 0.1 m/s as of sound
					te_vario.set( te );  // max 10x per second
		}
		if( !(count%10) ){  // every second read temperature of baro sensor
					bool ok=false;
					float xt = baroSensor->readTemperature(ok);
					if( ok ){
						xcvTemp = xt;
					}
		}
		float iasraw = Atmosphere::pascal2kmh( dynamicP );
		if( baroP != 0 )
			tasraw =  Atmosphere::TAS( iasraw , baroP, T);  // True airspeed in km/h

		// ESP_LOGI("FNAME","P: %f  IAS:%f", dynamicP, iasraw );

		if( airspeed_mode.get() == MODE_CAS ){
			float casraw=Atmosphere::CAS( dynamicP );
			cas += (casraw-cas)*0.25;       // low pass filter
			// ESP_LOGI(FNAME,"IAS=%f, TAS=%f CAS=%f baro=%f", iasraw, tasraw, cas, baroP );
		}
		static float new_ias = 0;
		new_ias = ias.get() + (iasraw - ias.get())*0.25;
		if( (int( ias.get()+0.5 ) != int( new_ias+0.5 ) ) || !(count%20) ){
			ias.set( new_ias );  // low pass filter
		}
		if( airspeed_max.get() < ias.get() ){
			airspeed_max.set( ias.get() );
		}
		// ESP_LOGI("FNAME","P: %f  IAS:%f IASF: %d", dynamicP, iasraw, ias );
		if( !theCompass || !(theCompass->externalData()) ){
			tas += (tasraw-tas)*0.25;       // low pass filter
		}
		// ESP_LOGI(FNAME,"IAS=%f, T=%f, TAS=%f baroP=%f", ias, T, tas, baroP );

		// Slip angle estimation
		float as = tas/3.6;                  // tas in m/s
		const float K = 4000 * 180/M_PI;      // airplane constant and Ay correction factor
		if( tas > 25.0 ){
			slipAngle += ((IMU::getGliderAccelY()*K / (as*as)) - slipAngle)*0.09;   // with atan(x) = x for small x
			// ESP_LOGI(FNAME,"AS: %f m/s, CURSL: %f°, SLIP: %f", as, IMU::getGliderAccelY()*K / (as*as), slipAngle );
		}

		// ESP_LOGI(FNAME,"count %d ccp %d", count, ccp );
		if( !(count % ccp) ) {
			AverageVario::recalcAvgClimb();
		}
		if (FLAP) { FLAP->progress(); }

		// ESP_LOGI(FNAME,"Baro Pressure: %4.3f", baroP );
		float altSTD = 0;
		if( Flarm::validExtAlt() && alt_select.get() == AS_EXTERNAL )
			altSTD = alt_external;
		else
			altSTD = baroSensor->calcAltitudeSTD( baroP );
		float new_alt = 0;
		if( alt_select.get() == AS_TE_SENSOR ) // TE
			new_alt = bmpVario.readAVGalt();
		else if( alt_select.get() == AS_BARO_SENSOR  || alt_select.get() == AS_EXTERNAL ){ // Baro or external
			if(  alt_unit.get() == ALT_UNIT_FL ) { // FL, always standard
				new_alt = altSTD;
				gflags.standard_setting = true;
				// ESP_LOGI(FNAME,"au: %d", alt_unit.get() );
			}else if( (fl_auto_transition.get() == 1) && ((int)Units::meters2FL( altSTD ) + (int)(gflags.standard_setting) > transition_alt.get() ) ) { // above transition altitude
				new_alt = altSTD;
				gflags.standard_setting = true;
				ESP_LOGI(FNAME,"auto:%d alts:%f ss:%d ta:%f", fl_auto_transition.get(), altSTD, gflags.standard_setting, transition_alt.get() );
			}
			else {
				if( Flarm::validExtAlt() && alt_select.get() == AS_EXTERNAL )
					new_alt = altSTD + ( QNH.get()- 1013.25)*8.2296;  // correct altitude according to ISA model = 27ft / hPa
				else
					new_alt = baroSensor->calcAltitude( QNH.get(), baroP );
				gflags.standard_setting = false;
				// ESP_LOGI(FNAME,"QNH %f baro: %f alt: %f SS:%d", QNH.get(), baroP, alt, gflags.standard_setting  );
			}
		}
		if( (int( new_alt +0.5 ) != int( altitude.get() +0.5 )) || !(count%20) ){
			// ESP_LOGI(FNAME,"New Altitude: %.1f", new_alt );
			altitude.set( new_alt );
		}

		aTE = bmpVario.readAVGTE();

		if( (count % 2) == 0 ){
			airborne.set(ias.get() > glider_min_ias);
			toyFeed();
		}

		if( theCompass ){
			// ESP_LOGI(FNAME,"Compass, have sensor=%d  hdm=%d", compass->haveSensor(),  compass_nmea_hdt.get());
			if( ! theCompass->calibrationIsRunning() ) {
				// Trigger heading reading and low pass filtering. That job must be
				// done periodically.
				bool ok;
				float heading = theCompass->getGyroHeading( &ok );
				NmeaPrtcl *prtcl = static_cast<NmeaPrtcl*>(DEVMAN->getProtocol(NAVI_DEV, XCVARIO_P)); // Todo preliminary solution ..
				if(ok){
					if( (int)heading != (int)mag_hdm.get() && !(count%10) ){
						mag_hdm.set( heading );
					}
					if( !(count%5) && compass_nmea_hdm.get() == true ) {
						if ( prtcl ) {
							prtcl->sendXCVNmeaHDM(heading);
						}
					}
				}
				else{
					if( mag_hdm.get() != -1 )
						mag_hdm.set( -1 );
				}
				float theading = theCompass->filteredTrueHeading( &ok );
				if(ok){
					if( (int)theading != (int)mag_hdt.get() && !(count%10) ){
						mag_hdt.set( theading );
					}
					if( !(count%5) && ( compass_nmea_hdt.get() == true )  ) {
						if ( prtcl ) {
							prtcl->sendXCVNmeaHDM(heading);
						}
					}
				}
				else{
					if( mag_hdt.get() != -1 )
						mag_hdt.set( -1 );
				}
			}
		}
		if( IMU::getGliderAccelZ() > gload_pos_max.get() ){
			gload_pos_max.set( IMU::getGliderAccelZ() );
		}else if( IMU::getGliderAccelZ() < gload_neg_max.get() ){
			gload_neg_max.set( IMU::getGliderAccelZ() );
		}

		// Check on new clients connecting
		if ( client_sync_dataIdx < SetupCommon::numEntries() ) {
			while( client_sync_dataIdx < SetupCommon::numEntries() ) {
				if ( SetupCommon::syncEntry(client_sync_dataIdx++) ) {
					break; // Hit entry to actually sync and send data
				}
			}
			if ( client_sync_dataIdx >= SetupCommon::numEntries() ) {
				ESP_LOGI(FNAME,"Client sync complete");
			}
		}
		if( gflags.haveMPU && HAS_MPU_TEMP_CONTROL ){
			// ESP_LOGI(FNAME,"MPU temp control; T=%.2f", MPU.getTemperature() );
			MPU.temp_control( count, xcvTemp );
		}
		esp_task_wdt_reset();
		if( uxTaskGetStackHighWaterMark( bpid ) < 512 ) {
			ESP_LOGW(FNAME,"Warning sensor task stack low: %d bytes", uxTaskGetStackHighWaterMark( bpid ) );
		}
		vTaskDelayUntil(&xLastWakeTime, 100/portTICK_PERIOD_MS);
	}
}

static int ttick = 0;
static float temp_prev = -3000;


void readTemp(void *pvParameters)
{
	esp_task_wdt_add(NULL);
	while (1) {
		TickType_t xLastWakeTime = xTaskGetTickCount();
		batteryVoltage = Battery.get();
		// ESP_LOGI(FNAME,"Battery=%f V", battery );
		if( !SetupCommon::isClient() ) {  // client Vario will get Temperature info from main Vario
			if( !t_devices ){
				t_devices = ds18b20.search(t_addr, 1);
				// ESP_LOGI(FNAME,"Temperatur Sensors found N=%d Addr: %llx", t_devices, t_addr[0] );
			}
			if( t_devices && !gflags.inSetup ){
				// ESP_LOGI(FNAME,"Temp devices %d", t_devices);
				float temp;
				uint8_t err = ds18b20.getTemp(t_addr[0], temp );
				ds18b20.request();
				if( !err ){
					// ESP_LOGI(FNAME,"Raw Temp %f", temperature);
					if( gflags.validTemperature == false ) {
						ESP_LOGI(FNAME,"Temperatur Sensor connected");
						gflags.validTemperature = true;
					}
					// ESP_LOGI(FNAME,"temperature=%2.1f", temperature );
					temperature +=  (temp - temperature) * 0.3; // A bit low pass as strategy against toggling
					if( abs(temperature - temp_prev) > 0.1 ){
						float tr = std::round(temperature*10)/10;
						OAT.set( tr );
						ESP_LOGI(FNAME,"NEW temperature=%2.1f, prev T=%2.1f", tr, temp_prev );
						temp_prev = temperature;
					}
				}else{
					if( gflags.validTemperature == true ) {
						ESP_LOGI(FNAME,"Temperatur Sensor disconnected");
						gflags.validTemperature = false;
					}
				}
			}
			// ESP_LOGV(FNAME,"T=%f", temperature );
			Flarm::tick();
			if( theCompass )
				theCompass->ageIncr();
		}else{
			if( (OAT.get() > -55.0) && (OAT.get() < 85.0) )
				gflags.validTemperature = true;
		}
		Flarm::progress();
		esp_task_wdt_reset();

		if( (ttick++ % 50) == 0) {
			ESP_LOGI(FNAME,"Free Heap: %d bytes", heap_caps_get_free_size(MALLOC_CAP_8BIT) );
			if( uxTaskGetStackHighWaterMark( tpid ) < 256 ) {
				ESP_LOGW(FNAME,"Warning temperature task stack low: %d bytes", uxTaskGetStackHighWaterMark( tpid ) );
			}
			if( heap_caps_get_free_size(MALLOC_CAP_8BIT) < 20000 ) {
				ESP_LOGW(FNAME,"Warning heap_caps_get_free_size getting low: %d", heap_caps_get_free_size(MALLOC_CAP_8BIT));
			}
		}
		if( (ttick%5) == 0 ){
			SetupCommon::commitDirty();
			// DeviceManager* dm = DeviceManager::Instance();
			// static_cast<TestQuery*>(dm->getProtocol( TEST_DEV2, TEST_P ))->sendTestQuery();  // all 5 seconds on burst
		}
		vTaskDelayUntil(&xLastWakeTime, 1000/portTICK_PERIOD_MS);
	}
}

static esp_err_t _coredump_to_server_begin_cb(void * priv)
{
	std::printf("================= CORE DUMP START =================\r\n");
	return ESP_OK;
}

static esp_err_t _coredump_to_server_end_cb(void * priv)
{
	std::printf("================= CORE DUMP END ===================\r\n");
	return ESP_OK;
}

static esp_err_t _coredump_to_server_write_cb(void * priv, char const * const str)
{
	std::printf("%s\r\n", str);
	return ESP_OK;
}

void register_coredump() {
	coredump_to_server_config_t coredump_cfg = {
			.start = _coredump_to_server_begin_cb,
			.end = _coredump_to_server_end_cb,
			.write = _coredump_to_server_write_cb,
			.priv = NULL,
	};
	if( coredump_to_server(&coredump_cfg) != ESP_OK ){  // Dump to console and do not clear (will done after fetched from Webserver)
		ESP_LOGI( FNAME, "+++ All green, no coredump found in FLASH +++");
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// Sensor board init method. Herein all functions that make the XCVario are launched and tested.
void system_startup(void *args){

	bool selftestPassed=true;

	MCP = new MCP3221();
	MCP->setBus( &i2c );
	gpio_set_drive_capability(GPIO_NUM_23, GPIO_DRIVE_CAP_1);

	esp_wifi_set_mode(WIFI_MODE_NULL);
	spiMutex = xSemaphoreCreateMutex();
	ESP_LOGI( FNAME, "Log level set globally to INFO %d; Max Prio: %d Wifi: %d",  ESP_LOG_INFO, configMAX_PRIORITIES, ESP_TASKD_EVENT_PRIO-5 );
	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);
	ESP_LOGI( FNAME,"This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
			chip_info.cores,
			(chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
					(chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
	ESP_LOGI( FNAME,"Silicon revision %d, ", chip_info.revision);
	uint32_t flash_size = 0;
	ESP_ERROR_CHECK(esp_flash_get_size(NULL, &flash_size));
	ESP_LOGI( FNAME,"%dMB %s flash\n", (int)flash_size / (1024 * 1024),
			(chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
	ESP_LOGI(FNAME, "QNH.get() %.1f hPa", QNH.get() );
	// register_coredump();
	Polars::extract(glider_type_index.get());

	AverageVario::begin();

	Battery.begin();  // for battery voltage
	xMutex=xSemaphoreCreateMutex();
	ccp = (int)(core_climb_period.get()*10);
	spi_bus_config_t buscfg = {
		.mosi_io_num = SPI_MOSI,
		.miso_io_num = SPI_MISO,
		.sclk_io_num = SPI_SCLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.data4_io_num = -1,
		.data5_io_num = -1,
		.data6_io_num = -1,
		.data7_io_num = -1,
		.data_io_default_level = false,
		.max_transfer_sz = 8192,
		.flags = 0,
		.isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO,
		.intr_flags = ESP_INTR_FLAG_IRAM
	};
	ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO));

	egl = new AdaptUGC();
	egl->begin();
	ESP_LOGI( FNAME, "setColor" );
	egl->setColor( 0, 255, 0 );
	ESP_LOGI( FNAME, "drawLine" );
	egl->drawLine( 20,20, 20,80 );
	ESP_LOGI( FNAME, "finish Draw" );

	MYUCG = egl; // new AdaptUGC( SPI_DC, CS_Display, RESET_Display );
	Display = new IpsDisplay( MYUCG );
	Flarm::setDisplay( MYUCG );
	Display->begin();
	Display->bootDisplay();
	Menu = new SetupRoot(Display); // the root setup menu, screens still disabled

	Version V;
	std::string ver( " Ver.: " );
	ver += V.version();
	char hw[24];
	sprintf( hw,", XCV-%d", hardwareRevision.get()+18);  // plus 18, e.g. 2 = XCVario-20
	std::string hwrev( hw );
	ver += hwrev;
	Display->writeText(1, ver.c_str() );
	logged_tests.assign(ver);
	logged_tests += "\n";

	// Start UI task responsible to manage screens and display. Needed to habe the boot screen and message box working
	xTaskCreate(&drawDisplay, "drawDisplay", 6144, Rotary, 4, &dpid); // increase stack by 1K

	BootUpScreen *boot_screen = BootUpScreen::create();
	MessageBox::createMessageBox();
	if ( gflags.schedule_reboot ) {
		MBOX->newMessage(3, "Detecting XCV hardware");
	}
	Rotary->begin();
	if( software_update.get() || Rotary->readBootupStatus() ) {
		software_update.set( 0 ); // only one shot, then boot normal

		if( hardwareRevision.get() >= XCVARIO_22 ) {
			// Give CAN MagSens a chance for an update
			CANbus::createCAN();
			CAN->begin();
			DEVMAN->addDevice(CANREGISTRAR_DEV, REGISTRATION_P, CAN_REG_PORT, CAN_REG_PORT, CAN_BUS);
			DEVMAN->addDevice(MAGSENS_DEV, MAGSENSBIN_P, MagSensBin::LEGACY_MAGSTREAM_ID, 0, CAN_BUS); // fixme
		}
		delete boot_screen; // screen now belongs to OTA
		Menu->begin(new OTA());
		return;
	}
	if( hardwareRevision.get() >= XCVARIO_21 )
	{
		gflags.haveMPU = true;
		mpu_target_temp = mpu_temperature.get();
		ESP_LOGI( FNAME,"MPU initialize");
		MPU.initialize();  // this will initialize the chip and set default configurations
		MPU.setSampleRate(50);  // in (Hz)
		MPU.setAccelFullScale(mpud::ACCEL_FS_8G);
		MPU.setGyroFullScale( GYRO_FS );
		MPU.setDigitalLowPassFilter(mpud::DLPF_5HZ);  // smoother data
		mpud::raw_axes_t gb = gyro_bias.get();
		mpud::raw_axes_t ab = accl_bias.get();
		if( gb.isZero() && ab.isZero() ) {
			ESP_LOGI( FNAME,"MPU computeOffsets");
			MPU.computeOffsets( &ab, &gb );  // returns Offsets in 16G scale
			accl_bias.set( ab );
			gyro_bias.set( gb );
			MPU.setGyroOffset(gb);
			ESP_LOGI( FNAME,"MPU new offsets accl:%d/%d/%d gyro:%d/%d/%d ZERO:%d", ab.x, ab.y, ab.z, gb.x,gb.y,gb.z, gb.isZero() );
		}else{
			MPU.setAccelOffset(ab);
			MPU.setGyroOffset(gb);
		}
		mpud::raw_axes_t accelRaw;
		mpud::float_axes_t accelG;
		float samples = 0;
		delay(200);
		for( auto i=0; i<10; i++ ){
			esp_err_t err = MPU.acceleration(&accelRaw);  // fetch raw data from the registers
			if( err != ESP_OK ) {
				ESP_LOGE(FNAME, "AHRS acceleration I2C read error");
				continue;
			}
			samples++;
			accelG += mpud::accelGravity(accelRaw, mpud::ACCEL_FS_8G);  // raw data to gravity
			ESP_LOGI( FNAME,"MPU %.2f", accelG[0] );
			delay( 10 );
		}
		accelG /= samples;
		float accel = sqrt(accelG[0]*accelG[0]+accelG[1]*accelG[1]+accelG[2]*accelG[2]);
		char ahrs[10];
		sprintf(ahrs, "%.2f", accel);
		logged_tests += "MPU6050 AHRS (" + std::string(ahrs) + "g): ";
		if ( accel >0.8 && accel < 1.1 ) {
			logged_tests += passed_text;
		} else {
			logged_tests += failed_text;
		}
		IMU::init();
		if ( IMU::MPU6050Read() == ESP_OK) {
			IMU::Process();
		}
		ESP_LOGI( FNAME,"MPU current offsets accl:%d/%d/%d gyro:%d/%d/%d ZERO:%d", ab.x, ab.y, ab.z, gb.x,gb.y,gb.z, gb.isZero() );
	}

	// Create serial interfaces
	S1 = new SerialLine((uart_port_t)1, GPIO_NUM_16, GPIO_NUM_17);
	if ( hardwareRevision.get() >= XCVARIO_21 ) {
		S2 = new SerialLine((uart_port_t)2, GPIO_NUM_18, GPIO_NUM_4);
	}

	// Create CAN based on known HW revision (not the very first boot)
	if ( hardwareRevision.get() >= XCVARIO_22 ) {
		ESP_LOGI(FNAME,"NOW add/test CAN");
		CANbus::createCAN();
		logged_tests += "CAN Interface: ";
		if( CAN->selfTest() ) {
			logged_tests += passed_text;
		}
		else {
			MBOX->newMessage(1, "CAN bus: Fail");
			logged_tests += failed_text;
			ESP_LOGE(FNAME,"Error: CAN Interface failed");
		}
	}

	// DEVMAN serialization, read in all configured devices.
	DEVMAN->reserectFromNvs();
	if ( gflags.intrDevices ) {
		DEVMAN->introduceDevices(); // create a flarm etc.
	}
	if ( CAN ) {
		// just allways, it respects the XCV role setting
		DEVMAN->addDevice(CANREGISTRAR_DEV, REGISTRATION_P, CAN_REG_PORT, CAN_REG_PORT, CAN_BUS);
	}

	ESP_LOGI(FNAME,"Wirelss-ID: %s", SetupCommon::getID());
	std::string wireless_id;
	if( DEVMAN->isIntf(BT_SPP) ) {
		ESP_LOGI(FNAME,"Use BT");
		wireless_id.assign("BT ID: ");
	}
	else if( DEVMAN->isIntf(BT_LE) ) {
		ESP_LOGI(FNAME,"Use BLE");
		// blesender.begin(); fixme
	}
	else  if( DEVMAN->isIntf(WIFI_APSTA) ) {
		ESP_LOGI(FNAME,"Use WiFi");
		wireless_id.assign("WLAN SSID: ");
	}
	if ( ! gflags.schedule_reboot && custom_wireless_id.get().id[0] == '\0' ) {
		custom_wireless_id.set(SetupCommon::getDefaultID()); // Default ID created from MAC address CRC
	}
	wireless_id += SetupCommon::getID();
	if ( wireless_id.length() > 0 ) {
		MBOX->newMessage(2, wireless_id.c_str() );
	}

	{
		Cipher crypt;
		gflags.ahrsKeyValid = crypt.checkKeyAHRS();
		ESP_LOGI( FNAME, "AHRS key valid=%d", gflags.ahrsKeyValid );
		if ( ! gflags.ahrsKeyValid ) {
			// make sure the AHRS screen is not set
			screen_horizon.set(false);
		}
	}
	boot_screen->finish(0);

	ESP_LOGI(FNAME,"Airspeed sensor init..  type configured: %d", airspeed_sensor_type.get() );
	int offset;
	bool found = false;
	if( hardwareRevision.get() >= XCVARIO_20 ){ // autodetect new type of sensors in any case
		ESP_LOGI(FNAME," HW revision 3, check configured airspeed sensor");
		bool valid_config=true;
		switch( airspeed_sensor_type.get() ){
		case PS_TE4525:
			asSensor = new MS4525DO();
			ESP_LOGI(FNAME,"MS4525DO configured");
			break;
		case PS_ABPMRR:
			asSensor = new ABPMRR();
			ESP_LOGI(FNAME,"ABPMRR configured");
			break;
		case PS_MP3V5004:
			asSensor = new MP5004DP();
			ESP_LOGI(FNAME,"PS_MP3V5004 configured");
			break;
		case PS_MCPH21:
			asSensor = new MCPH21();
			ESP_LOGI(FNAME,"PS_MCPH21 configured");
			break;
		default:
			valid_config = false;
			ESP_LOGI(FNAME,"No valid config found");
			break;
		}
		if( valid_config ){
			ESP_LOGI(FNAME,"There is valid config for airspeed sensor: check this one first...");
			asSensor->setBus( &i2c );
			if( asSensor->selfTest( offset ) ){
				ESP_LOGI(FNAME,"Selftest for configured sensor OKAY");
				found = true;
			}
			else{
				ESP_LOGI(FNAME,"AS sensor not found");
				delete asSensor;
			}
		}
		// Probe any kind of ever known sensors
		if( !found ){   // behaves same as above, so we can't detect this, needs to be setup in factory
			ESP_LOGI(FNAME,"Try MCPH21");
			asSensor = new MCPH21();
			asSensor->setBus( &i2c );
			ESP_LOGI(FNAME,"Try MCPH21");
			if( asSensor->selfTest( offset ) ){
				airspeed_sensor_type.set( PS_MCPH21 );
				found = true;
			}
			else{
				ESP_LOGI(FNAME,"MCPH21 sensor not found");
				delete asSensor;
			}
			delay( 100 );
		}
		if( !found ){
			ESP_LOGI(FNAME,"Try ABPMRR");
			asSensor = new ABPMRR();
			asSensor->setBus( &i2c );
			if( asSensor->selfTest( offset ) ){
				airspeed_sensor_type.set( PS_ABPMRR );
				found = true;
			}
			else{
				ESP_LOGI(FNAME,"ABPMRR sensor not found");
				delete asSensor;
			}
		}
		if( !found ){   // behaves same as above, so we can't detect this, needs to be setup in factory
			ESP_LOGI(FNAME,"Configured sensor not found");
			asSensor = new MS4525DO();
			asSensor->setBus( &i2c );
			ESP_LOGI(FNAME,"Try MS4525");
			if( asSensor->selfTest( offset ) ){
				airspeed_sensor_type.set( PS_ABPMRR );
				found = true;
			}
			else{
				ESP_LOGI(FNAME,"MS4525DO sensor not found");
				delete asSensor;
			}
		}
		if( !found ){
			ESP_LOGI(FNAME,"Try MP5004DP");
			asSensor = new MP5004DP();
			asSensor->setBus( &i2c );
			if( asSensor->selfTest( offset ) ){
				ESP_LOGI(FNAME,"MP5004DP selfTest OK");
				airspeed_sensor_type.set( PS_MP3V5004 );
				found = true;
			}
			else{
				ESP_LOGI(FNAME,"MP5004DP sensor not found");
				delete asSensor;
			}
		}
	}
	logged_tests += "AS Sensor offset: ";
	if( found ){
		ESP_LOGI(FNAME,"AS Speed sensors self test PASSED, offset=%d", offset);
		asSensor->doOffset();
		bool offset_plausible = asSensor->offsetPlausible( offset );
		bool ok=false;
		float p=asSensor->readPascal(60, ok);
		if( ok )
			dynamicP = p;
		ias.set( Atmosphere::pascal2kmh( dynamicP ) );
		ESP_LOGI(FNAME,"Aispeed sensor current speed=%f", ias.get() );
		if( !offset_plausible && ( ias.get() < 50 ) ){
			ESP_LOGE(FNAME,"Error: air speed presure sensor offset out of bounds, act value=%d", offset );
			MBOX->newMessage(2, "AS Sensor: NEED ZERO");
			logged_tests += failed_text;
			selftestPassed = false;
		}
		else {
			ESP_LOGI(FNAME,"air speed offset test PASSED, readout value in bounds=%d", offset );
			logged_tests += passed_text;
			boot_screen->finish(1);
		}
	}
	else{
		ESP_LOGE(FNAME,"Error with air speed pressure sensor, no working sensor found!");
		MBOX->newMessage(2, "AS Sensor: NOT FOUND");
		logged_tests += "NOT FOUND\n";
		selftestPassed = false;
		asSensor = 0;
	}
	ESP_LOGI(FNAME,"Now start T sensor test");
	// Temp Sensor test
	if( !SetupCommon::isClient()  ) {
		ESP_LOGI(FNAME,"Now start T sensor test");
		temperature = DEVICE_DISCONNECTED_C;
		if( ds18b20.reset() )
		{
			t_devices = ds18b20.search(t_addr, 1);
			// ESP_LOGI(FNAME,"Temperatur Sensors found N=%d Addr: %llx", t_devices, t_addr[0] );
		}
		if( t_devices ){
			ESP_LOGI(FNAME,"Temp devices %d", t_devices);
			uint8_t err = ds18b20.getTemp(t_addr[0], temperature );
			if( err ){
				temperature = DEVICE_DISCONNECTED_C;
			}
		}
		ESP_LOGI(FNAME,"End T sensor test");
		logged_tests += "Ext. Temp. Sensor: ";
		if( temperature == DEVICE_DISCONNECTED_C ) {
			ESP_LOGE(FNAME,"Error: Self test Temperatur Sensor failed; returned T=%2.2f", temperature );
			MBOX->newMessage(1, "Temp Sensor: NOT FOUND");
			gflags.validTemperature = false;
			logged_tests += "NOT FOUND\n";
		}else
		{
			ESP_LOGI(FNAME,"Self test Temperatur Sensor PASSED; returned T=%2.2f", temperature );
			gflags.validTemperature = true;
			logged_tests += passed_text;

		}
	}
	ESP_LOGI(FNAME,"Absolute pressure sensors init, detect type of sensor type..");

	float ba_t, ba_p, te_t, te_p;
	SPL06_007 *splBA = new SPL06_007( SPL06_007_BARO );
	SPL06_007 *splTE = new SPL06_007( SPL06_007_TE );
	splBA->setBus( &i2c );
	splTE->setBus( &i2c );
	bool baok =  splBA->begin();
	bool teok =  splTE->begin();
	if( baok || teok ){
		ESP_LOGI(FNAME,"SPL06_007 type detected");
		i2c.begin(GPIO_NUM_21, GPIO_NUM_22, 100000 );  // higher speed, we have 10K pullups on that board
		baroSensor = splBA;
		teSensor = splTE;
	}
	else{
		delete splBA;
		ESP_LOGI(FNAME,"No SPL06_007 chip detected, now check BMP280");
		BME280_ESP32_SPI *bmpBA = new BME280_ESP32_SPI();
		BME280_ESP32_SPI *bmpTE= new BME280_ESP32_SPI();
		int spi_freq=rint( FREQ_BMP_SPI / 2 * ((100.0 + display_clock_adj.get())/100.0));
		bmpBA->setSPIBus(SPI_SCLK, SPI_MOSI, SPI_MISO, CS_bme280BA, spi_freq);
		bmpTE->setSPIBus(SPI_SCLK, SPI_MOSI, SPI_MISO, CS_bme280TE, spi_freq);
		bmpTE->begin();
		bmpBA->begin();
		baroSensor = bmpBA;
		teSensor = bmpTE;
		gpio_set_pull_mode(CS_bme280BA, GPIO_PULLUP_ONLY );
		gpio_set_pull_mode(CS_bme280TE, GPIO_PULLUP_ONLY );

	}
	bool tetest=true;
	bool batest=true;
	delay(200);

	logged_tests += "Baro Sensor: ";
	if( !baroSensor->selfTest( ba_t, ba_p)  ) {
		ESP_LOGE(FNAME,"HW Error: Self test Barometric Pressure Sensor failed!");
		MBOX->newMessage(2, "Baro Sensor: NOT FOUND");
		selftestPassed = false;
		batest=false;
		logged_tests += "NOT FOUND\n";
	}
	else {
		ESP_LOGI(FNAME,"Baro Sensor test OK, T=%f P=%f", ba_t, ba_p);
		logged_tests += passed_text;
	}
	logged_tests += "TE Sensor: ";
	if( !teSensor->selfTest(te_t, te_p) ) {
		ESP_LOGE(FNAME,"HW Error: Self test TE Pressure Sensor failed!");
		MBOX->newMessage(2, "TE Sensor: NOT FOUND");
		selftestPassed = false;
		tetest=false;
		logged_tests += "NOT FOUND\n";
	}
	else {
		ESP_LOGI(FNAME,"TE Sensor test OK,   T=%f P=%f", te_t, te_p);
		logged_tests += passed_text;
	}
	if( tetest && batest ) {
		ESP_LOGI(FNAME,"Both absolute pressure sensor TESTs SUCCEEDED, now test deltas");
		logged_tests += "TE/Baro Sens. T d. <4'C: ";
		if( (abs(ba_t - te_t) >4.0)  && ( ias.get() < 50 ) ) {   // each sensor has deviations, and new PCB has more heat sources
			selftestPassed = false;
			ESP_LOGE(FNAME,"Severe T delta > 4 °C between Baro and TE sensor: °C %f", abs(ba_t - te_t) );
			MBOX->newMessage(1, "TE/Baro Temp: Unequal");
			logged_tests += failed_text;
		}
		else{
			ESP_LOGI(FNAME,"Abs p sensors temp. delta test PASSED, delta: %f °C",  abs(ba_t - te_t));
			logged_tests += passed_text;
		}
		float delta = 2.5; // in factory we test at normal temperature, so temperature change is ignored.
		if( abs(factory_volt_adjust.get() - 0.00815) < 0.00001 ) {
			delta += 1.8; // plus 1.5 Pa per Kelvin, for 60K T range = 90 Pa or 0.9 hPa per Sensor, for both there is 2.5 plus 1.8 hPa to consider
		}
		logged_tests += "TE/Baro Sens. P d. <2hPa: ";
		if( (abs(ba_p - te_p) >delta)  && ( ias.get() < 50 ) ) {
			selftestPassed = false;
			ESP_LOGI(FNAME,"Abs p sensors deviation delta > 2.5 hPa between Baro and TE sensor: %f", abs(ba_p - te_p) );
			MBOX->newMessage(1, "TE/Baro P: Unequal");
			logged_tests += failed_text;
		}
		else {
			ESP_LOGI(FNAME,"AbsP sensor deta test PASSED, D: %f hPa", abs(ba_p - te_p) );
			logged_tests += passed_text;
		}
		boot_screen->finish(2);
	}
	else {
		ESP_LOGI(FNAME,"Absolute pressure sensor TESTs failed");
	}

	bmpVario.begin( teSensor, baroSensor, &Speed2Fly );
	bmpVario.setup();
	ESP_LOGI(FNAME,"Audio begin");
	AUDIO->begin( DAC_CHAN_0 );
	ESP_LOGI(FNAME,"Poti and Audio test");
	logged_tests += "Digi. Audio Poti test: ";
	if( !AUDIO->selfTest() ) {
		ESP_LOGE(FNAME,"Error: Digital potentiomenter selftest failed");
		MBOX->newMessage(1, "Digital Poti: Failure");
		selftestPassed = false;
		logged_tests += failed_text;
	}
	else{
		ESP_LOGI(FNAME,"Digital potentiometer test PASSED");
		logged_tests += passed_text;
	}
	audio_volume.set(default_volume.get());

	// 2021 series 3, or 2022 model with new digital poti CAT5171 also features CAN bus
	// do not move the check unless you know the sequence of HW detection
	if ( !CAN && AUDIO->haveCAT5171() )
	{
		// fixme, shouldnt be the HW increased to 22 based on audio poti??
		// check on CAN available, if 100% reliable this would only be a one shot need.
		ESP_LOGI(FNAME,"probing CAN");
		CANbus::createCAN();
		if( CAN->selfTest() ){
			// series 2023 has fixed slope control, prior slope bit for AHRS temperature control
			if ( CAN->hasSlopeSupport() ) {
				if( hardwareRevision.get() < XCVARIO_22)
					hardwareRevision.set(XCVARIO_22);  // XCV-22, CAN but no AHRS temperature control
			} else {
				ESP_LOGI(FNAME,"CAN Bus selftest without RS control OK: set hardwareRevision (XCV-23)");
				if( hardwareRevision.get() < XCVARIO_23)
					hardwareRevision.set(XCVARIO_23);  // XCV-23, including AHRS temperature control
			}
		}
		delete CAN;
		CAN = nullptr;
	}


	if( gflags.haveMPU ){
		if( MPU.whoAmI() == 0x12 ){
			if( hardwareRevision.get() < XCVARIO_25){
				hardwareRevision.set(XCVARIO_25);  // XCV-25: ICL20602
				ESP_LOGI(FNAME,"MPU ICM-20602 -> hardwareRevision (XCV-25)");
			}
		}
	}

	float bat = Battery.get(true);
	logged_tests += "Battery Voltage Sensor: ";
	if( bat < 1 || bat > 28.0 ){
		ESP_LOGE(FNAME,"Error: Battery voltage metering out of bounds, act value=%f", bat );
		MBOX->newMessage(1, "Bat Meter: Fail");
		logged_tests += failed_text;
		selftestPassed = false;
	}
	else{
		ESP_LOGI(FNAME,"Battery voltage metering test PASSED, act value=%f", bat );
		logged_tests += passed_text;
	}

	if ( BTspp) {
			logged_tests += "Bluetooth test: ";
		if ( BTspp->selfTest() ) {
			logged_tests += passed_text;
		}
		else {
			MBOX->newMessage(1, "Bluetooth: FAILED");
			logged_tests += failed_text;
		}
	}

	// magnetic sensor / compass selftest
	if( theCompass ) {
		logged_tests += "Compass test: ";
		theCompass->begin();
		ESP_LOGI( FNAME, "Magnetic sensor enabled: initialize");
		esp_err_t err = theCompass->selfTest();
		if( err == ESP_OK )		{
			// Activate working of magnetic sensor
			ESP_LOGI( FNAME, "Magnetic sensor selftest: OKAY");
			logged_tests += passed_text;
		}
		else{
			ESP_LOGI( FNAME, "Magnetic sensor selftest: FAILED");
			MBOX->newMessage(1, "Compass: FAILED");
			logged_tests += failed_text;
			selftestPassed = false;
		}
		theCompass->start();  // start task
	}

	// hardware components now got all detected
	if ( gflags.schedule_reboot ) {
		SetupCommon::commitDirty();
		esp_restart();
	}

	Speed2Fly.begin();
	Version myVersion;
	ESP_LOGI(FNAME,"Program Version %s", myVersion.version() );
	ESP_LOGI(FNAME,"\n\n%s", logged_tests.c_str());
	if( !selftestPassed )
	{
		ESP_LOGI(FNAME,"\n\n\nSelftest failed, see above LOG for Problems\n\n\n");
		MBOX->newMessage(2, "Selftest FAILED");
		if( !Rotary->readBootupStatus() )
			sleep(4);
	}
	else{
		ESP_LOGI(FNAME,"\n\n\n*****  Selftest PASSED  ********\n\n\n");
		boot_screen->finish(3); // signal self tests passed
	}

	if( Rotary->readBootupStatus() )
	{
		LeakTest::start( baroSensor, teSensor, asSensor );
	}

	// Set QNH from setup Airfiled elevation, when ! Second && ! airborn
	if( ! SetupCommon::isClient() && ias.get() < 50.0 ) {

		// remove logo immidiately
		sleep(1);
		delete boot_screen;

		ESP_LOGI(FNAME,"Master Mode: QNH Autosetup, IAS=%3f (<50 km/h)", ias.get() );
		// QNH autosetup
		float ae = elevation.get();
		float qnh_best = QNH.get();
		bool ok;
		baroP = baroSensor->readPressure(ok);
		if( ae > NOTSET_ELEVATION ) {
			float step=10.0; // 80 m
			float min=1000.0;
			for( float qnh = 870; qnh< 1085; qnh+=step ) {
				float alt = 0;
				if( Flarm::validExtAlt() && alt_select.get() == AS_EXTERNAL )
					alt = alt_external + (qnh  - 1013.25) * 8.2296;  // correct altitude according to ISA model = 27ft / hPa
				else
					alt = baroSensor->readAltitude( qnh, ok);
				float diff = alt - ae;
				// ESP_LOGI(FNAME,"Alt diff=%4.2f  abs=%4.2f", diff, abs(diff) );
				if( abs( diff ) < 100 )
					step=1.0;  // 8m
				if( abs( diff ) < 10 )
					step=0.05;  // 0.4 m
				if( abs( diff ) < abs(min) ) {
					min = diff;
					qnh_best = qnh;
					// ESP_LOGI(FNAME,"New min=%4.2f", min);
				}
				if( diff > 1.0 ) // we are ready, values get already positive
					break;
				delay(50);
			}
			ESP_LOGI(FNAME,"Auto QNH=%4.2f\n", qnh_best);
			QNH.set( qnh_best );
		}
		Display->clear();
		if ( NEED_VOLTAGE_ADJUST ) {
			ESP_LOGI(FNAME,"Do Factory Voltmeter adj");
			Menu->begin(SetupMenu::createVoltmeterAdjustMenu());
		}
		else {
			Menu->begin(SetupMenu::createQNHMenu());
		}
	}
	else {
		if ( SetupCommon::isClient() ) {
			bool already_connected = false;
			Device *dev = DEVMAN->getDevice(XCVARIOFIRST_DEV);
			if ( dev ) {
				NmeaPlugin *plg = dev->_link->getNmeaPlugin(XCVSYNC_P);
				if ( plg ) {
					already_connected = static_cast<XCVSyncMsg*>(plg)->syncStarted();
				}
			}
			if ( ! already_connected ) {
				// just sit, wait, show a little message
				MBOX->newMessage(1, "Waiting for XCV Master");
				ESP_LOGI(FNAME,"Client Mode: Wait for Master XCVario %p", dev);
			}
		}

		delete boot_screen;
		Display->clear();
		gflags.inSetup = false;
	}

	// Wind calculation
	WindCalcTask::createWindResources();

	// Init the vario screens
	SetupRoot::initScreens();

	if ( flap_enable.get() ) {
		Flap::init(MYUCG);
	}
	if( hardwareRevision.get() != XCVARIO_20 ){
		gpio_pullup_en( GPIO_NUM_34 );
		if( gflags.haveMPU && HAS_MPU_TEMP_CONTROL && !gflags.mpu_pwm_initalized  )
		{
			// series 2023 does not have slope support on CAN bus but MPU temperature control
			MPU.pwm_init();
			gflags.mpu_pwm_initalized = true;
		}
	}
	if( screen_centeraid.get() ){
		centeraid = new CenterAid( MYUCG );
	}

	// enter normal operation


	if( SetupCommon::isClient() ){
		xTaskCreate(&clientLoop, "clientLoop", 4096, NULL, 11, &bpid);
	}
	else {
		xTaskCreate(&readSensors, "readSensors", 5120, NULL, 12, &bpid);
	}
	xTaskCreate(&readTemp, "readTemp", 3000, NULL, 5, &tpid); // increase stack by 500 byte

	AUDIO->startAudio();
}

extern "C" void  app_main(void)
{
	// global log level
	esp_log_level_set("*", ESP_LOG_INFO);

	// Mute audio
	AUDIO = new Audio();
	AUDIO->mute();

	// Access to the non volatile setup
	ESP_LOGI(FNAME,"app_main" );
	DeviceManager::Instance(); // Create a blank DM, because on a cleard flash initSetup starts to access it.
	ESP32NVS::CreateInstance(); // NVS is needed for the SetupCommon::initSetup() to work, and to query nvs var existance
	// Check on the existance of some nvs variables
	if ( ! ahrs_licence_dig1.exists() ) {
		Cipher crypt;
		crypt.initTest();
	}
	if ( ! flarm_devsetup.exists() ) {
		ESP_LOGI(FNAME,"Init devices" );
		gflags.intrDevices = true;
	}
	ESP_LOGI(FNAME,"Now init all Setup elements");
	SetupCommon::initSetup();

	// Instance to a simple esp timer based clock
	MY_CLOCK = new Clock();

	// Figure HW revision first
	if( hardwareRevision.get() == HW_UNKNOWN ){  // per default we assume there is XCV-20
		ESP_LOGI( FNAME, "Hardware Revision unknown, set revision 2 (XCV-20)");
		hardwareRevision.set(XCVARIO_20);
		// Schedule a reboot after hardware revision got clarified
		gflags.schedule_reboot = true;
	}

	// start i2c bus
	ESP_LOGI( FNAME, "Now setup I2C bus IO 21/22");
	i2c.begin(GPIO_NUM_21, GPIO_NUM_22, 100000 );

	// probe on MPU
	MPU.setBus(i2c);  // set communication bus, for SPI -> pass 'hspi'
	MPU.setAddr(mpud::MPU_I2CADDRESS_AD0_LOW);  // set address or handle, for SPI -> pass 'mpu_spi_handle'
	if( (MPU.reset() == ESP_OK) && hardwareRevision.get() < XCVARIO_21 ){
		ESP_LOGI( FNAME,"MPU avail, increase hardware revision to 3 (XCV-21)");
		hardwareRevision.set(XCVARIO_21);  // there is MPU6050 gyro and acceleration sensor, at least we got an XCV-21
	}
	ESP_LOGI( FNAME,"Hardware revision %d", hardwareRevision.get());
	MPU.clearpwm(); // Stop MPU heating

	// Init ui and screen drawDisplay task recources
	uiEventQueue = xQueueCreate(10, sizeof(int));

	// Init of rotary
	if( hardwareRevision.get() == XCVARIO_20 ){
		Rotary = new ESPRotary( GPIO_NUM_4, GPIO_NUM_2, GPIO_NUM_0); // XCV-20 uses GPIO_2 for Rotary
	}
	else {
		Rotary = new ESPRotary( GPIO_NUM_39, GPIO_NUM_36, GPIO_NUM_0);
	}

#ifdef Quaternionen_Test
		Quaternion::quaternionen_test();
#endif
#ifdef WMM_Test
		WMM_Model::geomag_test();
#endif
	system_startup( 0 );
	Rotary->updateRotDir();   // Update Rotary direction after XCVario hardware has been detected
	vTaskDelete( NULL );
}
