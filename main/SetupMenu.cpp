/*
 * SetupMenu.cpp
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#include "SetupMenu.h"
#include "IpsDisplay.h"
#include "ESPAudio.h"
#include "BMPVario.h"
#include "S2F.h"
#include "Version.h"
#include "Polars.h"
#include "Cipher.h"
#include "Units.h"
#include "Switch.h"
#include "Flap.h"
#include "SetupMenuSelect.h"
#include "SetupMenuValFloat.h"
#include "SetupMenuChar.h"
#include "DisplayDeviations.h"
#include "ShowCompassSettings.h"
#include "ShowCirclingWind.h"
#include "ShowStraightWind.h"
#include "MenuEntry.h"
#include "Compass.h"
#include "CompassMenu.h"
#include "esp_wifi.h"
#include "Flarm.h"
#include "WifiClient.h"
#include "Blackboard.h"
#include "DataMonitor.h"

#include <inttypes.h>
#include <iterator>
#include <algorithm>
#include <logdef.h>
#include <sensor.h>
#include <cstring>
#include <string>
#include "SetupNG.h"
#include "quaternion.h"

SetupMenuSelect * audio_range_sm = 0;
SetupMenuSelect * mpu = 0;

// Menu for flap setup

float elev_step = 1;
static uint8_t screen_mask_len = 1;

bool SetupMenu::focus = false;

int gload_reset( SetupMenuSelect * p ){
	gload_pos_max.set(0);
	gload_neg_max.set(0);
	airspeed_max.set(0);
	return 0;
}

int compass_ena( SetupMenuSelect * p ){
	return 0;
}

void init_routing(){
	uint32_t s1rt = (uint32_t)serial1_tx.get();
	rt_s1_xcv.set( (s1rt >> (RT_XCVARIO))& 1 );
	rt_s1_wl.set( (s1rt >> (RT_WIRELESS))& 1 );
	rt_s1_s2.set( (s1rt >> (RT_S1))& 1 );
	rt_s1_can.set( (s1rt >> (RT_CAN))& 1 );

	uint32_t s2rt = (uint32_t)serial2_tx.get();
	rt_s2_xcv.set( (s2rt >> (RT_XCVARIO))& 1 );
	rt_s2_wl.set( (s2rt >> (RT_WIRELESS))& 1 );
	rt_s1_s2.set( (s2rt >> (RT_S1))& 1 );
	rt_s2_can.set( (s2rt >> (RT_CAN))& 1 );
}

int update_routing_s1( SetupMenuSelect * p ){
	uint32_t routing =
			( (uint32_t)rt_s1_xcv.get() << (RT_XCVARIO) ) |
			( (uint32_t)rt_s1_wl.get()  << (RT_WIRELESS) ) |
			( (uint32_t)rt_s1_s2.get()  << (RT_S1) ) |
			( (uint32_t)rt_s1_can.get() << (RT_CAN) );
	serial1_tx.set( routing );
	return 0;
}



void init_screens(){
	uint32_t scr = menu_screens.get();
	screen_gmeter.set( (scr >> SCREEN_GMETER) & 1);
	// 	screen_centeraid.set( (scr >> SCREEN_THERMAL_ASSISTANT) & 1);
	screen_flarm.set( (scr >> SCREEN_FLARM) & 1);
	screen_mask_len = 1; // default vario
	while( scr ){
		scr = scr >> 1;
		screen_mask_len++;
	}
	ESP_LOGI(FNAME,"screens mask len: %d, screens: %d", screen_mask_len, menu_screens.get() );
}

int vario_setup(SetupMenuValFloat * p)
{
	bmpVario.configChange();
	return 0;
}

int speedcal_change(SetupMenuValFloat * p)
{
	if( asSensor)
		asSensor->changeConfig();
	return 0;
}

gpio_num_t SetupMenu::getGearWarningIO(){
	gpio_num_t io = GPIO_NUM_0;
	if( gear_warning.get() == GW_FLAP_SENSOR || gear_warning.get() == GW_FLAP_SENSOR_INV ){
		io = GPIO_NUM_34;
	}
	else if( gear_warning.get() == GW_S2_RS232_RX || gear_warning.get() == GW_S2_RS232_RX_INV ){
		io = GPIO_NUM_18;
	}
	return io;
}

void initGearWarning(){
	gpio_num_t io = SetupMenu::getGearWarningIO();
	if( io != GPIO_NUM_0 ){
		gpio_reset_pin( io );
		gpio_set_direction(io, GPIO_MODE_INPUT);
		gpio_set_pull_mode(io, GPIO_PULLUP_ONLY);
		gpio_pullup_en( io );
	}
	ESP_LOGI(FNAME,"initGearWarning: IO: %d", io );
}

int config_gear_warning( SetupMenuSelect * p ){
	initGearWarning();
	return 0;
}

int upd_screens( SetupMenuSelect * p ){
	uint32_t screens =
			( (uint32_t)screen_gmeter.get() << (SCREEN_GMETER)  |
					//		( (uint32_t)screen_centeraid.get() << (SCREEN_THERMAL_ASSISTANT) ) |
					( (uint32_t)screen_flarm.get() << (SCREEN_FLARM) )
			);
	menu_screens.set( screens );
	init_screens();
	return 0;
}


void build_streams(){
	if( nmea_streams.get() & FT_IMU )
		IMUstream = true;
	else
		IMUstream = false;
	if( nmea_streams.get() & FT_SENSOR )
		SENstream = true;
	else
		SENstream = false;
}

int change_streams( SetupMenuSelect * p ){
	build_streams();
	return 0;
}

int update_routing_s2( SetupMenuSelect * p ){
	uint32_t routing =  (uint32_t)rt_s2_xcv.get()       << (RT_XCVARIO) |
			( (uint32_t)rt_s2_wl.get() << (RT_WIRELESS) ) |
			( (uint32_t)rt_s1_s2.get()       << (RT_S1) ) |
			( (uint32_t)rt_s2_can.get()      << (RT_CAN) );
	serial2_tx.set( routing );
	return 0;
}

int update_s2f_speed(SetupMenuValFloat * p)
{
	Switch::setCruiseSpeed( Units::Airspeed2Kmh( s2f_speed.get() ) );
	return 0;
}

int update_rentry(SetupMenuValFloat * p)
{
	// ESP_LOGI(FNAME,"update_rentry() entries: %d, vu:%s ", audio_range_sm->numEntries(), Units::VarioUnit() );
	static char rentry1[20];
	static char rentry2[20];
	static char rentry3[25];

	sprintf( rentry1, "Fix (5  %s)", Units::VarioUnit() );
	bool entry_in = audio_range_sm->numEntries() == 3 ? true : false;
	if( !entry_in )
		audio_range_sm->addEntry( rentry1  );
	else
		audio_range_sm->updateEntry( rentry1, 1 );
	sprintf( rentry2, "Fix (10 %s)", Units::VarioUnit() );
	if( !entry_in )
		audio_range_sm->addEntry( rentry2 );
	else
		audio_range_sm->updateEntry( rentry2, 2 );
	sprintf( rentry3, "Variable (%d %s)", (int)(range.get()), Units::VarioUnit() );
	if( !entry_in )
		audio_range_sm->addEntry( rentry3 );
	else
		audio_range_sm->updateEntry( rentry3, 3 );
	entry_in = true;
	return 0;
}

int update_rentrys(SetupMenuSelect * p){
	update_rentry(0);
	return 0;
}

int update_wifi_power(SetupMenuValFloat * p)
{
	ESP_ERROR_CHECK(esp_wifi_set_max_tx_power( int(wifi_max_power.get()*80.0/100.0) ));
	return 0;
}

int data_mon( SetupMenuSelect * p ){
	ESP_LOGI(FNAME,"data_mon( %d ) ", data_monitor.get() );
	if( data_monitor.get() != MON_OFF ){
		DM.start(p);
	}
	return 0;
}

int update_id( SetupMenuChar * p){
	const char *c = p->getEntry();
	ESP_LOGI(FNAME,"New Letter %c Index: %d", *c, p->getCharIndex() );
	char id[10] = { 0 };
	strcpy( id, custom_wireless_id.get().id );
	id[p->getCharIndex()] = *c;
	ESP_LOGI(FNAME,"New ID %s", id );
	custom_wireless_id.set( id );
	return 0;
}

int add_key( SetupMenuSelect * p )
{
	ESP_LOGI(FNAME,"add_key( %d ) ", p->getSelect() );
	if( Cipher::checkKeyAHRS() ){
		if( !mpu->existsEntry( "Enable") )
			mpu->addEntry( "Enable");
	}
	else{
		if( mpu->existsEntry( "Enable") )
			mpu->delEntry( "Enable");
	}
	return 0;
}

int qnh_adj( SetupMenuValFloat * p )
{
	ESP_LOGI(FNAME,"qnh_adj %f", QNH.get() );
	float alt=0;
	if( Flarm::validExtAlt() && alt_select.get() == AS_EXTERNAL ){
		alt = alt_external + ( QNH.get() - 1013.25)*8.2296;  // correct altitude according to ISA model = 27ft / hPa
	}
	else{
		int samples = 0;
		for( int i=0; i<6; i++ ) {
			bool ok;
			float a = p->_bmp->readAltitude( QNH.get(), ok );
			if( ok ){  // only consider correct readouts
				alt += a;
				samples++;
			}
			delay(10);
		}
		alt = alt/(float)samples;
	}
	ESP_LOGI(FNAME,"Setup BA alt=%f QNH=%f hPa", alt, QNH.get()  );
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	p->ucg->setFont(ucg_font_fub25_hr, true);
	float altp;
	const char *u = "m";
	if( alt_unit.get() == 0 ){ // m
		altp = alt;
	}
	else {
		u = "ft";
		altp = Units::meters2feet( alt );
	}
	p->ucg->setPrintPos(1,120);
	p->ucg->printf("%5d %s  ", (int)(altp+0.5), u );

	p->ucg->setFont(ucg_font_ncenR14_hr);
	xSemaphoreGive(spiMutex );
	return 0;
}

// Battery Voltage Meter Calibration
int factv_adj( SetupMenuValFloat * p )
{
	ESP_LOGI(FNAME,"factv_adj");
	float bat = p->_adc->get(true);
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	p->ucg->setPrintPos(1,100);
	p->ucg->printf("%0.2f Volt", bat);
	xSemaphoreGive(spiMutex );
	return 0;
}

int master_xcv_lock( SetupMenuSelect * p ){
	ESP_LOGI(FNAME,"master_xcv_lock");
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	p->ucg->setPrintPos(1,130);
	int mxcv = WifiClient::getScannedMasterXCV();
	p->ucg->printf("Scanned: XCVario-%d", mxcv );
	xSemaphoreGive(spiMutex );
	if( master_xcvario_lock.get() == 1 )
		master_xcvario.set( mxcv );
	return 0;
}

int polar_select( SetupMenuSelect * p )
{
	int index = Polars::getPolar( p->getSelect() ).index;
	ESP_LOGI(FNAME,"glider-index %d, glider num %d", index, p->getSelect() );
	glider_type_index.set( index );
	return 0;
}

void print_fb( SetupMenuValFloat * p, float wingload ){
	p->ucg->setFont(ucg_font_fub25_hr, true);
	p->ucg->setPrintPos(1,110);
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	p->ucg->printf("%0.2f kg/m2  ", wingload );
	xSemaphoreGive(spiMutex );
	p->ucg->setFont(ucg_font_ncenR14_hr);
}

int water_adj( SetupMenuValFloat * p )
{
	float wingload = (ballast_kg.get() + crew_weight.get()+ empty_weight.get()) / polar_wingarea.get();
	ESP_LOGI(FNAME,"water_adj() wingload:%.1f empty: %.1f cw:%.1f water:%.1f", wingload, empty_weight.get(), crew_weight.get(), ballast_kg.get() );
	print_fb( p, wingload );
	return 0;
}

int empty_weight_adj( SetupMenuValFloat * p )
{
	float wingload = (ballast_kg.get() + crew_weight.get()+ empty_weight.get()) / polar_wingarea.get();
	print_fb( p, wingload );
	return 0;
}


int crew_weight_adj( SetupMenuValFloat * p )
{
	float wingload = (ballast_kg.get() + empty_weight.get()+ crew_weight.get()) / polar_wingarea.get();
	print_fb( p, wingload );
	return 0;
}

int bug_adj( SetupMenuValFloat * p ){
	return 0;
}

int vol_adj( SetupMenuValFloat * p ){
	// Audio::setVolume( (int)(*(p->_value)) );
	return 0;
}

/**
 * C-Wrappers function to compass menu handlers.
 */
static int compassDeviationAction( SetupMenuSelect *p )
{
	if( p->getSelect() == 0 ){
		CompassMenu::deviationAction( p );
	}
	return 0;
}

static int compassResetDeviationAction( SetupMenuSelect *p )
{
	return CompassMenu::resetDeviationAction( p );
}

static int compassDeclinationAction( SetupMenuValFloat *p )
{
	return CompassMenu::declinationAction( p );
}

static int windResetAction( SetupMenuSelect *p )
{
	if( p->getSelect() == 1 )
	{
		// Reset is selected, set default values
		wind_as_min.set( 25 );
	}
	return 0;
}

static int eval_chop( SetupMenuSelect *p )
{
	Audio::evaluateChopping();
	return 0;
}

static int compassSensorCalibrateAction( SetupMenuSelect *p )
{
	ESP_LOGI(FNAME,"compassSensorCalibrateAction()");
	if( p->getSelect() != 0 ){ // Start, Show
		CompassMenu::sensorCalibrationAction( p );
	}
	p->setSelect( 0 );
	return 0;
}

SetupMenu::SetupMenu() : MenuEntry() {
	highlight = -1;
	_parent = 0;
	helptext = 0;
}

SetupMenu::SetupMenu( const char *title ) : MenuEntry() {
	// ESP_LOGI(FNAME,"SetupMenu::SetupMenu( %s ) ", title );
	attach(this);
	_title = title;
	highlight = -1;
}

SetupMenu::~SetupMenu()
{
	// ESP_LOGI(FNAME,"del SetupMenu( %s ) ", _title );
	detach(this);
}

void SetupMenu::begin( IpsDisplay* display, PressureSensor * bmp, AnalogInput *adc ){
	ESP_LOGI(FNAME,"SetupMenu() begin");
	_bmp = bmp;
	_display = display;
	ucg = display->getDisplay();
	_adc = adc;
	setup();
	audio_volume.set( default_volume.get() );
	init_routing();
	init_screens();
	initGearWarning();
	build_streams();
}

void SetupMenu::catchFocus( bool activate ){
	focus = activate;
}

void SetupMenu::display( int mode ){
	if( (selected != this) || !gflags.inSetup || focus )
		return;
	xSemaphoreTake(display_mutex,portMAX_DELAY);
	// ESP_LOGI(FNAME,"SetupMenu display( %s)", _title );
	clear();
	int y=25;
	// ESP_LOGI(FNAME,"Title: %s y=%d child size:%d", selected->_title,y, _childs.size()  );
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setFont(ucg_font_ncenR14_hr);
	ucg->setPrintPos(1,y);
	ucg->setFontPosBottom();
	ucg->printf("<< %s",selected->_title);
	ucg->drawFrame( 1,(selected->highlight+1)*25+3,238,25 );
	for (int i=0; i<_childs.size(); i++ ){
		MenuEntry * child = _childs[i];
		ucg->setPrintPos(1,(i+1)*25+25);
		ucg->setColor( COLOR_HEADER_LIGHT );
		ucg->printf("%s",child->_title);
		// ESP_LOGI(FNAME,"Child Title: %s", child->_title );
		if( child->value() ){
			int fl=ucg->getStrWidth( child->_title );
			ucg->setPrintPos(1+fl,(i+1)*25+25);
			ucg->printf(": ");
			ucg->setPrintPos(1+fl+ucg->getStrWidth( ":" ),(i+1)*25+25);
			ucg->setColor( COLOR_WHITE );
			ucg->printf(" %s",child->value());
		}
		ucg->setColor( COLOR_WHITE );
		// ESP_LOGI(FNAME,"Child: %s y=%d",child->_title ,y );
	}
	y+=170;
	xSemaphoreGive(spiMutex );
	showhelp( y );
	xSemaphoreGive(display_mutex);
}

void SetupMenu::down(int count){
	if( selected == this && !gflags.inSetup ) {
		// ESP_LOGI(FNAME,"root: down");
		if( rot_default.get() == 1) {	 // MC Value
			float mc = MC.get();
			float step = Units::Vario2ms( 0.1 );
			mc -= step*count;
			if( mc < 0.0 )
				mc = 0.0;
			MC.set( mc );
		}
		else{  // Volume
			int vol = (int)audio_volume.get();
			vol -= count*2;
			vol = std::max( vol, 0 );
			audio_volume.set( vol );
		}
	}
	if( (selected != this) || !gflags.inSetup )
		return;
	// ESP_LOGI(FNAME,"down %d %d", highlight, _childs.size() );
	if( focus )
		return;
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setColor(COLOR_BLACK);
	ucg->drawFrame( 1,(highlight+1)*25+3,238,25 );
	ucg->setColor(COLOR_WHITE);
	if( highlight  > -1 ){
		highlight --;
	}
	else
		highlight = (int)(_childs.size() -1 );
	ucg->drawFrame( 1,(highlight+1)*25+3,238,25 );
	xSemaphoreGive(spiMutex );
	pressed = true;
}

void SetupMenu::up(int count){
	if( selected == this && !gflags.inSetup ) {
		// ESP_LOGI(FNAME,"root: up");
		if(rot_default.get() == 1) {	 // MC Value
			float mc = MC.get();
			// ESP_LOGI(FNAME,"MC up: %f count: %d", mc, count );
			float step = Units::Vario2ms( 0.1 );
			mc += step*count;
			if( mc > 9.9 )
				mc = 9.9;
			// ESP_LOGI(FNAME,"NEW MC: %f", mc );
			MC.set( mc );
		}
		else{  // Volume
			int vol = (int)audio_volume.get();
			vol += count;
			vol = std::min( vol, 100 );
			audio_volume.set( (float)vol );
		}
	}
	if( (selected != this) || !gflags.inSetup )
		return;
	// ESP_LOGI(FNAME,"SetupMenu::up %d %d", highlight, _childs.size() );
	if( focus )
		return;
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setColor(COLOR_BLACK);
	ucg->drawFrame( 1,(highlight+1)*25+3,238,25 );
	ucg->setColor(COLOR_WHITE);
	if( highlight < (int)(_childs.size()-1) ){
		highlight ++;
	}
	else
		highlight = -1;
	ucg->drawFrame( 1,(highlight+1)*25+3,238,25 );
	pressed = true;
	xSemaphoreGive(spiMutex );
}

void SetupMenu::showMenu(){
	// ESP_LOGI(FNAME,"showMenu() p:%d h:%d parent:%x", pressed, highlight, (int)_parent );
	// default is not pressed, so just display, but we toogle pressed state at the end
	// so next time we either step up to parent,
	if( pressed )
	{
		if( highlight == -1 ) {
			// ESP_LOGI(FNAME,"SetupMenu to parent");
			if( _parent != 0 ){
				selected = _parent;
				selected->highlight = -1;
				selected->pressed = true;
				delete_subtree();
			}
		}
		else {
			// ESP_LOGI(FNAME,"SetupMenu to child");
			if( (highlight >=0) && (highlight < (int)(_childs.size()) ) ){
				selected = _childs[highlight];
				selected->pressed = false;
			}
		}
	}
	if( (_parent == 0) && (highlight == -1) ) // entering setup menu root
	{
		if( !gflags.inSetup )
		{
			gflags.inSetup=true;
			// ESP_LOGI(FNAME,"Start Setup Menu");
			_display->doMenu(true);
			delay(200);  // fixme give display task time to finish drawing
		}
		else
		{
			// ESP_LOGI(FNAME,"End Setup Menu");
			screens_init = INIT_DISPLAY_NULL;
			_display->doMenu(false);
			SetupCommon::commitNow();
			gflags.inSetup=false;
		}
	}
	// ESP_LOGI(FNAME,"end showMenu()");
}

static int screen_index = 0;

void SetupMenu::create_subtree(){
	if( !subtree_created && menu_create_ptr ){
		(menu_create_ptr)(this);
		subtree_created = true;
		// ESP_LOGI(FNAME,"create_subtree() %d", _childs.size() );
	}
}

void SetupMenu::delete_subtree(){
	// ESP_LOGI(FNAME,"delete_subtree() %d", _childs.size() );
	if( subtree_created && menu_create_ptr ){
		subtree_created = false;
		for (int i=0; i<_childs.size(); i++ ){
			delete _childs[i];
		}
		_childs.clear();
	}
}

void SetupMenu::press(){
	if( (selected != this) || focus )
		return;
	// ESP_LOGI(FNAME,"press() active_srceen %d, pressed %d inSet %d  subtree_created: %d mptr: %p", active_screen, pressed, gflags.inSetup, subtree_created, menu_create_ptr );
	create_subtree();
	if( !gflags.inSetup ){
		active_screen = 0;
		while( !active_screen && (screen_index < screen_mask_len) ){
			if( menu_screens.get() & (1 << screen_index) ){
				active_screen = ( 1 << screen_index );
				// ESP_LOGI(FNAME,"New active_screen: %d", active_screen );
			}
			screen_index++;
		}
		if( screen_index >= screen_mask_len ){
			// ESP_LOGI(FNAME,"select vario screen");
			screen_index = 0;
			active_screen = 0; // fall back into default vario screen after optional screens
		}
	}
	if( !active_screen || gflags.inSetup ){
		// ESP_LOGI(FNAME,"press() gflags.inSetup");
		if( !menu_long_press.get() || gflags.inSetup )
			showMenu();
		if( pressed )
			pressed = false;
		else
			pressed = true;
	}
}

void SetupMenu::longPress(){
	if( (selected != this) )
		return;
	// ESP_LOGI(FNAME,"longPress()");
	ESP_LOGI(FNAME,"longPress() active_srceen %d, pressed %d inSet %d", active_screen, pressed, gflags.inSetup );
	if( menu_long_press.get() && !gflags.inSetup ){
		showMenu();
	}
	if( pressed ){
		pressed = false;
	}
	else{
		pressed = true;
#ifdef Quaternionen_Test
		Quaternion::quaternionen_test();
#endif
	}
}

void SetupMenu::escape(){
	if( gflags.inSetup ){
		ESP_LOGI(FNAME,"escape now Setup Menu");
		_display->clear();
		_display->doMenu(false);
		SetupCommon::commitNow();
		gflags.inSetup=false;
	}
}

void SetupMenu::vario_menu_create_damping( MenuEntry *top ){
	SetupMenuValFloat * vda = new SetupMenuValFloat( 	"Damping", "sec", 2.0, 10.0, 0.1, vario_setup, false, &vario_delay );
	vda->setHelp(PROGMEM"Response time, time constant of Vario low pass kalman filter");
	top->addEntry( vda );

	SetupMenuValFloat * vdav = new SetupMenuValFloat( 	"Averager", "sec", 2.0, 60.0, 1, 0, false, &vario_av_delay );
	vdav->setHelp(PROGMEM"Response time, time constant of digital Average Vario Display");
	top->addEntry( vdav );
}

void SetupMenu::vario_menu_create_meanclimb( MenuEntry *top ){
	SetupMenuValFloat * vccm = new SetupMenuValFloat( "Minimum climb", "",	0.0, 2.0, 0.1, 0, false, &core_climb_min );
	vccm->setHelp(PROGMEM"Minimum climb rate that counts for arithmetic mean climb value");
	top->addEntry( vccm );

	SetupMenuValFloat * vcch = new SetupMenuValFloat( "Duration", "min", 1, 300, 1, 0, false, &core_climb_history );
	vcch->setHelp(PROGMEM"Duration in minutes where samples for mean climb value are regarded, default is last 3 thermals or 45 min");
	top->addEntry( vcch );

	SetupMenuValFloat * vcp = new SetupMenuValFloat( "Cycle", "sec", 60, 300, 1, 0, false, &core_climb_period );
	vcp->setHelp(PROGMEM"Cycle in number of seconds when mean climb value is recalculated, default is every 60 seconds");
	top->addEntry( vcp);

	SetupMenuValFloat * vcmc = new SetupMenuValFloat( "Major Change", "m/s", 0.1, 5.0, 0.1, 0, false, &mean_climb_major_change );
	vcmc->setHelp(PROGMEM"Change in mean climb during last cycle (minute), that results in a major change indication with (arrow symbol)");
	top->addEntry( vcmc);
}

void SetupMenu::vario_menu_create_s2f( MenuEntry *top ){
	SetupMenuValFloat * vds2 = new SetupMenuValFloat( "Damping", "sec", 0.10001, 10.0, 0.1, 0, false, &s2f_delay );
	vds2->setHelp(PROGMEM"Time constant of S2F low pass filter");
	top->addEntry( vds2 );

	SetupMenuSelect * blck = new SetupMenuSelect( "Blockspeed", false, 0 , true, &s2f_blockspeed );
	blck->setHelp(PROGMEM"With Blockspeed enabled, vertical movement of airmass or G-load is not considered for speed to fly (S2F) calculation");
	blck->addEntry( "DISABLE");
	blck->addEntry( "ENABLE");
	top->addEntry( blck );

	SetupMenuSelect * s2fmod = new SetupMenuSelect( "S2F Mode", false, 0 , true, &s2f_switch_mode );
	s2fmod->setHelp( PROGMEM"Select source for S2F <-> Vario change, that supports multiple ways", 230 );
	s2fmod->addEntry( "Vario fix");
	s2fmod->addEntry( "Cruise fix");
	s2fmod->addEntry( "Switch");
	s2fmod->addEntry( "AutoSpeed");
	s2fmod->addEntry( "External");
	s2fmod->addEntry( "Flap");
	s2fmod->addEntry( "AHRS-Gyro");
	top->addEntry( s2fmod );

	SetupMenuValFloat * autospeed = new SetupMenuValFloat( "S2F AutoSpeed", "", 20.0, 250.0, 1.0, update_s2f_speed, false, &s2f_speed );
	top->addEntry( autospeed );
	autospeed->setHelp(PROGMEM"Transition speed in AutoSpeed mode to switch Vario <-> Cruise (S2F) mode");

	SetupMenuValFloat * s2f_flap = new SetupMenuValFloat( "S2F Flap Pos", "", -3, 3, 0.1, 0 , false, &s2f_flap_pos );
	top->addEntry( s2f_flap );
	s2f_flap->setHelp(PROGMEM"Precise flap position in Flap mode to switch Vario <-> Cruise (S2F) mode");

	SetupMenuValFloat * s2f_gyro = new SetupMenuValFloat( "S2F AHRS Deg", "°", 0, 100, 1, 0 , false, &s2f_gyro_deg );
	top->addEntry( s2f_gyro );
	s2f_gyro->setHelp(PROGMEM"Attitude change in degree per second to switch Vario <-> Cruise (S2F) mode");

	SetupMenuValFloat * s2fhy = new SetupMenuValFloat( "Hysteresis", "",	-20, 20, 1, 0, false, &s2f_hysteresis );
	s2fhy->setHelp(PROGMEM"Hysteresis for auto S2F transition at autospeed +- this value");
	top->addEntry( s2fhy );

	SetupMenuSelect * s2fnc = new SetupMenuSelect( "Arrow Color", false, 0 , true, &s2f_arrow_color );
	s2fnc->setHelp( PROGMEM"Select color of the S2F arrow's when painted in Up/Down position" );
	s2fnc->addEntry( "White/White");
	s2fnc->addEntry( "Blue/Blue");
	s2fnc->addEntry( "Green/Red");
	top->addEntry( s2fnc );
}

void SetupMenu::vario_menu_create_ec( MenuEntry *top ){
	SetupMenuSelect * enac = new SetupMenuSelect( "eCompensation", false, 0 , false, &te_comp_enable );
	enac->setHelp(PROGMEM"Enable/Disable electronic TE compensation option; Enable only when TE pressure is connected to ST (static) pressure");
	enac->addEntry( "DISABLE");
	enac->addEntry( "ENABLE");
	top->addEntry( enac );

	SetupMenuValFloat * elca = new SetupMenuValFloat( "Adjustment", "%",	-100, 100, 0.1, 0, false, &te_comp_adjust );
	elca->setHelp(PROGMEM"Adjustment option for electronic compensation in %. This affects in % the energy altitude calculated from airspeed");
	top->addEntry( elca );
};

void SetupMenu::vario_menu_create( MenuEntry *vae ){
	ESP_LOGI(FNAME,"SetupMenu::vario_menu_create( %p )", vae );

	SetupMenuValFloat * vga = new SetupMenuValFloat( "Range", "",	1.0, 30.0, 1, 0, true, &range );
	vga->setHelp(PROGMEM"Upper and lower value for Vario graphic display region");
	vga->setPrecision( 0 );
	vae->addEntry( vga );

	SetupMenuSelect *vlogscale = new SetupMenuSelect( "Log. Scale", false, 0, true, &log_scale );
	vlogscale->setHelp(PROGMEM"Use a logarithmic scale to the vario gauge");
	vlogscale->addEntry( "DISABLE" );
	vlogscale->addEntry( "ENABLE" );
	vae->addEntry( vlogscale, vga );

	SetupMenuSelect * vamod = new SetupMenuSelect( 	"Mode", false, 0 , true, &vario_mode );
	vamod->setHelp(PROGMEM"Controls if vario considers polar sink (=Netto), or not (=Brutto), or if Netto vario applies only in Cruise Mode");
	vamod->addEntry( "Brutto");
	vamod->addEntry( "Netto");
	vamod->addEntry( "Cruise-Netto");
	vae->addEntry( vamod, vlogscale);

	SetupMenuSelect * nemod = new SetupMenuSelect( 	"Netto Mode", false, 0 , true, &netto_mode );
	nemod->setHelp(PROGMEM"In 'Relative' mode, also circling sink is considered also called 'Super-Netto' to show climb rate as if you were circling there");
	nemod->addEntry( "Normal");
	nemod->addEntry( "Relative");
	vae->addEntry( nemod, vamod  );

	SetupMenuSelect * sink = new SetupMenuSelect( 	"Polar Sink", false, 0 , true, &ps_display );
	sink->setHelp(PROGMEM"Show polar sink rate together with TE in Vario bar when Vario is in Brutto Mode (else disabled)");
	sink->addEntry( "DISABLE");
	sink->addEntry( "ENABLE");
	vae->addEntry( sink, nemod );

	SetupMenuSelect * ncolor = new SetupMenuSelect( "Needle Color", false, 0 , true, &needle_color );
	ncolor->setHelp(PROGMEM"Choose the color of the vario needle");
	ncolor->addEntry( "White");
	ncolor->addEntry( "Orange");
	ncolor->addEntry( "Red");
	vae->addEntry( ncolor, sink );

	SetupMenuSelect * scrcaid = new SetupMenuSelect( "Center-Aid", true, 0, true, &screen_centeraid );
	scrcaid->addEntry( "Disable");
	scrcaid->addEntry( "Enable");
	vae->addEntry(scrcaid,ncolor);

	SetupMenu * vdamp = new SetupMenu( "Vario Damping" );
	vae->addEntry( vdamp, scrcaid );
	vdamp->addCreator( vario_menu_create_damping );

	SetupMenu * meanclimb = new SetupMenu( "Mean Climb" );
	meanclimb->setHelp(PROGMEM"Mean Climb or MC recommendation by green/red rhombus displayed in vario scale adjustment");
	vae->addEntry( meanclimb, vdamp);
	meanclimb->addCreator( vario_menu_create_meanclimb );

	SetupMenu * s2fs = new SetupMenu( "S2F Settings" );
	vae->addEntry( s2fs, meanclimb );
	s2fs->addCreator( vario_menu_create_s2f );

	SetupMenu * elco = new SetupMenu( "Electronic Compensation" );
	vae->addEntry( elco, s2fs );
	elco->addCreator( vario_menu_create_ec );
}

void SetupMenu::audio_menu_create_tonestyles( MenuEntry *top ){
	SetupMenuValFloat * cf = new SetupMenuValFloat( "CenterFreq", "Hz", 200.0, 2000.0, 10.0, 0, false, &center_freq );
	cf->setHelp(PROGMEM"Center frequency for Audio at zero Vario or zero S2F delta");
	top->addEntry( cf );

	SetupMenuValFloat * oc = new SetupMenuValFloat( "Octaves", "fold", 1.5, 4, 0.1, 0, false, &tone_var );
	oc->setHelp(PROGMEM"Maximum tone frequency variation");
	top->addEntry( oc );

	SetupMenuSelect * dt = new SetupMenuSelect( "Dual Tone", true, 0 , true, &dual_tone );
	dt->setHelp(PROGMEM"Select dual tone modue aka ilec sound, (di/da/di) or single tone (di/di/di) mode");
	dt->addEntry( "Disable");       // 0
	dt->addEntry( "Enable");        // 1
	top->addEntry( dt );

	SetupMenuValFloat * htv = new SetupMenuValFloat( "Dual Tone Pich", "%", 0, 50, 1.0, 0, false, &high_tone_var );
	htv->setHelp(PROGMEM"Tone variation in Dual Tone mode, percent of frequency pitch up for second tone");
	top->addEntry( htv );

	SetupMenuSelect * tch = new SetupMenuSelect( "Chopping", false, eval_chop, true, &chopping_mode );
	tch->setHelp(PROGMEM"Select tone chopping option on positive values for Vario and or S2F");
	tch->addEntry( "Disabled");             // 0
	tch->addEntry( "Vario only");           // 1
	tch->addEntry( "S2F only");             // 2
	tch->addEntry( "Vario and S2F");        // 3  default
	top->addEntry( tch );

	SetupMenuSelect * tchs = new SetupMenuSelect( "Chopping Style", false, 0 , true, &chopping_style );
	tchs->setHelp(PROGMEM"Select style of tone chopping either hard, or soft with fadein/fadeout");
	tchs->addEntry( "Soft");              // 0  default
	tchs->addEntry( "Hard");              // 1
	top->addEntry( tchs );

	SetupMenuSelect * advarto = new SetupMenuSelect( "Variable Tone", false, 0 , true, &audio_variable_frequency );
	advarto->setHelp(PROGMEM"Option to enable audio frequency updates within climbing tone intervals, disable keeps frequency constant");
	advarto->addEntry( "Disable");       // 0
	advarto->addEntry( "Enable");        // 1
	top->addEntry( advarto );
}

void SetupMenu::audio_menu_create_deadbands( MenuEntry *top ){
	SetupMenuValFloat * dbminlv = new SetupMenuValFloat( "Lower Vario", "", -5.0, 0, 0.1, 0 , false, &deadband_neg );
	dbminlv->setHelp(PROGMEM"Lower deadband limit (sink) for Audio mute function when in Vario mode");
	top->addEntry( dbminlv );

	SetupMenuValFloat * dbmaxlv = new SetupMenuValFloat( "Upper Vario", "", 0, 5.0, 0.1, 0 , false, &deadband );
	dbmaxlv->setHelp(PROGMEM"Upper deadband limit (climb) for Audio mute function when in Vario mode");
	top->addEntry( dbmaxlv );

	SetupMenuValFloat * dbmaxls2fn = new SetupMenuValFloat(	"Lower S2F", "", -25.0, 0, 1, 0 , false, &s2f_deadband_neg );
	dbmaxls2fn->setHelp(PROGMEM"Negative deadband limit in speed (too slow) deviation when in S2F mode");
	top->addEntry( dbmaxls2fn );

	SetupMenuValFloat * dbmaxls2f = new SetupMenuValFloat( "Upper S2F", "", 0, 25.0, 1, 0 , false, &s2f_deadband );
	dbmaxls2f->setHelp(PROGMEM"Positive deadband limit in speed (too high) deviation when in S2F mode");
	top->addEntry( dbmaxls2f );
}

void SetupMenu::audio_menu_create( MenuEntry *audio ){
	SetupMenuValFloat * dv = new SetupMenuValFloat( "Default Volume", "%", 0, 100, 1.0, 0, false, &default_volume );
	audio->addEntry( dv );
	dv->setHelp(PROGMEM"Default volume for Audio when device is switched on");

	SetupMenuValFloat * mv = new SetupMenuValFloat( "Max Volume", "%", 0, 100, 1.0, 0, false, &max_volume );
	audio->addEntry( mv );
	mv->setHelp(PROGMEM"Maximum volume for Audio when volume setting is at max. Set to 0% to mute audio entirely.");

	SetupMenuSelect * abnm = new SetupMenuSelect( "Cruise Audio", false, 0 , true, &cruise_audio_mode );
	abnm->setHelp(PROGMEM"Select either S2F command or Variometer (Netto/Brutto as selected) as audio source while cruising");
	abnm->addEntry( "Speed2Fly");       // 0
	abnm->addEntry( "Vario");           // 1
	audio->addEntry( abnm );

	SetupMenu * audios = new SetupMenu( "Tone Styles" );
	audio->addEntry( audios );
	audios->setHelp( PROGMEM "Configure audio style in terms of center frequency, octaves, single/dual tone, pitch and chopping", 220);
	audios->addCreator(audio_menu_create_tonestyles);

	SetupMenuSelect * ar = new SetupMenuSelect( "Range", false, 0 , true, &audio_range  );
	audio_range_sm = ar;
	update_rentrys(0);
	ar->setHelp(PROGMEM"Select either fixed or variable Audio range according to current Vario setting");
	audio->addEntry( ar );

	SetupMenu * db = new SetupMenu( "Deadbands" );
	audio->addEntry( db );
	db->setHelp(PROGMEM"Audio dead band limits within Audio remains silent in metric scale. 0,1 m/s equals roughly 20 ft/min or 0.2 knots");
	db->addCreator(audio_menu_create_deadbands);

	SetupMenuValFloat * afac = new SetupMenuValFloat( 	"Audio Exponent", "", 0.1, 2, 0.025, 0 , false, &audio_factor );
	afac->setHelp(PROGMEM"Exponential factor < 1 gives a logarithmic, and > 1 exponential characteristic for frequency of audio signal");
	audio->addEntry( afac);

	SetupMenuSelect * amps = new SetupMenuSelect( "Amplifier Off", false, 0 , true, &amplifier_shutdown );
	amps->setHelp(PROGMEM"Select if Amplifier is totally shutdown while in deadband (saves energy), or stays always on");
	amps->addEntry( "Always On");         // 0
	amps->addEntry( "Shutdown");          // 1
	audio->addEntry( amps );

	SetupMenuSelect * ameda = new SetupMenuSelect( "Audio in Setup", false, 0 , true, &audio_disable );
	ameda->setHelp(PROGMEM"Select if Audio will get muted while Setup Menu is open or stays always on");
	ameda->addEntry( "Stay On");      // 0
	ameda->addEntry( "Silent");       // 1
	audio->addEntry( ameda );

	SetupMenuValFloat * frqr = new SetupMenuValFloat( "Frequency Response", "%", -70.0, 70.0, 1.0, 0, false, &frequency_response );
	frqr->setHelp(PROGMEM"Setup frequency response, double frequency will be attenuated by the factor given, half frequency will be amplified");
	audio->addEntry( frqr );

	SetupMenuSelect * amspvol = new SetupMenuSelect( "Split Volume", false, 0 , true, &audio_split_vol );
	amspvol->setHelp(PROGMEM"Enable to control audio volume individually in SpeedToFly and in Vario mode, else there is one volume for both");
	amspvol->addEntry( "Disable");      // 0
	amspvol->addEntry( "Enable");       // 1
	audio->addEntry( amspvol );
}

void SetupMenu::glider_menu_create_polarpoints( MenuEntry *top ){
	SetupMenuValFloat * wil = new SetupMenuValFloat( "Ref Wingload", "kg/m2", 10.0, 100.0, 0.1, 0, false, &polar_wingload );
	wil->setHelp(PROGMEM"Wingload that corresponds to the 3 value pairs for speed/sink of polar");
	top->addEntry( wil );
	SetupMenuValFloat * pov1 = new SetupMenuValFloat( "Speed 1", "km/h", 50.0, 120.0, 1, 0, false, &polar_speed1);
	pov1->setHelp(PROGMEM"Speed 1, near to minimum sink from polar e.g. 80 km/h");
	top->addEntry( pov1 );
	SetupMenuValFloat * pos1 = new SetupMenuValFloat( "Sink  1", "m/s", -3.0, 0.0, 0.01, 0, false, &polar_sink1 );
	pos1->setHelp(PROGMEM"Sink indication at Speed 1 from polar");
	top->addEntry( pos1 );
	SetupMenuValFloat * pov2 = new SetupMenuValFloat( "Speed 2", "km/h", 70.0, 180.0, 1, 0, false, &polar_speed2 );
	pov2->setHelp(PROGMEM"Speed 2 for a moderate cruise from polar e.g. 120 km/h");
	top->addEntry( pov2 );
	SetupMenuValFloat * pos2 = new SetupMenuValFloat( "Sink  2",  "m/s", -5.0, 0.0, 0.01, 0, false, &polar_sink2 );
	pos2->setHelp(PROGMEM"Sink indication at Speed 2 from polar");
	top->addEntry( pos2 );
	SetupMenuValFloat * pov3 = new SetupMenuValFloat( "Speed 3", "km/h", 100.0, 250.0, 1, 0, false, &polar_speed3 );
	pov3->setHelp(PROGMEM"Speed 3 for a fast cruise from polar e.g. 170 km/h");
	top->addEntry( pov3 );
	SetupMenuValFloat * pos3 = new SetupMenuValFloat( "Sink  3", "m/s", -6.0, 0.0, 0.01, 0, false, &polar_sink3 );
	pos3->setHelp(PROGMEM"Sink indication at Speed 3 from polar");
	top->addEntry( pos3 );
}

void SetupMenu::glider_menu_create( MenuEntry *poe ){
	SetupMenuSelect * glt = new SetupMenuSelect( "Glider-Type",	false, polar_select, true, &glider_type );
	poe->addEntry( glt );
	for( int x=0; x< Polars::numPolars(); x++ ){
		glt->addEntry( Polars::getPolar(x).type );
	}
	poe->setHelp( PROGMEM"Weight and polar setup for best match with performance of glider", 220 );
	ESP_LOGI(FNAME, "Number of Polars installed: %d", Polars::numPolars() );

	SetupMenu * pa = new SetupMenu( "Polar Points" );
	pa->setHelp(PROGMEM "Adjust polar at 3 points of selected polar in commonly used metric system for polars", 230 );
	poe->addEntry( pa );
	pa->addCreator(glider_menu_create_polarpoints);

	SetupMenuValFloat * maxbal = new SetupMenuValFloat(	"Max Ballast", "liters", 0, 500, 1, 0, false, &polar_max_ballast );
	maxbal->setHelp(PROGMEM"Maximum water ballast for selected glider to allow sync from XCSoar using fraction of max ballast");
	poe->addEntry( maxbal );

	SetupMenuValFloat * wingarea = new SetupMenuValFloat( "Wing Area", "m2", 0, 50, 0.1, 0, false, &polar_wingarea );
	wingarea->setHelp(PROGMEM"Wingarea for the selected glider, to allow adjustments to support wing extensions or new types in square meters");
	poe->addEntry( wingarea );

	SetupMenuValFloat * fixball = new SetupMenuValFloat( "Empty Weight", "kg", 0, 1000, 1, empty_weight_adj, false, &empty_weight );
	fixball->setPrecision(0);
	fixball->setHelp(PROGMEM"Net rigged weight of the glider, according to the weight and balance plan");
	poe->addEntry( fixball );
}

void SetupMenu::options_menu_create_units( MenuEntry *top ){
	SetupMenuSelect * alu = new SetupMenuSelect( "Altimeter", false, 0, true, &alt_unit );
	alu->addEntry( "Meter (m)");
	alu->addEntry( "Foot  (ft)");
	alu->addEntry( "FL    (FL)");
	top->addEntry( alu );
	SetupMenuSelect * iau = new SetupMenuSelect( "Airspeed", false , 0, true, &ias_unit );
	iau->addEntry( "Kilom. (Km/h)");
	iau->addEntry( "Miles  (mph)");
	iau->addEntry( "Knots  (kt)");
	top->addEntry( iau );
	SetupMenuSelect * vau = new SetupMenuSelect( "Vario", false , update_rentrys, true, &vario_unit );
	vau->addEntry( "Meters/sec (m/s)");
	vau->addEntry( "100ft/min (cft/min)");
	vau->addEntry( "Knots     (knots)");
	top->addEntry( vau );
	SetupMenuSelect * teu = new SetupMenuSelect( "Temperature", false , 0, true, &temperature_unit );
	teu->addEntry( "Celcius");
	teu->addEntry( "Fahrenheit");
	teu->addEntry( "Kelvin");
	top->addEntry( teu );
	SetupMenuSelect * qnhi = new SetupMenuSelect( "QNH", false, 0, true, &qnh_unit );
	qnhi->addEntry( "Hectopascal");
	qnhi->addEntry( "InchMercury");
	top->addEntry( qnhi );
}

void SetupMenu::options_menu_create_flarm( MenuEntry *top ){
	SetupMenuSelect * flarml = new SetupMenuSelect( "Alarm Level",	false, 0, true, &flarm_warning );
	flarml->setHelp( PROGMEM "Enable FLARM Alarm level 1 is lowest with 13-18 sec, 2 medium 9-12 sec and 3 highest with 0-8 sec until impact");
	flarml->addEntry( "Disable");
	flarml->addEntry( "Level 1");
	flarml->addEntry( "Level 2");
	flarml->addEntry( "Level 3");
	top->addEntry( flarml );

	SetupMenuValFloat * flarmv = new SetupMenuValFloat("Alarm Volume",  "%", 20, 125, 1, 0, false, &flarm_volume  );
	flarmv->setHelp(PROGMEM "Maximum volume FLARM alarm audio warning");
	top->addEntry( flarmv );

	SetupMenuSelect * flarms = new SetupMenuSelect( "FLARM Simulation",	false, 0, true, &flarm_sim, false, true );
	flarms->setHelp( PROGMEM "Simulate an airplane crossing from left to right with different alarm levels and vertical distance 5 seconds after pressed (leave setup!)");
	flarms->addEntry( "Disable");
	flarms->addEntry( "Start Sim");
	top->addEntry( flarms );
}

void SetupMenu::options_menu_create_compasswind_compass_dev( MenuEntry *top ){
	const char *skydirs[8] = { "0°", "45°", "90°", "135°", "180°", "225°", "270°", "315°" };
	for( int i = 0; i < 8; i++ )
	{
		SetupMenuSelect* sms = new SetupMenuSelect( "Direction", false, compassDeviationAction, false, 0 );
		sms->setHelp( "Push button to start deviation action" );
		sms->addEntry( skydirs[i] );
		top->addEntry( sms );
	}
}

void SetupMenu::options_menu_create_compasswind_compass_nmea( MenuEntry *top ){
	SetupMenuSelect * nmeaHdm = new SetupMenuSelect( "Magnetic Heading", false, 0, true, &compass_nmea_hdm );
	nmeaHdm->addEntry( "Disable");
	nmeaHdm->addEntry( "Enable");
	nmeaHdm->setHelp( PROGMEM "Enable/disable NMEA '$HCHDM' sentence generation for magnetic heading" );
	top->addEntry( nmeaHdm );

	SetupMenuSelect * nmeaHdt = new SetupMenuSelect( "True Heading", false, 0, true, &compass_nmea_hdt );
	nmeaHdt->addEntry( "Disable");
	nmeaHdt->addEntry( "Enable");
	nmeaHdt->setHelp( PROGMEM "Enable/disable NMEA '$HCHDT' sentence generation for true heading" );
	top->addEntry( nmeaHdt );
}

void SetupMenu::options_menu_create_compasswind_compass( MenuEntry *top ){
	SetupMenuSelect * compSensor = new SetupMenuSelect( "Sensor Option", true, compass_ena, true, &compass_enable);
	compSensor->addEntry( "Disable");
	compSensor->addEntry( "Enable I2C sensor");
	compSensor->addEntry( "Disable");
	compSensor->addEntry( "Enable CAN sensor");

	compSensor->setHelp( PROGMEM "Option to enable/disable the Compass Sensor" );
	top->addEntry( compSensor );

	SetupMenuSelect * compSensorCal = new SetupMenuSelect( "Sensor Calibration", false, compassSensorCalibrateAction, false );
	compSensorCal->addEntry( "Cancel");
	compSensorCal->addEntry( "Start");
	compSensorCal->addEntry( "Show");
	compSensorCal->addEntry( "Show Raw Data");
	compSensorCal->setHelp( PROGMEM "Calibrate Magnetic Sensor, mandatory for operation" );
	top->addEntry( compSensorCal );

	SetupMenuValFloat *cd = new SetupMenuValFloat( "Setup Declination",	"°",	-180, 180, 1.0, compassDeclinationAction, false, &compass_declination );
	cd->setHelp( PROGMEM "Set compass declination in degrees" );
	top->addEntry( cd );

	SetupMenuSelect * devMenuA = new SetupMenuSelect( "AutoDeviation", false, 0, true, &compass_dev_auto );
	devMenuA->setHelp( "Automatic adaptive deviation and precise airspeed evaluation method using data from circling wind");
	devMenuA->addEntry( "Disable");
	devMenuA->addEntry( "Enable");
	top->addEntry( devMenuA );

	SetupMenu * devMenu = new SetupMenu( "Setup Deviations" );
	devMenu->setHelp( "Compass Deviations", 280 );
	top->addEntry( devMenu );
	devMenu->addCreator( options_menu_create_compasswind_compass_dev );

	// Show comapss deviations
	DisplayDeviations* smd = new DisplayDeviations( "Show Deviations" );
	top->addEntry( smd );

	SetupMenuSelect* sms = new SetupMenuSelect( "Reset Deviations ", false, compassResetDeviationAction, false,	0 );
	sms->setHelp( "Reset all deviation data to zero" );
	sms->addEntry( "Cancel" );
	sms->addEntry( "Reset" );
	top->addEntry( sms );

	SetupMenu * nmeaMenu = new SetupMenu( "Setup NMEA" );
	top->addEntry( nmeaMenu );
	nmeaMenu->addCreator( options_menu_create_compasswind_compass_nmea );

	SetupMenuValFloat * compdamp = new SetupMenuValFloat( "Damping", "sec", 0.1, 10.0, 0.1, 0, false, &compass_damping );
	compdamp->setPrecision(1);
	top->addEntry( compdamp );
	compdamp->setHelp(PROGMEM "Compass or magnetic heading damping factor in seconds");

	SetupMenuValFloat * compi2c = new SetupMenuValFloat( "I2C Clock", "KHz", 10.0, 400.0, 10, 0, false, &compass_i2c_cl, true );
	top->addEntry( compi2c );
	compi2c->setHelp(PROGMEM "Setup compass I2C Bus clock in KHz");

	// Show compass settings
	ShowCompassSettings* scs = new ShowCompassSettings( "Show Settings" );
	top->addEntry( scs );
}

void SetupMenu::options_menu_create_compasswind_straightwind_filters( MenuEntry *top ){
	SetupMenuValFloat *smgsm = new SetupMenuValFloat( "Airspeed Lowpass", "", 0, 0.05, 0.001, nullptr, false, &wind_as_filter );
	smgsm->setPrecision(3);
	top->addEntry( smgsm );
	smgsm->setHelp(PROGMEM "Lowpass factor for airspeed correction in airspeed estimation from AS/Compass and GPS tracks");

	SetupMenuValFloat *devlp = new SetupMenuValFloat( "Deviation Lowpass", "", 0, 0.05, 0.001, nullptr, false, &wind_dev_filter );
	devlp->setPrecision(3);
	top->addEntry( devlp );
	devlp->setHelp(PROGMEM "Lowpass factor for deviation table correction from AS/Compass and GPS tracks estimation");

	SetupMenuValFloat *smgps = new SetupMenuValFloat( "GPS Lowpass", "sec", 0.1, 10.0, 0.1, nullptr, false, &wind_gps_lowpass );
	smgps->setPrecision(1);
	top->addEntry( smgps );
	smgps->setHelp(PROGMEM "Lowpass factor for GPS track and speed, to correlate with Compass latency");

	SetupMenuValFloat *wlpf = new SetupMenuValFloat( "Averager", "", 5, 120, 1, nullptr, false, &wind_filter_lowpass );
	wlpf->setPrecision(0);
	top->addEntry( wlpf );
	wlpf->setHelp(PROGMEM "Number of measurements used for straight flight live wind averager");
}

void SetupMenu::options_menu_create_compasswind_straightwind_limits( MenuEntry *top ){
	SetupMenuValFloat *smdev = new SetupMenuValFloat( "Deviation Limit", "°", 0.0, 180.0, 1.0,	nullptr, false, &wind_max_deviation );
	smdev->setHelp( PROGMEM "Maximum deviation accepted derived from AS/Compass and GPS tracks" );
	top->addEntry( smdev );

	SetupMenuValFloat *smslip = new SetupMenuValFloat( "Sideslip Limit", "°", 0, 45.0, 0.1, nullptr, false, &swind_sideslip_lim );
	smslip->setPrecision(1);
	top->addEntry( smslip );
	smslip->setHelp(PROGMEM "Maximum side slip in ° from side slip estimator accepted straight wind calculation");

	SetupMenuValFloat *smcourse = new SetupMenuValFloat( "Course Limit", "°", 2.0, 30.0, 0.1, nullptr, false, &wind_straight_course_tolerance );
	smcourse->setPrecision(1);
	top->addEntry( smcourse );
	smcourse->setHelp(PROGMEM "Maximum delta angle in ° per second during straight flight accepted for straight wind calculation");

	SetupMenuValFloat *aslim = new SetupMenuValFloat( "AS Delta Limit", "km/h", 1.0, 30.0, 1, nullptr, false, &wind_straight_speed_tolerance );
	aslim->setPrecision(0);
	top->addEntry( aslim );
	aslim->setHelp(PROGMEM "Maximum delta in airspeed estimation from wind and GPS during straight flight accpeted for straight wind calculation");
}

void SetupMenu::options_menu_create_compasswind_straightwind( MenuEntry *top ){
	SetupMenu * strWindFM = new SetupMenu( "Filters" );
	top->addEntry( strWindFM );
	strWindFM->addCreator( options_menu_create_compasswind_straightwind_filters );
	SetupMenu * strWindLM = new SetupMenu( "Limits" );
	top->addEntry( strWindLM );
	strWindLM->addCreator( options_menu_create_compasswind_straightwind_limits );
	ShowStraightWind* ssw = new ShowStraightWind( "Straight Wind Status" );
	top->addEntry( ssw );
}

void SetupMenu::options_menu_create_compasswind_circlingwind( MenuEntry *top ){
	// Show Circling Wind Status
	ShowCirclingWind* scw = new ShowCirclingWind( "Circling Wind Status" );
	top->addEntry( scw );

	SetupMenuValFloat *cirwd = new SetupMenuValFloat( "Max Delta", "°", 0, 90.0, 1.0, nullptr, false, &max_circle_wind_diff );
	top->addEntry( cirwd );
	cirwd->setHelp(PROGMEM "Maximum accepted delta accepted value for heading error in circling wind calculation");

	SetupMenuValFloat *cirlp = new SetupMenuValFloat( "Averager", "", 1, 10, 1, nullptr, false, &circle_wind_lowpass );
	cirlp->setPrecision(0);
	top->addEntry( cirlp );
	cirlp->setHelp(PROGMEM "Number of circles used for circling wind averager. A value of 1 means no average");

	SetupMenuValFloat *cirwsd = new SetupMenuValFloat( "Max Speed Delta", "km/h", 0.0, 20.0, 0.1, nullptr, false, &max_circle_wind_delta_speed );
	top->addEntry( cirwsd );
	cirwsd->setPrecision(1);
	cirwsd->setHelp(PROGMEM "Maximum accepted wind speed delta from last measurement accepted value for wind report to straight wind calculation");

	SetupMenuValFloat *cirwdd = new SetupMenuValFloat( "Max Dir Delta", "°", 0.0, 60.0, 0.1, nullptr, false, &max_circle_wind_delta_deg );
	top->addEntry( cirwdd );
	cirwdd->setPrecision(1);
	cirwdd->setHelp(PROGMEM "Maximum accepted wind direction delta from last measurement accepted value for wind report to straight wind calculation");
}

void SetupMenu::options_menu_create_compasswind( MenuEntry *top ){
	SetupMenu * compassMenu = new SetupMenu( "Compass" );
	top->addEntry( compassMenu );
	compassMenu->addCreator( options_menu_create_compasswind_compass );

	// Wind speed observation window
	SetupMenuSelect * windcal = new SetupMenuSelect( "Wind Calculation", false, 0, true, &wind_enable );
	windcal->addEntry( "Disable");
	windcal->addEntry( "Straight");
	windcal->addEntry( "Circling");
	windcal->addEntry( "Both");
	windcal->setHelp(PROGMEM "Enable Wind calculation for straight flight (needs compass), circling or both and display wind in reto display style");
	top->addEntry( windcal );

	// Display option
	SetupMenuSelect * winddis = new SetupMenuSelect( "Display", false, 0, true, &wind_display );
	winddis->addEntry( "Disable");
	winddis->addEntry( "Wind Digits");
	winddis->addEntry( "Wind Arrow");
	winddis->addEntry( "Wind Both");
	winddis->addEntry( "Compass");
	winddis->setHelp( PROGMEM "Control what is to be displayed, either as digits or by arrow or both on retro style screen");
	top->addEntry( winddis );

	// Wind speed observation window
	SetupMenuSelect * windref = new SetupMenuSelect( "Arrow Ref", false, 0, true, &wind_reference );
	windref->addEntry( "North");
	windref->addEntry( "Mag Heading");
	windref->addEntry( "GPS Course");
	windref->setHelp( PROGMEM "Wind arrow related either to geographic north or related to true airplane heading");
	top->addEntry( windref );

	SetupMenu * strWindM = new SetupMenu( "Straight Wind" );
	top->addEntry( strWindM );
	strWindM->setHelp( PROGMEM "Straight flight wind calculation needs compass module active", 250 );
	strWindM->addCreator( options_menu_create_compasswind_straightwind );

	SetupMenu * cirWindM = new SetupMenu( "Circling Wind" );
	top->addEntry( cirWindM );
	cirWindM->addCreator( options_menu_create_compasswind_circlingwind );

	SetupMenuSelect * windlog = new SetupMenuSelect( "Wind Logging", false, 0, true, &wind_logging );
	windlog->addEntry( "Disable");
	windlog->addEntry( "Enable WIND");
	windlog->addEntry( "Enable GYRO/MAG");
	windlog->addEntry( "Enable Both");
	windlog->setHelp(PROGMEM "Enable Wind logging NMEA output to WIFI port 8882");
	top->addEntry( windlog );
}

void SetupMenu::options_menu_create_wireless_routing( MenuEntry *top ){
	SetupMenuSelect * wloutxcv = new SetupMenuSelect( PROGMEM "XCVario", false, 0, true, &rt_xcv_wl );
	wloutxcv->addEntry( PROGMEM "Disable");
	wloutxcv->addEntry( PROGMEM "Enable");
	top->addEntry( wloutxcv );
	SetupMenuSelect * wloutxs1 = new SetupMenuSelect( PROGMEM "S1-RS232", false, update_routing_s1, true, &rt_s1_wl );
	wloutxs1->addEntry( PROGMEM "Disable");
	wloutxs1->addEntry( PROGMEM "Enable");
	top->addEntry( wloutxs1 );
	SetupMenuSelect * wloutxs2 = new SetupMenuSelect( PROGMEM "S2-RS233", false, update_routing_s2, true, &rt_s2_wl );
	wloutxs2->addEntry( PROGMEM "Disable");
	wloutxs2->addEntry( PROGMEM "Enable");
	top->addEntry( wloutxs2 );
	SetupMenuSelect * wloutxcan = new SetupMenuSelect( PROGMEM "CAN-bus", false, 0, true, &rt_wl_can );
	wloutxcan->addEntry( PROGMEM "Disable");
	wloutxcan->addEntry( PROGMEM "Enable");
	top->addEntry( wloutxcan );
}


void SetupMenu::options_menu_create_wireless_custom_id( MenuEntry *top ){
	SetupMenuChar * c1 = new SetupMenuChar( "Letter 1",	false, update_id, false, custom_wireless_id.get().id, 0 );
	SetupMenuChar * c2 = new SetupMenuChar( "Letter 2",	false, update_id, false, custom_wireless_id.get().id, 1 );
	SetupMenuChar * c3 = new SetupMenuChar( "Letter 3",	false, update_id, false, custom_wireless_id.get().id, 2 );
	SetupMenuChar * c4 = new SetupMenuChar( "Letter 4",	false, update_id, false, custom_wireless_id.get().id, 3 );
	SetupMenuChar * c5 = new SetupMenuChar( "Letter 5",	false, update_id, false, custom_wireless_id.get().id, 4 );
	SetupMenuChar * c6 = new SetupMenuChar( "Letter 6",	false, update_id, false, custom_wireless_id.get().id, 5 );
	top->addEntry( c1 );
	top->addEntry( c2 );
	top->addEntry( c3 );
	top->addEntry( c4 );
	top->addEntry( c5 );
	top->addEntry( c6 );
	static const char keys[][4] { "\0","0","1","2","3","4","5","6","7","8","9","-","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};
	c1->addEntryList( keys, sizeof(keys)/4 );
	c2->addEntryList( keys, sizeof(keys)/4 );
	c3->addEntryList( keys, sizeof(keys)/4 );
	c4->addEntryList( keys, sizeof(keys)/4 );
	c5->addEntryList( keys, sizeof(keys)/4 );
	c6->addEntryList( keys, sizeof(keys)/4 );
}

void SetupMenu::options_menu_create_wireless( MenuEntry *top )
{
	SetupMenuSelect * btm = new SetupMenuSelect( PROGMEM "Wireless", true, 0, true, &wireless_type );
	btm->setHelp( PROGMEM "Activate wireless interface type to connect navigation devices, or to another XCVario as client");
	btm->addEntry( "Disable");
	btm->addEntry( "Bluetooth");
	btm->addEntry( "Wireless Master");
	btm->addEntry( "Wireless Client");
	btm->addEntry( "Wireless Standalone");
	top->addEntry( btm );

	SetupMenu * wlrt = new SetupMenu( PROGMEM "WL Routing" );
	top->addEntry( wlrt );
	wlrt->setHelp( PROGMEM "Select data source that is routed from/to Wireless BT or WIFI interface");
	wlrt->addCreator( options_menu_create_wireless_routing );

	SetupMenuValFloat *wifip = new SetupMenuValFloat( PROGMEM "WIFI Power", "%", 10.0, 100.0, 5.0, update_wifi_power, false, &wifi_max_power );
	wifip->setPrecision(0);
	top->addEntry( wifip );
	wifip->setHelp(PROGMEM "Maximum Wifi Power to be used 10..100% or 2..20dBm");

	SetupMenuSelect * wifimal = new SetupMenuSelect( PROGMEM "Lock Master", false, master_xcv_lock, true, &master_xcvario_lock );
	wifimal->setHelp( PROGMEM "In wireless client role, lock the scanned master XCVario ID above to this client");
	wifimal->addEntry( "Unlock");
	wifimal->addEntry( "Lock");
	top->addEntry( wifimal );

	SetupMenuSelect * datamon = new SetupMenuSelect( PROGMEM "Monitor", false, data_mon, true, &data_monitor );
	datamon->setHelp( PROGMEM "Short press button to start/pause, long press to terminate data monitor", 260);
	datamon->addEntry( "Disable");
	datamon->addEntry( "Bluetooth");
	datamon->addEntry( "Wifi 8880");
	datamon->addEntry( "Wifi 8881");
	datamon->addEntry( "Wifi 8882");
	datamon->addEntry( "RS232 S1");
	datamon->addEntry( "RS232 S2");
	datamon->addEntry( "CAN Bus");
	top->addEntry( datamon );

	SetupMenuSelect * datamonmod = new SetupMenuSelect( PROGMEM "Monitor Mode", false, data_mon, true, &data_monitor_mode );
	datamonmod->setHelp( PROGMEM "Display data either in ASCII or do a Binary hexdump");
	datamonmod->addEntry( "ASCII");
	datamonmod->addEntry( "Binary");
	top->addEntry( datamonmod );

	SetupMenu * cusid = new SetupMenu( PROGMEM "Custom-ID" );
	top->addEntry( cusid );
	cusid->setHelp( PROGMEM "Select custom ID (SSID) for wireless BT (or WIFI) interface, e.g. D-1234. Restart device to activate", 215);
	cusid->addCreator( options_menu_create_wireless_custom_id );
}

void SetupMenu::options_menu_create_gload( MenuEntry *top ){
	SetupMenuSelect * glmod = new SetupMenuSelect( "Activation Mode", false, 0, true, &gload_mode );
	glmod->setHelp( PROGMEM "Switch off G-Force screen, or activate G-Force screen by threshold 'Dynamic', or static by 'Always-On'");
	glmod->addEntry( "Off");
	glmod->addEntry( "Dynamic");
	glmod->addEntry( "Always-On");
	top->addEntry( glmod );

	SetupMenuValFloat * gtpos = new SetupMenuValFloat( "Positive Threshold", "", 1.0, 8.0, 0.1, 0, false, &gload_pos_thresh );
	top->addEntry( gtpos );
	gtpos->setPrecision( 1 );
	gtpos->setHelp(PROGMEM "Positive threshold to launch G-Load display");

	SetupMenuValFloat * gtneg = new SetupMenuValFloat( "Negative Threshold", "", -8.0, 1.0, 0.1, 0, false, &gload_neg_thresh );
	top->addEntry( gtneg );
	gtneg->setPrecision( 1 );
	gtneg->setHelp(PROGMEM "Negative threshold to launch G-Load display");

	SetupMenuValFloat * glpos = new SetupMenuValFloat( "Red positive limit", "", 1.0, 8.0, 0.1, 0, false, &gload_pos_limit );
	top->addEntry( glpos );
	glpos->setPrecision( 1 );
	glpos->setHelp(PROGMEM "Positive g load factor limit the airplane is able to handle according to manual below manoevering speed");

	SetupMenuValFloat * glposl = new SetupMenuValFloat( "Yellow pos. Limit", "", 1.0, 8.0, 0.1, 0, false, &gload_pos_limit_low );
	top->addEntry( glposl );
	glposl->setPrecision( 1 );
	glposl->setHelp(PROGMEM "Positive g load factor limit the structure of airplane is able to handle, see manual, above manoevering speed");

	SetupMenuValFloat * glneg = new SetupMenuValFloat( "Red negative limit", "", -8.0, 1.0, 0.1, 0, false, &gload_neg_limit );
	top->addEntry( glneg );
	glneg->setPrecision( 1 );
	glneg->setHelp(PROGMEM "Negative g load factor limit the airplane is able to handle according to manual below manoevering speed");

	SetupMenuValFloat * glnegl = new SetupMenuValFloat( "Yellow neg. Limit", "", -8.0, 1.0, 0.1, 0, false, &gload_neg_limit_low );
	top->addEntry( glnegl );
	glnegl->setPrecision( 1 );
	glnegl->setHelp(PROGMEM "Negative g load factor limit the structure of airplane is able to handle, see manual, below manoevering speed");


	SetupMenuValFloat * gmpos = new SetupMenuValFloat( "Max Positive", "", 0.0, 0.0, 0.0, 0, false, &gload_pos_max );
	top->addEntry( gmpos );
	gmpos->setPrecision( 1 );
	gmpos->setHelp(PROGMEM "Maximum positive G-Load measured since last reset");

	SetupMenuValFloat * gmneg = new SetupMenuValFloat( "Max Negative", "", 0.0, 0.0, 0.0, 0, false, &gload_neg_max );
	top->addEntry( gmneg );
	gmneg->setPrecision( 1 );
	gmneg->setHelp(PROGMEM "Maximum negative G-Load measured since last reset");

	SetupMenuSelect * gloadres = new SetupMenuSelect( "G-Load reset", false, gload_reset, false, 0 );
	gloadres->setHelp(PROGMEM "Option to reset stored maximum positive and negative G-load values");
	gloadres->addEntry( "Reset");
	gloadres->addEntry( "Cancel");
	top->addEntry( gloadres );
}

void SetupMenu::options_menu_create( MenuEntry *opt ){
	if( student_mode.get() == 0 ) {
		SetupMenuSelect *stumo  = new SetupMenuSelect( "Student Mode", true, 0, true, &student_mode );
		opt->addEntry( stumo );
		stumo->setHelp( PROGMEM "Student mode, disables all sophisticated setup to just basic pre-flight related items like MC, ballast or bugs");
		stumo->addEntry( "Disable");
		stumo->addEntry( "Enable");
	}
	Flap::setupMenue( opt );
	// Units
	SetupMenu * un = new SetupMenu( "Units" );
	opt->addEntry( un );
	un->setHelp( PROGMEM "Setup altimeter, airspeed indicator and variometer with European Metric, American, British or Australian units", 200);
	un->addCreator(options_menu_create_units);

	SetupMenuSelect * amode = new SetupMenuSelect( "Airspeed Mode",	false, 0, true, &airspeed_mode );
	opt->addEntry( amode );
	amode->setHelp( PROGMEM "Select mode of Airspeed indicator to display IAS (Indicated AirSpeed), TAS (True AirSpeed) or CAS (calibrated airspeed)", 180 );
	amode->addEntry( "IAS");
	amode->addEntry( "TAS");
	amode->addEntry( "CAS");
	amode->addEntry( "Slip Angle");

	SetupMenuSelect * atl = new SetupMenuSelect( "Auto Transition",	false, 0, true, &fl_auto_transition );
	opt->addEntry( atl );
	atl->setHelp( PROGMEM "Option to enable automatic altitude transition to QNH Standard (1013.25) above 'Transition Altitude'");
	atl->addEntry( "Disable");
	atl->addEntry( "Enable");

	SetupMenuSelect * altDisplayMode = new SetupMenuSelect( "Altitude Mode", false, 0, true, &alt_display_mode );
	opt->addEntry( altDisplayMode );
	altDisplayMode->setHelp( PROGMEM "Select altitude display mode");
	altDisplayMode->addEntry( "QNH");
	altDisplayMode->addEntry( "QFE");

	SetupMenuValFloat * tral = new SetupMenuValFloat( "Transition Altitude", "FL", 0, 400, 10, 0, false, &transition_alt  );
	tral->setHelp(PROGMEM"Transition altitude (or transition height, when using QFE) is the altitude/height above which standard pressure (QNE) is set (1013.2 mb/hPa)", 100 );
	opt->addEntry( tral );

	SetupMenu * flarm = new SetupMenu( "FLARM" );
	opt->addEntry( flarm );
	flarm->setHelp( PROGMEM "Option to display FLARM Warnings depending on FLARM alarm level");
	flarm->addCreator(options_menu_create_flarm);

	SetupMenu * compassWindMenu = new SetupMenu( "Compass/Wind" );
	opt->addEntry( compassWindMenu );
	compassWindMenu->setHelp( PROGMEM "Setup Compass and Wind", 280 );
	compassWindMenu->addCreator(options_menu_create_compasswind);

	SetupMenu * wireless = new SetupMenu( PROGMEM "Wireless" );
	opt->addEntry( wireless );
	wireless->addCreator(options_menu_create_wireless);

	SetupMenu * gload = new SetupMenu( "G-Load Display" );
	opt->addEntry( gload );
	gload->addCreator(options_menu_create_gload);
}

void SetupMenu::system_menu_create_software( MenuEntry *top ){
	Version V;
	SetupMenuSelect * ver = new SetupMenuSelect( "Software Vers.", false, 0, false );
	ver->addEntry( V.version() );
	top->addEntry( ver );

	SetupMenuSelect * upd = new SetupMenuSelect( "Software Update", true, 0, true, &software_update );
	upd->setHelp(PROGMEM "Software Update over the air (OTA). Start Wifi AP, then connect to Wifi 'ESP32 OTA' and open http://192.168.4.1 to upload firmware");
	upd->addEntry( "Cancel");
	upd->addEntry( "Start");
	top->addEntry( upd );
}

void SetupMenu::system_menu_create_battery( MenuEntry *top ){
	SetupMenuValFloat * blow = new SetupMenuValFloat( "Battery Low", "Volt ", 0.0, 28.0, 0.1, 0, false, &bat_low_volt );
	SetupMenuValFloat * bred = new SetupMenuValFloat( "Battery Red", "Volt ", 0.0, 28.0, 0.1, 0, false, &bat_red_volt  );
	SetupMenuValFloat * byellow = new SetupMenuValFloat( "Battery Yellow", "Volt ", 0.0, 28.0, 0.1, 0, false, &bat_yellow_volt );
	SetupMenuValFloat * bfull = new SetupMenuValFloat( "Battery Full", "Volt ", 0.0, 28.0, 0.1, 0, false, &bat_full_volt  );

	SetupMenuSelect * batv = new SetupMenuSelect( "Battery Display", false, 0, true, &battery_display  );
	batv->setHelp(PROGMEM "Option to display battery charge state either in Percentage e.g. 75% or Voltage e.g. 12.5V");
	batv->addEntry( "Percentage");
	batv->addEntry( "Voltage");
	batv->addEntry( "Voltage Big");

	top->addEntry(blow);
	top->addEntry(bred);
	top->addEntry(byellow);
	top->addEntry(bfull);
	top->addEntry( batv );
}

void SetupMenu::system_menu_create_hardware_type( MenuEntry *top ){
	// UNIVERSAL, RAYSTAR_RFJ240L_40P, ST7789_2INCH_12P, ILI9341_TFT_18P
	SetupMenuSelect * dtype = new SetupMenuSelect( 	"HW Type", false, 0, true, &display_type );
	dtype->setHelp( PROGMEM "Factory setup for corresponding display type used");
	dtype->addEntry( "UNIVERSAL");
	dtype->addEntry( "RAYSTAR");
	dtype->addEntry( "ST7789");
	dtype->addEntry( "ILI9341");
	top->addEntry( dtype );

	SetupMenuSelect * disty = new SetupMenuSelect( "Style", false , 0, false, &display_style );
	top->addEntry( disty );
	disty->setHelp( PROGMEM "Display style in more digital airliner stype or retro mode with classic vario meter needle");
	disty->addEntry( "Airliner");
	disty->addEntry( "Retro");
	disty->addEntry( "UL");

	SetupMenuSelect * disva = new SetupMenuSelect( "Color Variant", false , 0, false, &display_variant );
	top->addEntry( disva );
	disva->setHelp( PROGMEM "Display variant white on black (W/B) or black on white(B/W)");
	disva->addEntry( "W/B");
	disva->addEntry( "B/W");

	// Orientation   _display_orientation
	SetupMenuSelect * diso = new SetupMenuSelect( "Orientation", true, 0, true, &display_orientation );
	top->addEntry( diso );
	diso->setHelp( PROGMEM "Display Orientation either NORMAL means Rotary is right, or TOPDOWN means Rotary is left");
	diso->addEntry( "NORMAL");
	diso->addEntry( "TOPDOWN");

	//
	SetupMenuSelect * drawp = new SetupMenuSelect( "Needle Alignment", false, 0, true, &drawing_prio );
	top->addEntry( drawp );
	drawp->setHelp( PROGMEM "Alignment of the variometer needle either in front of the infos displayed, or in background");
	drawp->addEntry( "Front");
	drawp->addEntry( "Back");

}


void SetupMenu::system_menu_create_hardware_rotary_screens( MenuEntry *top ){
	SetupMenuSelect * scrgmet = new SetupMenuSelect( "G-Meter", false, upd_screens, true, &screen_gmeter );
	scrgmet->addEntry( "Disable");
	scrgmet->addEntry( "Enable");
	top->addEntry(scrgmet);
}

void SetupMenu::system_menu_create_hardware_rotary( MenuEntry *top ){
	SetupMenu * screens = new SetupMenu( "Screens");
	top->addEntry( screens );
	screens->setHelp(PROGMEM "Select screens to be activated one after each other by short press");
	screens->addCreator( system_menu_create_hardware_rotary_screens );

	SetupMenuSelect * rotype;
	if( hardwareRevision.get() < XCVARIO_21 )
		rotype = new SetupMenuSelect( "Direction", false , 0, false, &rotary_dir );
	else
		rotype = new SetupMenuSelect( "Direction", false , 0, false, &rotary_dir_21 );
	top->addEntry( rotype );
	rotype->setHelp( PROGMEM "Select type of rotary switch, different brands may need adjustment");
	rotype->addEntry( "Clockwise");
	rotype->addEntry( "Counterclockwise");

	SetupMenuSelect * roinc = new SetupMenuSelect( "Sensitivity", false , 0, false, &rotary_inc );
	top->addEntry( roinc );
	roinc->setHelp( PROGMEM "Select rotary sensitivity in number of Indent's for one increment, to your personal preference, 1 Indent is most sensitive");
	roinc->addEntry( "1 Indent");
	roinc->addEntry( "2 Indent");
	roinc->addEntry( "3 Indent");
	roinc->addEntry( "4 Indent");

	// Rotary Default
	SetupMenuSelect * rd = new SetupMenuSelect( "Rotation", true, 0, true, &rot_default );
	top->addEntry( rd );
	rd->setHelp(PROGMEM "Select value to be altered at rotary movement outside of setup menu");
	rd->addEntry( "Volume");
	rd->addEntry( "MC Value");

	SetupMenuSelect * sact = new SetupMenuSelect( "Setup Menu by", false, 0, true, &menu_long_press );
	top->addEntry( sact);
	sact->setHelp(PROGMEM "Select Mode to activate setup menu either by short press or long press > 0.4 seconds");
	sact->addEntry( "Short Press");
	sact->addEntry( "Long Press");
}


static const char PROGMEM lkeys[][4] { "0","1","2","3","4","5","6","7","8","9",":",";","<","=",">","?","@","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};

void SetupMenu::system_menu_create_hardware_ahrs_lc( MenuEntry *top ){
	SetupMenuSelect * ahrslc1 = new SetupMenuSelect( "First    Letter",	false, add_key, false, &ahrs_licence_dig1 );
	SetupMenuSelect * ahrslc2 = new SetupMenuSelect( "Second Letter",	false, add_key, false, &ahrs_licence_dig2 );
	SetupMenuSelect * ahrslc3 = new SetupMenuSelect( "Third   Letter",	false, add_key, false, &ahrs_licence_dig3 );
	SetupMenuSelect * ahrslc4 = new SetupMenuSelect( "Last     Letter",	false, add_key, false, &ahrs_licence_dig4 );
	top->addEntry( ahrslc1 );
	top->addEntry( ahrslc2 );
	top->addEntry( ahrslc3 );
	top->addEntry( ahrslc4 );
	ahrslc1->addEntryList( lkeys, sizeof(lkeys)/4 );
	ahrslc2->addEntryList( lkeys, sizeof(lkeys)/4 );
	ahrslc3->addEntryList( lkeys, sizeof(lkeys)/4 );
	ahrslc4->addEntryList( lkeys, sizeof(lkeys)/4 );
}



void SetupMenu::system_menu_create_hardware_ahrs( MenuEntry *top ){
	SetupMenuSelect * ahrsid = new SetupMenuSelect( "AHRS ID", false, 0, false );
	ahrsid->addEntry( Cipher::id() );
	top->addEntry( ahrsid );

	mpu = new SetupMenuSelect( "AHRS Option", true , 0, true, &attitude_indicator );
	top->addEntry( mpu );
	mpu->setHelp( PROGMEM "Enable High Accuracy Attitude Sensor (AHRS) NMEA messages (need valid license key entered)");
	mpu->addEntry( "Disable");
	if( gflags.ahrsKeyValid )
		mpu->addEntry( "Enable");

	SetupMenuSelect * ahrsaz = new SetupMenuSelect( "AHRS Autozero", true , 0, true, &ahrs_autozero );
	top->addEntry( ahrsaz );
	ahrsaz->setHelp( PROGMEM "Start Autozero of AHRS Sensor; Preconditions: On ground; Wings 100% horizontal, fuselage in flight position !");
	ahrsaz->addEntry( "Cancel");
	ahrsaz->addEntry( "Start");

	SetupMenu * ahrslc = new SetupMenu( "AHRS License Key" );
	ahrslc->setHelp( PROGMEM "Enter valid AHRS License Key, then with valid key under 'AHRS Option', AHRS feature can be enabled");
	top->addEntry( ahrslc );
	ahrslc->addCreator( system_menu_create_hardware_ahrs_lc );

	SetupMenuValFloat * ahrsgf = new SetupMenuValFloat( "AHRS Gyro", "%", 0, 100, 0.1, 0, false, &ahrs_gyro_factor  );
	ahrsgf->setHelp(PROGMEM"Gyro factor in artifical horizont bank and pitch (more instant movement), zero disables Gyro");
	top->addEntry( ahrsgf );

	SetupMenuSelect * rpyl = new SetupMenuSelect( "AHRS RPYL", false , 0, true, &ahrs_rpyl_dataset );
	top->addEntry( rpyl );
	rpyl->setHelp( PROGMEM "Send LEVIL AHRS like $RPYL sentence for artifical horizon");
	rpyl->addEntry( "Disable");
	rpyl->addEntry( "Enable");

	SetupMenuValFloat * tcontrol = new SetupMenuValFloat( "AHRS Temp Control", "", -1, 60, 1, 0, false, &mpu_temperature  );
	tcontrol->setPrecision( 0 );
	tcontrol->setHelp( PROGMEM"Regulated target temperature of AHRS silicon chip, if supported in hardware (model > 2023), -1 means OFF");
	top->addEntry( tcontrol );
}



void SetupMenu::system_menu_create_hardware( MenuEntry *top ){
	SetupMenu * display = new SetupMenu( "DISPLAY Setup" );
	top->addEntry( display );
	display->addCreator( system_menu_create_hardware_type );

	SetupMenu * rotary = new SetupMenu( "Rotary Setup" );
	top->addEntry( rotary );
	rotary->addCreator( system_menu_create_hardware_rotary );

	SetupMenuSelect * s2fsw = new SetupMenuSelect( "S2F Switch", false , 0, false, &s2f_switch_type );
	top->addEntry( s2fsw );
	s2fsw->setHelp( PROGMEM "Select S2F hardware switch type, what can be an normal switch or a push button without lock toggling S2F mode any time pressed");
	s2fsw->addEntry( "Switch");
	s2fsw->addEntry( "Push Button");
	s2fsw->addEntry( "Switch Inverted");

	SetupMenuSelect * gear = new SetupMenuSelect( "Gear Warn", false , config_gear_warning, false, &gear_warning );
	top->addEntry( gear );
	gear->setHelp( PROGMEM "Enable gear warning on S2 flap sensor or serial RS232 pin (pos. or neg. signal) or by external command", 220 );
	gear->addEntry( "Disable");
	gear->addEntry( "S2 Flap positive");   // A positive signal, high signal or > 2V will start alarm
	gear->addEntry( "S2 RS232 positive");
	gear->addEntry( "S2 Flap negative");   // A negative signal, low signal or < 1V will start alarm
	gear->addEntry( "S2 RS232 negative");
	gear->addEntry( "External");           // A $g,w<n>*CS command from an external device

	if( hardwareRevision.get() >= XCVARIO_21 ){
		SetupMenu * ahrs = new SetupMenu( "AHRS Setup" );
		top->addEntry( ahrs );
		ahrs->addCreator( system_menu_create_hardware_ahrs );
	}

	SetupMenuSelect * pstype = new SetupMenuSelect( "AS Sensor type", true , 0, false, &airspeed_sensor_type );
	top->addEntry( pstype );
	pstype->setHelp( PROGMEM "Factory default for type of pressure sensor, will not erase on factory reset");
	pstype->addEntry( "ABPMRR");
	pstype->addEntry( "TE4525");
	pstype->addEntry( "MP5004");
	pstype->addEntry( "Autodetect");

	SetupMenuValFloat * met_adj = new SetupMenuValFloat( "Voltmeter Adjust", "%",	-25.0, 25.0, 0.01, factv_adj, false, &factory_volt_adjust,  true, false, true);
	met_adj->setHelp(PROGMEM "Option to fine factory adjust voltmeter");
	top->addEntry( met_adj );
}

void SetupMenu::system_menu_create_altimeter_airspeed_stallwa( MenuEntry *top ){
	SetupMenuSelect * stawaen = new SetupMenuSelect( "Stall Warning", false, 0, false, &stall_warning );
	top->addEntry( stawaen );
	stawaen->setHelp( PROGMEM "Enable alarm sound when speed goes below configured stall speed and below (until 30% less)");
	stawaen->addEntry( "Disable");
	stawaen->addEntry( "Enable");

	SetupMenuValFloat * staspe = new SetupMenuValFloat( "Stall Speed", "", 20, 200, 1, 0, true, &stall_speed, true  );
	staspe->setHelp(PROGMEM"Configure stalling speed for corresponding airplane type and reboot");
	top->addEntry( staspe );
}

void SetupMenu::system_menu_create_altimeter_airspeed( MenuEntry *top ){
	SetupMenuSelect * als = new SetupMenuSelect( "Altimeter Source", false, 0, true, &alt_select );
	top->addEntry( als );
	als->setHelp( PROGMEM "Select source for barometric altitude, either TE sensor or Baro sensor (recommended) or an external source e.g. FLARM (if avail)");
	als->addEntry( "TE Sensor");
	als->addEntry( "Baro Sensor");
	als->addEntry( "External");

	SetupMenuValFloat * spc = new SetupMenuValFloat( "AS Calibration", "%", -100, 100, 1, speedcal_change, false, &speedcal  );
	spc->setHelp(PROGMEM"Calibration of airspeed sensor (AS). Normally not needed, hence pressure probes may have systematic error");
	top->addEntry( spc );

	SetupMenuSelect * auze = new SetupMenuSelect( "AutoZero AS Sensor",	true, 0, true, &autozero );
	top->addEntry( auze );
	auze->setHelp( PROGMEM "Recalculate zero point for airspeed sensor on next power on");
	auze->addEntry( "Cancel");
	auze->addEntry( "Start");

	SetupMenuSelect * alq = new SetupMenuSelect( "Alt. Quantization", false,  0, true, &alt_quantization );
	alq->setHelp( PROGMEM "Set altimeter mode with discrete steps and rolling last digits");
	alq->addEntry( "Disable");
	alq->addEntry( "2");
	alq->addEntry( "5");
	alq->addEntry( "10");
	alq->addEntry( "20");
	top->addEntry( alq );

	SetupMenu * stallwa = new SetupMenu( "Stall Warning");
	top->addEntry( stallwa );
	stallwa->setHelp( PROGMEM "Configure stall warning parameters");
	stallwa->addCreator( system_menu_create_altimeter_airspeed_stallwa );

	SetupMenuValFloat * vmax = new SetupMenuValFloat( "Maximum Speed", "", 70, 450, 1, 0, false, &v_max  );
	vmax->setHelp(PROGMEM"Configure maximum speed for corresponding airplane type");
	top->addEntry( vmax );
}

void SetupMenu::system_menu_create_interfaceS1_routing( MenuEntry *top ){
	SetupMenuSelect * s1outxcv = new SetupMenuSelect( PROGMEM "XCVario", false, update_routing_s1, true, &rt_s1_xcv );
	s1outxcv->addEntry( "Disable");
	s1outxcv->addEntry( "Enable");
	top->addEntry( s1outxcv );

	SetupMenuSelect * s1outwl = new SetupMenuSelect( PROGMEM  "Wireless", false, update_routing_s1, true, &rt_s1_wl );
	s1outwl->addEntry( "Disable");
	s1outwl->addEntry( "Enable");
	top->addEntry( s1outwl );

	SetupMenuSelect * s1outs1 = new SetupMenuSelect( PROGMEM  "S2-RS232", false, update_routing_s1, true, &rt_s1_s2 );
	s1outs1->addEntry( "Disable");
	s1outs1->addEntry( "Enable");
	top->addEntry( s1outs1 );

	SetupMenuSelect * s1outcan = new SetupMenuSelect( PROGMEM "CAN-bus", false, update_routing_s1, true, &rt_s1_can );
	s1outcan->addEntry( "Disable");
	s1outcan->addEntry( "Enable");
	top->addEntry( s1outcan );
}

void SetupMenu::system_menu_create_interfaceS1( MenuEntry *top ){
	SetupMenuSelect * s2sp = new SetupMenuSelect( PROGMEM "Baudraute",	true, 0, true, &serial1_speed );
	top->addEntry( s2sp );
	s2sp->addEntry( "OFF");
	s2sp->addEntry( "4800 baud");
	s2sp->addEntry( "9600 baud");
	s2sp->addEntry( "19200 baud");
	s2sp->addEntry( "38400 baud");
	s2sp->addEntry( "57600 baud");
	s2sp->addEntry( "115200 baud");

	SetupMenuSelect * s1in = new SetupMenuSelect( PROGMEM "Serial Loops", false, 0, true, &serial1_rxloop );
	top->addEntry( s1in );
	s1in->setHelp( "Option to loop serial S1 RX to S1 TX, e.g. for unidirectional OV or Kobo connection" );
	s1in->addEntry( "Disable");     // 0
	s1in->addEntry( "Enable");      // 1

	SetupMenu * s1out = new SetupMenu( PROGMEM "S1 Routing");
	top->addEntry( s1out );
	s1out->setHelp( PROGMEM "Select data source to be routed from/to serial interface S1");
	s1out->addCreator( system_menu_create_interfaceS1_routing );

	SetupMenuSelect * stxi = new SetupMenuSelect( PROGMEM "TX Inversion", true , 0, true, &serial1_tx_inverted );
	top->addEntry( stxi );
	stxi->setHelp( "Serial RS232 (TTL) option for negative logic, means a '1' will be sent at zero level (RS232 standard and default) and vice versa");
	stxi->addEntry( "Normal");
	stxi->addEntry( "Inverted");

	SetupMenuSelect * srxi = new SetupMenuSelect( PROGMEM "RX Inversion", true, 0, true, &serial1_rx_inverted );
	top->addEntry( srxi );
	srxi->setHelp( "Serial RS232 (TTL) option for negative logic, means a '1' will be received at zero level (RS232 standard and default) and vice versa");
	srxi->addEntry( "Normal");
	srxi->addEntry( "Inverted");

	SetupMenuSelect * srxtw1 = new SetupMenuSelect( PROGMEM "Twist RX/TX Pins", true, 0, true, &serial1_pins_twisted );
	top->addEntry( srxtw1 );
	srxtw1->setHelp( "Option to twist RX and TX line for S1, e.g. for OpenVario. After change also a true power-cycle is needed");
	srxtw1->addEntry( "Normal");
	srxtw1->addEntry( "Twisted");

	SetupMenuSelect * stxdis1 = new SetupMenuSelect( PROGMEM "TX Line", true, 0, true, &serial1_tx_enable );
	top->addEntry( stxdis1 );
	stxdis1->setHelp( "Option to switch off RS232 TX line in case active sending is not required, e.g. for multiple devices connected to one device");
	stxdis1->addEntry( "Disable");
	stxdis1->addEntry( "Enable");
}

void SetupMenu::system_menu_create_interfaceS2_routing( MenuEntry *top ){
	SetupMenuSelect * s2outxcv = new SetupMenuSelect( PROGMEM "XCVario", false, update_routing_s2, true, &rt_s2_xcv );
	s2outxcv->addEntry( "Disable");
	s2outxcv->addEntry( "Enable");
	top->addEntry( s2outxcv );
	SetupMenuSelect * s2outwl = new SetupMenuSelect( PROGMEM  "Wireless", false, update_routing_s2, true, &rt_s2_wl );
	s2outwl->addEntry( "Disable");
	s2outwl->addEntry( "Enable");
	top->addEntry( s2outwl );
	SetupMenuSelect * s2outs2 = new SetupMenuSelect( PROGMEM  "S1-RS232", false, update_routing_s2, true, &rt_s1_s2 );
	s2outs2->addEntry( "Disable");
	s2outs2->addEntry( "Enable");
	top->addEntry( s2outs2 );
	SetupMenuSelect * s2outcan = new SetupMenuSelect( PROGMEM "CAN-bus", false, update_routing_s2, true, &rt_s2_can );
	s2outcan->addEntry( "Disable");
	s2outcan->addEntry( "Enable");
	top->addEntry( s2outcan );
}

void SetupMenu::system_menu_create_interfaceS2( MenuEntry *top ){
	SetupMenuSelect * s2sp2 = new SetupMenuSelect( PROGMEM "Baudraute",	true, 0, true, &serial2_speed );
	top->addEntry( s2sp2 );
	// s2sp->setHelp( "Serial RS232 (TTL) speed, pins RX:2, TX:3 on external RJ45 connector");
	s2sp2->addEntry( "OFF");
	s2sp2->addEntry( "4800 baud");
	s2sp2->addEntry( "9600 baud");
	s2sp2->addEntry( "19200 baud");
	s2sp2->addEntry( "38400 baud");
	s2sp2->addEntry( "57600 baud");
	s2sp2->addEntry( "115200 baud");

	SetupMenu * s2out = new SetupMenu( PROGMEM "S2 Routing" );
	s2out->setHelp( PROGMEM "Select data source to be routed from/to serial interface S2");
	top->addEntry( s2out );
	s2out->addCreator( system_menu_create_interfaceS1_routing );

	SetupMenuSelect * stxi2 = new SetupMenuSelect( PROGMEM "TX Inversion", true , 0, true, &serial2_tx_inverted );
	top->addEntry( stxi2 );
	stxi2->setHelp( "Serial RS232 (TTL) option for negative logic, means a '1' will be sent at zero level (RS232 standard and default) and vice versa");
	stxi2->addEntry( "Normal");
	stxi2->addEntry( "Inverted");

	SetupMenuSelect * srxi2 = new SetupMenuSelect( PROGMEM "RX Inversion", true, 0, true, &serial2_rx_inverted );
	top->addEntry( srxi2 );
	srxi2->setHelp( "Serial RS232 (TTL) option for negative logic, means a '1' will be received at zero level (RS232 standard and default) and vice versa");
	srxi2->addEntry( "Normal");
	srxi2->addEntry( "Inverted");

	SetupMenuSelect * srxtw2 = new SetupMenuSelect( PROGMEM "Twist RX/TX Pins", true, 0, true, &serial2_pins_twisted );
	top->addEntry( srxtw2 );
	srxtw2->setHelp( "Option to twist RX and TX line for S2, e.g. for OpenVario. After change also a true power-cycle is needed");
	srxtw2->addEntry( "Normal");
	srxtw2->addEntry( "Twisted");

	SetupMenuSelect * stxdis2 = new SetupMenuSelect( PROGMEM "TX Line", true, 0, true, &serial2_tx_enable );
	top->addEntry( stxdis2 );
	stxdis2->setHelp( "Option to switch off RS232 TX line in case active sending is not required, e.g. for multiple devices connected to one device");
	stxdis2->addEntry( "Disable");
	stxdis2->addEntry( "Enable");
}

void SetupMenu::system_menu_create_interfaceCAN_routing( MenuEntry *top ){
	SetupMenuSelect * canoutxcv = new SetupMenuSelect( PROGMEM "XCVario", false, 0, true, &rt_can_xcv );
	canoutxcv->addEntry( "Disable");
	canoutxcv->addEntry( "Enable");
	top->addEntry( canoutxcv );

	SetupMenuSelect * canoutwl = new SetupMenuSelect( PROGMEM "Wireless", false, 0, true, &rt_wl_can );
	canoutwl->addEntry( "Disable");
	canoutwl->addEntry( "Enable");
	top->addEntry( canoutwl );

	SetupMenuSelect * canouts1 = new SetupMenuSelect( PROGMEM "S1-RS232", false, update_routing_s1, true, &rt_s1_can );
	canouts1->addEntry( "Disable");
	canouts1->addEntry( "Enable");
	top->addEntry( canouts1 );

	SetupMenuSelect * canouts2 = new SetupMenuSelect( PROGMEM "S2-RS232", false, update_routing_s2, true, &rt_s2_can );
	canouts2->addEntry( "Disable");
	canouts2->addEntry( "Enable");
	top->addEntry( canouts2 );
}

void SetupMenu::system_menu_create_interfaceCAN( MenuEntry *top ){
	SetupMenuSelect * canmode = new SetupMenuSelect( PROGMEM "Datarate", true, 0, true, &can_speed );
	top->addEntry( canmode );
	canmode->setHelp( "Datarate on high speed serial CAN interace in kbit per second");
	canmode->addEntry( "CAN OFF");
	canmode->addEntry( "250 kbit");
	canmode->addEntry( "500 kbit");
	canmode->addEntry( "1000 kbit");

	SetupMenu * canrt = new SetupMenu( PROGMEM "CAN Routing" );
	top->addEntry( canrt );
	canrt->setHelp( PROGMEM "Select data source that is routed from/to CAN interface");
	canrt->addCreator( system_menu_create_interfaceCAN_routing );

	SetupMenuSelect * devmod = new SetupMenuSelect( PROGMEM "Mode", true , 0, false, &can_mode );
	top->addEntry( devmod );
	devmod->setHelp( PROGMEM "Select 'Standalone' for single seater or 'Master' in front, for secondary device in rear 'Client'");
	devmod->addEntry( "Master");
	devmod->addEntry( "Client");
	devmod->addEntry( "Standalone");
}

void SetupMenu::system_menu_create( MenuEntry *sye ){
	SetupMenu * soft = new SetupMenu( "Software Update" );
	sye->addEntry( soft );
	soft->addCreator(system_menu_create_software);

	SetupMenuSelect * fa = new SetupMenuSelect( "Factory Reset", true, 0, false, &factory_reset );
	fa->setHelp(PROGMEM "Option to reset all settings to factory defaults, means metric system, 5 m/s vario range and more");
	fa->addEntry( "Cancel");
	fa->addEntry( "ResetAll");
	sye->addEntry( fa );

	SetupMenu * bat = new SetupMenu( "Battery Setup" );
	bat->setHelp( PROGMEM "Adjust corresponding voltage for battery symbol display low,red,yellow and full");
	sye->addEntry( bat );
	bat->addCreator(system_menu_create_battery);

	SetupMenu * hardware = new SetupMenu( "Hardware Setup" );
	hardware->setHelp( PROGMEM "Setup variometer hardware e.g. display, rotary, AS and AHRS sensor, voltmeter, etc", 240 );
	sye->addEntry( hardware );
	hardware->addCreator(system_menu_create_hardware);

	// Altimeter, IAS
	SetupMenu * aia = new SetupMenu( "Altimeter, Airspeed" );
	sye->addEntry( aia );
	aia->addCreator(system_menu_create_altimeter_airspeed);

	// _serial1_speed
	SetupMenu * rs232 = new SetupMenu( "RS232 Interface S1" );
	sye->addEntry( rs232 );
	rs232->addCreator(system_menu_create_interfaceS1);

	if( hardwareRevision.get() >= XCVARIO_21 ) {
		SetupMenu * rs232_2 = new SetupMenu( "RS232 Interface S2" );
		sye->addEntry( rs232_2 );
		rs232_2->addCreator(system_menu_create_interfaceS2);
	}
	if( hardwareRevision.get() >= XCVARIO_22 ){
		// Can Interface C1
		SetupMenu * can = new SetupMenu( "CAN Interface" );
		sye->addEntry( can );
		can->addCreator(system_menu_create_interfaceCAN);
	}

	// NMEA protocol of variometer
	SetupMenuSelect * nmea = new SetupMenuSelect( PROGMEM "NMEA Protocol", false , 0, true, &nmea_protocol );
	sye->addEntry( nmea );
	nmea->setHelp( "Setup the protocol used for sending NMEA sentences. This needs to be inline with the device driver chosen in XCSoar/LK8000");
	nmea->addEntry( "OpenVario");
	nmea->addEntry( "Borgelt");
	nmea->addEntry( "Cambridge");
	nmea->addEntry( "XCVario");
	nmea->addEntry( "Disable");

	SetupMenuSelect * nmeas = new SetupMenuSelect( PROGMEM "NMEA Streams", false , change_streams, true, &nmea_streams );
	sye->addEntry( nmeas );
	nmeas->setHelp( "Setup the flight test streams for IMU or SEN (other Sensors) to be transmitted in realtime");
	nmeas->addEntry( "Disable");
	nmeas->addEntry( "IMU");
	nmeas->addEntry( "SEN");
	nmeas->addEntry( "IMU&SEN");
}

void SetupMenu::setup_create_root(MenuEntry *top ){
	ESP_LOGI(FNAME,"setup_create_root()");
	if( rot_default.get() == 0 ) {
		SetupMenuValFloat * mc = new SetupMenuValFloat( "MC", "",	0.0, 9.9, 0.1, 0, true, &MC );
		mc->setHelp(PROGMEM"Mac Cready value for optimum cruise speed, or average climb rate to be provided in same unit as the variometer");
		mc->setPrecision(1);
		top->addEntry( mc );
	}
	else {
		SetupMenuValFloat * vol = new SetupMenuValFloat( "Audio Volume", "%", 0.0, 100, 1, vol_adj, true, &audio_volume );
		vol->setHelp(PROGMEM"Audio volume level for variometer tone on internal and external speaker");
		top->addEntry( vol );
	}

	SetupMenuValFloat * bgs = new SetupMenuValFloat( "Bugs", "%", 0.0, 50, 1, bug_adj, true, &bugs  );
	bgs->setHelp(PROGMEM"Percent of bugs contamination to indicate degradation of gliding performance");
	top->addEntry( bgs );

	SetupMenuValFloat * bal = new SetupMenuValFloat( "Ballast", "litre", 0.0, 500, 1, water_adj, true, &ballast_kg  );
	bal->setHelp(PROGMEM"Amount of water ballast added to the over all weight");
	bal->setPrecision(0);
	top->addEntry( bal );

	SetupMenuValFloat * crewball = new SetupMenuValFloat( "Crew Weight", "kg", 0, 300, 1, crew_weight_adj, false, &crew_weight );
	crewball->setPrecision(0);
	crewball->setHelp(PROGMEM"Weight of the pilot(s) including parachute (everything on top of the empty weight apart from ballast)");
	top->addEntry( crewball );

	SetupMenuValFloat *qnh_menu = SetupMenu::createQNHMenu();
	top->addEntry( qnh_menu );

	SetupMenuValFloat * afe = new SetupMenuValFloat( "Airfield Elevation", "", -1, 3000, 1, 0, true, &elevation );
	afe->setHelp(PROGMEM"Airfield elevation in meters for QNH auto adjust on ground according to this elevation");
	top->addEntry( afe );

    // Clear student mode, password correct
	if( (int)(password.get()) == 271 ) {
		student_mode.set( 0 );
		password.set( 0 );
	}
    // Student mode: Query password
	if( student_mode.get() )
	{
		SetupMenuValFloat * passw = new SetupMenuValFloat( "Expert Password", "", 0, 1000, 1, 0, false, &password  );
		passw->setPrecision( 0 );
		passw->setHelp( PROGMEM"To exit from student mode enter expert password and restart device after expert password has been set correctly");
		top->addEntry( passw );
	}
	else
	{
		// Vario
		SetupMenu * va = new SetupMenu( "Vario and Speed 2 Fly" );
		top->addEntry( va );
		va->addCreator( vario_menu_create );

		// Audio
		SetupMenu * ad = new SetupMenu( "Audio" );
		top->addEntry( ad );
		ad->addCreator( audio_menu_create );

		// Glider Setup
		SetupMenu * po = new SetupMenu( "Glider Details" );
		top->addEntry( po );
		po->addCreator( glider_menu_create );

		// Options Setup
		SetupMenu * opt = new SetupMenu( "Options" );
		top->addEntry( opt );
		opt->addCreator( options_menu_create );

		// System Setup
		SetupMenu * sy = new SetupMenu( "System" );
		top->addEntry( sy );
		sy->addCreator( system_menu_create );
	}
}

SetupMenuValFloat * SetupMenu::createQNHMenu(){
	SetupMenuValFloat * qnh = new SetupMenuValFloat( "QNH", "", 900, 1100.0, 0.250, qnh_adj, true, &QNH );
	qnh->setHelp(PROGMEM"QNH pressure value from next ATC. On ground you may adjust to airfield altitude above MSL", 180 );
	return qnh;
}

void SetupMenu::setup( )
{
	ESP_LOGI(FNAME,"SetupMenu setup()");
	SetupMenu * root = new SetupMenu( "Setup" );
	root->setRoot( root );
	root->addEntry( root );
	// Create static menues
	if( NEED_VOLTAGE_ADJUST && !SetupMenuValFloat::meter_adj_menu ){
			SetupMenuValFloat::meter_adj_menu = new SetupMenuValFloat( "Voltmeter Adjust", "%",	-25.0, 25.0, 0.01, factv_adj, false, &factory_volt_adjust,  true, false, true);
	}
	setup_create_root( root );
}
