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
	ESP_LOGI(FNAME,"init_routing S1: %x", s1rt);
	rt_s1_xcv.set( (s1rt >> (RT_XCVARIO))& 1 );
	rt_s1_wl.set( (s1rt >> (RT_WIRELESS))& 1 );
	rt_s1_s2.set( (s1rt >> (RT_S1))& 1 );
	rt_s1_can.set( (s1rt >> (RT_CAN))& 1 );

	uint32_t s2rt = (uint32_t)serial2_tx.get();
	ESP_LOGI(FNAME,"init_routing S2: %x", s2rt);
	rt_s2_xcv.set( (s2rt >> (RT_XCVARIO))& 1 );
	rt_s2_wl.set( (s2rt >> (RT_WIRELESS))& 1 );
	rt_s1_s2.set( (s2rt >> (RT_S1))& 1 );
	rt_s2_can.set( (s2rt >> (RT_CAN))& 1 );
}


int update_routing( SetupMenuSelect * p ){
	uint32_t routing =
			( (uint32_t)rt_s1_xcv.get() << (RT_XCVARIO) ) |
			( (uint32_t)rt_s1_wl.get()  << (RT_WIRELESS) ) |
			( (uint32_t)rt_s1_s2.get()  << (RT_S1) ) |
			( (uint32_t)rt_s1_can.get() << (RT_CAN) );
	ESP_LOGI(FNAME,"update_routing S1: %x", routing);
	serial1_tx.set( routing );
	routing =
			(uint32_t)rt_s2_xcv.get()  << (RT_XCVARIO) |
			( (uint32_t)rt_s2_wl.get() << (RT_WIRELESS) ) |
			( (uint32_t)rt_s1_s2.get() << (RT_S1) ) |
			( (uint32_t)rt_s2_can.get()<< (RT_CAN) );
	ESP_LOGI(FNAME,"update_routing S2: %x", routing);
	serial2_tx.set( routing );
	return 0;
}

void init_screens(){
	uint32_t scr = menu_screens.get();
	screen_gmeter.set( (scr >> SCREEN_GMETER) & 1);
	// 	screen_centeraid.set( (scr >> SCREEN_THERMAL_ASSISTANT) & 1);
	screen_horizon.set( (scr >> SCREEN_HORIZON) & 1);
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

int audio_setup_s(SetupMenuSelect * p)
{
	Audio::setup();
	return 0;
}

int audio_setup_f(SetupMenuValFloat * p)
{
	Audio::setup();
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
			( (uint32_t)screen_horizon.get() << (SCREEN_HORIZON) )
			);
	menu_screens.set( screens );
	// init_screens();
	return 0;
}

int do_display_test(SetupMenuSelect * p){
	if( display_test.get() ){
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		p->ucg->setColor( 0,0,0 );
		p->ucg->drawBox( 0, 0, 240, 320 );
		xSemaphoreGive(spiMutex );
		while( !p->readSwitch() ){
			delay(100);
			ESP_LOGI(FNAME,"Wait for key press");
		}
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		p->ucg->setColor( 255,255,255 );
		p->ucg->drawBox( 0, 0, 240, 320 );
		xSemaphoreGive(spiMutex );
		while( !p->readSwitch() ){
			delay(100);
			ESP_LOGI(FNAME,"Wait for key press");
		}
		esp_restart();
	}
	return 0;
}

int update_s2f_speed(SetupMenuValFloat * p)
{
	Switch::setCruiseSpeed( Units::Airspeed2Kmh( s2f_speed.get() ) );
	return 0;
}

static char rentry0[32];
static char rentry1[32];
static char rentry2[32];

int update_rentry(SetupMenuValFloat * p)
{
	// ESP_LOGI(FNAME,"update_rentry() vu:%s ar:%p", Units::VarioUnit(), audio_range_sm );
	sprintf( rentry0, "Fixed (5  %s)", Units::VarioUnit() );
	sprintf( rentry1, "Fixed (10 %s)", Units::VarioUnit() );
	sprintf( rentry2, "Variable (%d %s)", (int)(range.get()), Units::VarioUnit() );
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
	if( (ballast_kg.get() > polar_max_ballast.get()) || (ballast_kg.get() < 0) ){
		ballast_kg.set(0);
		ballast_kg.commit();
		delay(1000);
	}
	p->setMax(polar_max_ballast.get());
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
	// Audio::setVolume( (*(p->_value)) );
	return 0;
}

int cur_vol_dflt( SetupMenuSelect *p ){
	if( p->getSelect() != 0 )  // "set"
		default_volume.set( audio_volume.get() );
	p->setSelect( 0 );   // return to "cancel"
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
			float vol = audio_volume.get();
			for( int i=0; i<count; i++ )
				vol = vol * 0.83;
			if( vol<3.0 )
				vol=0;
			audio_volume.set( vol );
			// ESP_LOGI(FNAME,"NEW DN VOL: %f", vol );
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
			float vol = audio_volume.get();
			if( vol<3.0 )
				vol=3.0;
			for( int i=0; i<count; i++ )
				vol = vol * 1.17;
			if( vol > 100.0 )
				vol = 100.0;
			audio_volume.set( vol );
		    // ESP_LOGI(FNAME,"NEW UP VOL: %f", vol );
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
			if( selected->get_restart() )
				selected->restart();
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
	ESP_LOGI(FNAME,"press() active_srceen %d, pressed %d inSet %d  subtree_created: %d mptr: %p", active_screen, pressed, gflags.inSetup, subtree_created, menu_create_ptr );
	create_subtree();
	if( !gflags.inSetup ){
		active_screen = 0;
		while( !active_screen && (screen_index <= screen_mask_len) ){
			if( menu_screens.get() & (1 << screen_index) ){
				active_screen = screen_index;
				ESP_LOGI(FNAME,"New active_screen: %d", active_screen );
			}
			screen_index++;
		}
		if( screen_index > screen_mask_len ){
			ESP_LOGI(FNAME,"select vario screen");
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
		gflags.inSetup=false;
	}
}

void SetupMenu::vario_menu_create_damping( MenuEntry *top ){
	SetupMenuValFloat * vda = new SetupMenuValFloat( 	PROGMEM"Damping", "sec", 2.0, 10.0, 0.1, vario_setup, false, &vario_delay );
	vda->setHelp(PROGMEM"Response time, time constant of Vario low pass kalman filter");
	top->addEntry( vda );

	SetupMenuValFloat * vdav = new SetupMenuValFloat( 	PROGMEM"Averager", "sec", 2.0, 60.0, 1, 0, false, &vario_av_delay );
	vdav->setHelp(PROGMEM"Response time, time constant of digital Average Vario Display");
	top->addEntry( vdav );
}

void SetupMenu::vario_menu_create_meanclimb( MenuEntry *top ){
	SetupMenuValFloat * vccm = new SetupMenuValFloat( PROGMEM"Minimum climb", "",	0.0, 2.0, 0.1, 0, false, &core_climb_min );
	vccm->setHelp(PROGMEM"Minimum climb rate that counts for arithmetic mean climb value");
	top->addEntry( vccm );

	SetupMenuValFloat * vcch = new SetupMenuValFloat( PROGMEM"Duration", "min", 1, 300, 1, 0, false, &core_climb_history );
	vcch->setHelp(PROGMEM"Duration in minutes over which mean climb rate is computed, default is last 3 thermals or 45 min");
	top->addEntry( vcch );

	SetupMenuValFloat * vcp = new SetupMenuValFloat( PROGMEM"Cycle", "sec", 60, 300, 1, 0, false, &core_climb_period );
	vcp->setHelp(PROGMEM"Cycle: number of seconds when mean climb value is recalculated, default is every 60 seconds");
	top->addEntry( vcp);

	SetupMenuValFloat * vcmc = new SetupMenuValFloat( PROGMEM"Major Change", "m/s", 0.1, 5.0, 0.1, 0, false, &mean_climb_major_change );
	vcmc->setHelp(PROGMEM"Change in mean climb during last cycle (minute), that results in a major change indication (with arrow symbol)");
	top->addEntry( vcmc);
}

void SetupMenu::vario_menu_create_s2f( MenuEntry *top ){
	SetupMenuValFloat * vds2 = new SetupMenuValFloat( PROGMEM"Damping", "sec", 0.10001, 10.0, 0.1, 0, false, &s2f_delay );
	vds2->setHelp(PROGMEM"Time constant of S2F low pass filter");
	top->addEntry( vds2 );

	SetupMenuSelect * blck = new SetupMenuSelect( PROGMEM"Blockspeed", RST_NONE, 0 , true, &s2f_blockspeed );
	blck->setHelp(PROGMEM"With Blockspeed enabled, vertical movement of airmass or G-load is not considered for speed to fly (S2F) calculation");
	blck->addEntry( PROGMEM"DISABLE");
	blck->addEntry( PROGMEM"ENABLE");
	top->addEntry( blck );

	SetupMenuSelect * s2fmod = new SetupMenuSelect( PROGMEM"S2F Mode", RST_NONE, 0 , true, &s2f_switch_mode );
	s2fmod->setHelp( PROGMEM"Select data source for switching between S2F and Vario modes", 230 );
	s2fmod->addEntry( PROGMEM"Vario fix");
	s2fmod->addEntry( PROGMEM"Cruise fix");
	s2fmod->addEntry( PROGMEM"Switch");
	s2fmod->addEntry( PROGMEM"AutoSpeed");
	s2fmod->addEntry( PROGMEM"External");
	s2fmod->addEntry( PROGMEM"Flap");
	s2fmod->addEntry( PROGMEM"AHRS-Gyro");
	top->addEntry( s2fmod );

	SetupMenuSelect * s2fsw = new SetupMenuSelect( PROGMEM"S2F Switch", RST_NONE , 0, false, &s2f_switch_type );
	top->addEntry( s2fsw );
	s2fsw->setHelp( PROGMEM "Select S2F switch type: normal switch, push button (toggling S2F mode on each press), or disabled");
	s2fsw->addEntry( PROGMEM"Switch");
	s2fsw->addEntry( PROGMEM"Push Button");
	s2fsw->addEntry( PROGMEM"Switch Invert");
	s2fsw->addEntry( PROGMEM"Disable");

	SetupMenuValFloat * autospeed = new SetupMenuValFloat( PROGMEM"S2F AutoSpeed", "", 20.0, 250.0, 1.0, update_s2f_speed, false, &s2f_speed );
	top->addEntry( autospeed );
	autospeed->setHelp(PROGMEM"Transition speed if using AutoSpeed to switch Vario <-> Cruise (S2F) mode");

	SetupMenuValFloat * s2f_flap = new SetupMenuValFloat( PROGMEM"S2F Flap Pos", "", -3, 3, 0.1, 0 , false, &s2f_flap_pos );
	top->addEntry( s2f_flap );
	s2f_flap->setHelp(PROGMEM"Precise flap position used for switching Vario <-> Cruise (S2F)");

	SetupMenuValFloat * s2f_gyro = new SetupMenuValFloat( PROGMEM"S2F AHRS Deg", "°", 0, 100, 1, 0 , false, &s2f_gyro_deg );
	top->addEntry( s2f_gyro );
	s2f_gyro->setHelp(PROGMEM"Attitude change in degrees per second to switch Vario <-> Cruise (S2F) mode");

	SetupMenuValFloat * s2fhy = new SetupMenuValFloat( PROGMEM"Hysteresis", "",	-20, 20, 1, 0, false, &s2f_hysteresis );
	s2fhy->setHelp(PROGMEM"Hysteresis (+- this value) for Autospeed S2F transition");
	top->addEntry( s2fhy );

	SetupMenuSelect * s2fnc = new SetupMenuSelect( PROGMEM"Arrow Color", RST_NONE, 0 , true, &s2f_arrow_color );
	s2fnc->setHelp( PROGMEM"Select color of the S2F arrow when painted in Up/Down position" );
	s2fnc->addEntry( PROGMEM"White/White");
	s2fnc->addEntry( PROGMEM"Blue/Blue");
	s2fnc->addEntry( PROGMEM"Green/Red");
	top->addEntry( s2fnc );
}

void SetupMenu::vario_menu_create_ec( MenuEntry *top ){
	SetupMenuSelect * enac = new SetupMenuSelect( PROGMEM"eCompensation", RST_NONE, 0 , false, &te_comp_enable );
	enac->setHelp(PROGMEM"Enable/Disable electronic TE compensation option; Enable only when TE port is connected to ST (static) pressure");
	enac->addEntry( PROGMEM"DISABLE");
	enac->addEntry( PROGMEM"ENABLE");
	top->addEntry( enac );

	SetupMenuValFloat * elca = new SetupMenuValFloat( PROGMEM"Adjustment", "%",	-100, 100, 0.1, 0, false, &te_comp_adjust );
	elca->setHelp(PROGMEM"Adjustment option for electronic TE compensation in %. This affects the energy altitude calculated from airspeed");
	top->addEntry( elca );
};

void SetupMenu::vario_menu_create( MenuEntry *vae ){
	ESP_LOGI(FNAME,"SetupMenu::vario_menu_create( %p )", vae );

	SetupMenuValFloat * vga = new SetupMenuValFloat( PROGMEM"Range", "",	1.0, 30.0, 1, audio_setup_f, true, &range );
	vga->setHelp(PROGMEM"Upper and lower value for Vario graphic display region");
	vga->setPrecision( 0 );
	vae->addEntry( vga );

	SetupMenuSelect *vlogscale = new SetupMenuSelect( PROGMEM"Log. Scale", RST_NONE, 0, true, &log_scale );
	vlogscale->setHelp(PROGMEM"Use a logarithmic scale in the vario gauge");
	vlogscale->addEntry( PROGMEM"DISABLE" );
	vlogscale->addEntry( PROGMEM"ENABLE" );
	vae->addEntry( vlogscale, vga );

	SetupMenuSelect * vamod = new SetupMenuSelect( 	PROGMEM"Mode", RST_NONE, 0 , true, &vario_mode );
	vamod->setHelp(PROGMEM"Controls if vario considers polar sink (=Netto), or not (=Brutto), or if Netto vario applies only in Cruise Mode");
	vamod->addEntry( PROGMEM"Brutto");
	vamod->addEntry( PROGMEM"Netto");
	vamod->addEntry( PROGMEM"Cruise-Netto");
	vae->addEntry( vamod, vlogscale);

	SetupMenuSelect * nemod = new SetupMenuSelect( PROGMEM"Netto Mode", RST_NONE, 0 , true, &netto_mode );
	nemod->setHelp(PROGMEM"In 'Relative' mode (also called 'Super-Netto') circling sink is considered,  to show climb rate as if you were circling there");
	nemod->addEntry( PROGMEM"Normal");
	nemod->addEntry( PROGMEM"Relative");
	vae->addEntry( nemod, vamod  );

	SetupMenuSelect * sink = new SetupMenuSelect( PROGMEM"Polar Sink", RST_NONE, 0 , true, &ps_display );
	sink->setHelp(PROGMEM"Display polar sink rate together with climb rate when Vario is in Brutto Mode (else disabled)");
	sink->addEntry( "DISABLE");
	sink->addEntry( "ENABLE");
	vae->addEntry( sink, nemod );

	SetupMenuSelect * ncolor = new SetupMenuSelect( PROGMEM"Needle Color", RST_NONE, 0 , true, &needle_color );
	ncolor->setHelp(PROGMEM"Choose the color of the vario needle");
	ncolor->addEntry( PROGMEM"White");
	ncolor->addEntry( PROGMEM"Orange");
	ncolor->addEntry( PROGMEM"Red");
	vae->addEntry( ncolor, sink );

	SetupMenuSelect * scrcaid = new SetupMenuSelect( PROGMEM"Center-Aid", RST_ON_EXIT, 0, true, &screen_centeraid );
	scrcaid->setHelp(PROGMEM"Enable/disable display of centering aid (reboots)");
	scrcaid->addEntry( PROGMEM"Disable");
	scrcaid->addEntry( PROGMEM"Enable");
	vae->addEntry(scrcaid,ncolor);

	SetupMenu * vdamp = new SetupMenu( PROGMEM"Vario Damping" );
	vae->addEntry( vdamp, scrcaid );
	vdamp->addCreator( vario_menu_create_damping );

	SetupMenu * meanclimb = new SetupMenu( PROGMEM"Mean Climb" );
	meanclimb->setHelp(PROGMEM"Options for calculation of Mean Climb (MC recommendation) displayed by green/red rhombus");
	vae->addEntry( meanclimb, vdamp);
	meanclimb->addCreator( vario_menu_create_meanclimb );

	SetupMenu * s2fs = new SetupMenu( PROGMEM"S2F Settings" );
	vae->addEntry( s2fs, meanclimb );
	s2fs->addCreator( vario_menu_create_s2f );

	SetupMenu * elco = new SetupMenu( PROGMEM"Electronic Compensation" );
	vae->addEntry( elco, s2fs );
	elco->addCreator( vario_menu_create_ec );
}

void SetupMenu::audio_menu_create_tonestyles( MenuEntry *top ){
	SetupMenuValFloat * cf = new SetupMenuValFloat( PROGMEM"CenterFreq", "Hz", 200.0, 2000.0, 10.0, audio_setup_f, false, &center_freq );
	cf->setHelp(PROGMEM"Center frequency for Audio at zero Vario or zero S2F delta");
	top->addEntry( cf );

	SetupMenuValFloat * oc = new SetupMenuValFloat( "Octaves", "fold", 1.2, 4, 0.05, audio_setup_f, false, &tone_var );
	oc->setHelp(PROGMEM"Maximum tone frequency variation");
	top->addEntry( oc );

	SetupMenuSelect * dt = new SetupMenuSelect( PROGMEM"Dual Tone", RST_NONE, audio_setup_s, true, &dual_tone );
	dt->setHelp(PROGMEM"Select dual tone modue aka ilec sound (di/da/di), or single tone (di/di/di) mode");
	dt->addEntry( PROGMEM"Disable");       // 0
	dt->addEntry( PROGMEM"Enable");        // 1
	top->addEntry( dt );

	SetupMenuValFloat * htv = new SetupMenuValFloat( PROGMEM"Dual Tone Pitch", "%", 0, 50, 1.0, audio_setup_f, false, &high_tone_var );
	htv->setHelp(PROGMEM"Tone variation in Dual Tone mode, percent of frequency pitch up for second tone");
	top->addEntry( htv );

	SetupMenuSelect * tch = new SetupMenuSelect( PROGMEM"Chopping", RST_NONE, eval_chop, true, &chopping_mode );
	tch->setHelp(PROGMEM"Select tone chopping option on positive values for Vario and or S2F");
	tch->addEntry( PROGMEM"Disabled");             // 0
	tch->addEntry( PROGMEM"Vario only");           // 1
	tch->addEntry( PROGMEM"S2F only");             // 2
	tch->addEntry( PROGMEM"Vario and S2F");        // 3  default
	top->addEntry( tch );

	SetupMenuSelect * tchs = new SetupMenuSelect( PROGMEM"Chopping Style", RST_NONE, 0 , true, &chopping_style );
	tchs->setHelp(PROGMEM"Select style of tone chopping either hard, or soft with fadein/fadeout");
	tchs->addEntry( PROGMEM"Soft");              // 0  default
	tchs->addEntry( PROGMEM"Hard");              // 1
	top->addEntry( tchs );

	SetupMenuSelect * advarto = new SetupMenuSelect( PROGMEM"Variable Tone", RST_NONE, 0 , true, &audio_variable_frequency );
	advarto->setHelp(PROGMEM"Enable audio frequency updates within climbing tone intervals, disable keeps frequency constant");
	advarto->addEntry( PROGMEM"Disable");       // 0
	advarto->addEntry( PROGMEM"Enable");        // 1
	top->addEntry( advarto );
}

void SetupMenu::audio_menu_create_deadbands( MenuEntry *top ){
	SetupMenuValFloat * dbminlv = new SetupMenuValFloat( PROGMEM"Lower Vario", "", -5.0, 0, 0.1, 0 , false, &deadband_neg );
	dbminlv->setHelp(PROGMEM"Lower (sink) limit of deadband for Audio mute function when in Vario mode");
	top->addEntry( dbminlv );

	SetupMenuValFloat * dbmaxlv = new SetupMenuValFloat( PROGMEM"Upper Vario", "", 0, 5.0, 0.1, 0 , false, &deadband );
	dbmaxlv->setHelp(PROGMEM"Upper (climb) limit of deadband for Audio mute function when in Vario mode");
	top->addEntry( dbmaxlv );

	SetupMenuValFloat * dbmaxls2fn = new SetupMenuValFloat(	PROGMEM"Lower S2F", "", -25.0, 0, 1, 0 , false, &s2f_deadband_neg );
	dbmaxls2fn->setHelp(PROGMEM"Negative (too slow) speed deviation limit of deadband in S2F mode");
	top->addEntry( dbmaxls2fn );

	SetupMenuValFloat * dbmaxls2f = new SetupMenuValFloat( PROGMEM"Upper S2F", "", 0, 25.0, 1, 0 , false, &s2f_deadband );
	dbmaxls2f->setHelp(PROGMEM"Positive (too fast) speed deviation limit of deadband in S2F mode");
	top->addEntry( dbmaxls2f );
}

void SetupMenu::audio_menu_create_equalizer( MenuEntry *top ){
	SetupMenuSelect * audeqt = new SetupMenuSelect( PROGMEM"Equalizer", RST_ON_EXIT, 0 , true, &audio_equalizer );
	audeqt->setHelp(PROGMEM"Select the equalizer according to the type of loudspeaker used");
	audeqt->addEntry( PROGMEM"Disable");
	audeqt->addEntry( PROGMEM"Speaker 8 Ohms");
	audeqt->addEntry( PROGMEM"Speaker 4 Ohms");
	audeqt->addEntry( PROGMEM"Speaker External");
	top->addEntry( audeqt );

    SetupMenuValFloat * frqr = new SetupMenuValFloat( PROGMEM"Frequency Response", "%", -70.0, 70.0, 1.0, 0, false, &frequency_response );
	frqr->setHelp(PROGMEM"Setup frequency response, double frequency will be attenuated by the factor given, half frequency will be amplified");
	top->addEntry( frqr );
}

void SetupMenu::audio_menu_create_volume( MenuEntry *top ){
	SetupMenuValFloat * vol = new SetupMenuValFloat( PROGMEM"Current Volume", "%",
	    0.0, 100.0, 2.0, vol_adj, false, &audio_volume );
	// unlike top-level menu volume which exits setup, this returns to parent menu
	vol->setHelp(PROGMEM"Audio volume level for variometer tone on internal and external speaker");
	top->addEntry( vol );

	SetupMenuSelect * cdv = new SetupMenuSelect( PROGMEM"Current->Default", RST_NONE, cur_vol_dflt, true );
	cdv->addEntry( PROGMEM"Cancel" );
	cdv->addEntry( PROGMEM"Set" );
	cdv->setHelp(PROGMEM"Set current volume as default volume when device is switched on");
	top->addEntry( cdv );

	SetupMenuValFloat * dv = new SetupMenuValFloat( PROGMEM"Default Volume", "%", 0, 100, 1.0, 0, false, &default_volume );
	top->addEntry( dv );
	dv->setHelp(PROGMEM"Default volume for Audio when device is switched on");

	SetupMenuValFloat * mv = new SetupMenuValFloat( PROGMEM"Max Volume", "%", 0, 100, 1.0, 0, false, &max_volume );
	top->addEntry( mv );
	mv->setHelp(PROGMEM"Maximum audio volume setting allowed. Set to 0% to mute audio entirely.");

	SetupMenu * audeq = new SetupMenu( PROGMEM"Equalizer" );
	top->addEntry( audeq );
	audeq->setHelp( PROGMEM "Equalization parameters for a constant perceived volume over a wide frequency range", 220);
	audeq->addCreator(audio_menu_create_equalizer);

	SetupMenuSelect * amspvol = new SetupMenuSelect( PROGMEM"STF Volume", RST_NONE, 0 , true, &audio_split_vol );
	amspvol->setHelp(PROGMEM"Enable independent audio volume in SpeedToFly and Vario modes, disable for one volume for both");
	amspvol->addEntry( PROGMEM"Disable");      // 0
	amspvol->addEntry( PROGMEM"Enable");       // 1
	top->addEntry( amspvol );
}

void SetupMenu::audio_menu_create_mute( MenuEntry *top ){
	SetupMenuSelect * asida = new SetupMenuSelect( PROGMEM"In Sink", RST_NONE, 0 , true, &audio_mute_sink );
	asida->setHelp(PROGMEM"Select whether vario audio will be muted while in sink");
	asida->addEntry( PROGMEM"Stay On");  // 0
	asida->addEntry( PROGMEM"Mute");     // 1
	top->addEntry( asida );

	SetupMenuSelect * ameda = new SetupMenuSelect( PROGMEM"In Setup", RST_NONE, 0 , true, &audio_mute_menu );
	ameda->setHelp(PROGMEM"Select whether vario audio will be muted while Setup Menu is open");
	ameda->addEntry( PROGMEM"Stay On");  // 0
	ameda->addEntry( PROGMEM"Mute");     // 1
	top->addEntry( ameda );

	SetupMenuSelect * ageda = new SetupMenuSelect( PROGMEM"Generally", RST_NONE, 0 , true, &audio_mute_gen );
	ageda->setHelp(PROGMEM"Select audio on, or vario audio muted, or all audio muted including alarms");
	ageda->addEntry( PROGMEM"Audio On");      // 0
	ageda->addEntry( PROGMEM"Alarms On");     // 1
	ageda->addEntry( PROGMEM"Audio Off");     // 2
	top->addEntry( ageda );

	SetupMenuSelect * amps = new SetupMenuSelect( PROGMEM"Amplifier", RST_NONE, 0 , true, &amplifier_shutdown );
	amps->setHelp(PROGMEM"Select whether amplifier is shutdown during long silences, or always stays on");
	amps->addEntry( PROGMEM"Stay On");   // 0
	amps->addEntry( PROGMEM"Shutdown");  // 1
	top->addEntry( amps );
}

void SetupMenu::audio_menu_create( MenuEntry *audio ){
	SetupMenu * volumes = new SetupMenu( PROGMEM"Volume options" );
	audio->addEntry( volumes );
	volumes->setHelp( PROGMEM "Configure audio volume options", 240);
	volumes->addCreator(audio_menu_create_volume);

	SetupMenu * mutes = new SetupMenu( PROGMEM"Mute Audio" );
	audio->addEntry( mutes );
	mutes->setHelp( PROGMEM "Configure audio muting options", 240);
	mutes->addCreator(audio_menu_create_mute);

	SetupMenuSelect * abnm = new SetupMenuSelect( PROGMEM"Cruise Audio", RST_NONE, 0 , true, &cruise_audio_mode );
	abnm->setHelp(PROGMEM"Select either S2F command or Variometer (Netto/Brutto as selected) as audio source while cruising");
	abnm->addEntry( PROGMEM"Speed2Fly");       // 0
	abnm->addEntry( PROGMEM"Vario");           // 1
	audio->addEntry( abnm );

	SetupMenu * audios = new SetupMenu( PROGMEM"Tone Styles" );
	audio->addEntry( audios );
	audios->setHelp( PROGMEM "Configure audio style in terms of center frequency, octaves, single/dual tone, pitch and chopping", 220);
	audios->addCreator(audio_menu_create_tonestyles);

	update_rentry(0);
	audio_range_sm = new SetupMenuSelect( PROGMEM"Range", RST_NONE, audio_setup_s, true, &audio_range  );
	audio_range_sm->addEntry( rentry0  );
	audio_range_sm->addEntry( rentry1  );
	audio_range_sm->addEntry( rentry2  );
	audio_range_sm->setHelp(PROGMEM"Audio range: fixed, or variable according to current Vario display range setting");
	audio->addEntry( audio_range_sm );

	SetupMenu * db = new SetupMenu( PROGMEM"Deadbands" );
	audio->addEntry( db );
	db->setHelp(PROGMEM"Dead band limits within which audio remains silent.  1 m/s equals roughly 200 fpm or 2 knots");
	db->addCreator(audio_menu_create_deadbands);

	SetupMenuValFloat * afac = new SetupMenuValFloat( PROGMEM"Audio Exponent", "", 0.1, 2, 0.025, 0 , false, &audio_factor );
	afac->setHelp(PROGMEM"How the audio frequency responds to the climb rate: < 1 for logarithmic, and > 1 for exponential, response");
	audio->addEntry( afac);
}

void SetupMenu::glider_menu_create_polarpoints( MenuEntry *top ){
	SetupMenuValFloat * wil = new SetupMenuValFloat( PROGMEM"Ref Wingload", "kg/m2", 10.0, 100.0, 0.1, 0, false, &polar_wingload );
	wil->setHelp(PROGMEM"Wingloading that corresponds to the 3 value pairs for speed/sink of polar");
	top->addEntry( wil );
	SetupMenuValFloat * pov1 = new SetupMenuValFloat( PROGMEM"Speed 1", "km/h", 50.0, 120.0, 1, 0, false, &polar_speed1);
	pov1->setHelp(PROGMEM"Speed 1, near minimum sink from polar e.g. 80 km/h");
	top->addEntry( pov1 );
	SetupMenuValFloat * pos1 = new SetupMenuValFloat( PROGMEM"Sink  1", "m/s", -3.0, 0.0, 0.01, 0, false, &polar_sink1 );
	pos1->setHelp(PROGMEM"Sink at Speed 1 from polar");
	top->addEntry( pos1 );
	SetupMenuValFloat * pov2 = new SetupMenuValFloat( PROGMEM"Speed 2", "km/h", 70.0, 180.0, 1, 0, false, &polar_speed2 );
	pov2->setHelp(PROGMEM"Speed 2 for a moderate cruise from polar e.g. 120 km/h");
	top->addEntry( pov2 );
	SetupMenuValFloat * pos2 = new SetupMenuValFloat( PROGMEM"Sink  2",  "m/s", -5.0, 0.0, 0.01, 0, false, &polar_sink2 );
	pos2->setHelp(PROGMEM"Sink at Speed 2 from polar");
	top->addEntry( pos2 );
	SetupMenuValFloat * pov3 = new SetupMenuValFloat( PROGMEM"Speed 3", "km/h", 100.0, 250.0, 1, 0, false, &polar_speed3 );
	pov3->setHelp(PROGMEM"Speed 3 for a fast cruise from polar e.g. 170 km/h");
	top->addEntry( pov3 );
	SetupMenuValFloat * pos3 = new SetupMenuValFloat( PROGMEM"Sink  3", "m/s", -6.0, 0.0, 0.01, 0, false, &polar_sink3 );
	pos3->setHelp(PROGMEM"Sink at Speed 3 from polar");
	top->addEntry( pos3 );
}

void SetupMenu::glider_menu_create( MenuEntry *poe ){
	SetupMenuSelect * glt = new SetupMenuSelect( PROGMEM"Glider-Type",	RST_NONE, polar_select, true, &glider_type );
	poe->addEntry( glt );
	for( int x=0; x< Polars::numPolars(); x++ ){
		glt->addEntry( Polars::getPolar(x).type );
	}
	poe->setHelp( PROGMEM"Weight and polar setup for best match with performance of glider", 220 );
	ESP_LOGI(FNAME, "Number of Polars installed: %d", Polars::numPolars() );

	SetupMenu * pa = new SetupMenu( PROGMEM"Polar Points" );
	pa->setHelp(PROGMEM "Adjust the polar at 3 points, in the commonly used metric system", 230 );
	poe->addEntry( pa );
	pa->addCreator(glider_menu_create_polarpoints);

	SetupMenuValFloat * maxbal = new SetupMenuValFloat(	PROGMEM"Max Ballast", "liters", 0, 500, 1, 0, false, &polar_max_ballast );
	maxbal->setHelp(PROGMEM"Maximum water ballast for selected glider, to sync with XCSoar using fraction of max ballast");
	poe->addEntry( maxbal );

	SetupMenuValFloat * wingarea = new SetupMenuValFloat( PROGMEM"Wing Area", "m2", 0, 50, 0.1, 0, false, &polar_wingarea );
	wingarea->setHelp(PROGMEM"Wing area for the selected glider - adjustment to support wing extensions or new types, in square meters");
	poe->addEntry( wingarea );

	SetupMenuValFloat * fixball = new SetupMenuValFloat( PROGMEM"Empty Weight", "kg", 0, 1000, 1, empty_weight_adj, false, &empty_weight );
	fixball->setPrecision(0);
	fixball->setHelp(PROGMEM"Net rigged weight of the glider, according to the weight and balance plan");
	poe->addEntry( fixball );
}

void SetupMenu::options_menu_create_units( MenuEntry *top ){
	SetupMenuSelect * alu = new SetupMenuSelect( PROGMEM"Altimeter", RST_NONE, 0, true, &alt_unit );
	alu->addEntry( PROGMEM"Meter (m)");
	alu->addEntry( PROGMEM"Feet (ft)");
	alu->addEntry( PROGMEM"FL (FL)");
	top->addEntry( alu );
	SetupMenuSelect * iau = new SetupMenuSelect( PROGMEM"Airspeed", RST_NONE , 0, true, &ias_unit );
	iau->addEntry( PROGMEM"Kilom./hour (Km/h)");
	iau->addEntry( PROGMEM"Miles/hour (mph)");
	iau->addEntry( PROGMEM"Knots (kt)");
	top->addEntry( iau );
	SetupMenuSelect * vau = new SetupMenuSelect( PROGMEM"Vario", RST_NONE , update_rentrys, true, &vario_unit );
	vau->addEntry( PROGMEM"Meters/sec (m/s)");
	vau->addEntry( PROGMEM"Feet/min x 100 (fpm)");
	vau->addEntry( PROGMEM"Knots (kt)");
	top->addEntry( vau );
	SetupMenuSelect * teu = new SetupMenuSelect( PROGMEM"Temperature", RST_NONE , 0, true, &temperature_unit );
	teu->addEntry( PROGMEM"Celcius");
	teu->addEntry( PROGMEM"Fahrenheit");
	teu->addEntry( PROGMEM"Kelvin");
	top->addEntry( teu );
	SetupMenuSelect * qnhi = new SetupMenuSelect( PROGMEM"QNH", RST_NONE, 0, true, &qnh_unit );
	qnhi->addEntry( PROGMEM"Hectopascal");
	qnhi->addEntry( PROGMEM"InchMercury");
	top->addEntry( qnhi );
	SetupMenuSelect * dst = new SetupMenuSelect( PROGMEM"Distance", RST_NONE , 0, true, &dst_unit );
	dst->addEntry( PROGMEM"Meter (m)");
	dst->addEntry( PROGMEM"Feet (ft)");
	top->addEntry( dst );
}

void SetupMenu::options_menu_create_flarm( MenuEntry *top ){
	SetupMenuSelect * flarml = new SetupMenuSelect( PROGMEM"Alarm Level",	RST_NONE, 0, true, &flarm_warning );
	flarml->setHelp( PROGMEM "Level of FLARM alarm to enable: 1 is lowest (13-18 sec), 2 medium (9-12 sec), 3 highest (0-8 sec) until impact");
	flarml->addEntry( PROGMEM"Disable");
	flarml->addEntry( PROGMEM"Level 1");
	flarml->addEntry( PROGMEM"Level 2");
	flarml->addEntry( PROGMEM"Level 3");
	top->addEntry( flarml );

	SetupMenuValFloat * flarmv = new SetupMenuValFloat( PROGMEM"Alarm Volume",  "%", 20, 100, 1, 0, false, &flarm_volume  );
	flarmv->setHelp( PROGMEM "Maximum audio volume of FLARM alarm warning");
	top->addEntry( flarmv );

	SetupMenuSelect * flarms = new SetupMenuSelect( PROGMEM"FLARM Simulation",	RST_NONE, 0, true, &flarm_sim, false, true );
	flarms->setHelp( PROGMEM "Simulate an airplane crossing from left to right with different alarm levels and vertical distance 5 seconds after pressed (exits setup!)");
	flarms->addEntry( PROGMEM"Disable");
	flarms->addEntry( PROGMEM"Start Sim");
	top->addEntry( flarms );
}

void SetupMenu::options_menu_create_compasswind_compass_dev( MenuEntry *top ){
	const char *skydirs[8] = { "0°", "45°", "90°", "135°", "180°", "225°", "270°", "315°" };
	for( int i = 0; i < 8; i++ )
	{
		SetupMenuSelect* sms = new SetupMenuSelect( PROGMEM"Direction", RST_NONE, compassDeviationAction, false, 0 );
		sms->setHelp( PROGMEM"Push button to start deviation action" );
		sms->addEntry( skydirs[i] );
		top->addEntry( sms );
	}
}

void SetupMenu::options_menu_create_compasswind_compass_nmea( MenuEntry *top ){
	SetupMenuSelect * nmeaHdm = new SetupMenuSelect( PROGMEM"Magnetic Heading", RST_NONE, 0, true, &compass_nmea_hdm );
	nmeaHdm->addEntry( PROGMEM"Disable");
	nmeaHdm->addEntry( PROGMEM"Enable");
	nmeaHdm->setHelp( PROGMEM "Enable/disable NMEA '$HCHDM' sentence generation for magnetic heading" );
	top->addEntry( nmeaHdm );

	SetupMenuSelect * nmeaHdt = new SetupMenuSelect( PROGMEM"True Heading", RST_NONE, 0, true, &compass_nmea_hdt );
	nmeaHdt->addEntry( PROGMEM"Disable");
	nmeaHdt->addEntry( PROGMEM"Enable");
	nmeaHdt->setHelp( PROGMEM "Enable/disable NMEA '$HCHDT' sentence generation for true heading" );
	top->addEntry( nmeaHdt );
}

void SetupMenu::options_menu_create_compasswind_compass( MenuEntry *top ){
	SetupMenuSelect * compSensor = new SetupMenuSelect( PROGMEM"Sensor Option", RST_ON_EXIT, compass_ena, true, &compass_enable);
	compSensor->addEntry( PROGMEM"Disable");
	compSensor->addEntry( PROGMEM"Enable I2C sensor");
	compSensor->addEntry( PROGMEM"Disable");
	compSensor->addEntry( PROGMEM"Enable CAN sensor");
	compSensor->setHelp( PROGMEM "Option to enable/disable the Compass Sensor (reboots)" );
	top->addEntry( compSensor );

	SetupMenuSelect * compSensorCal = new SetupMenuSelect( PROGMEM"Sensor Calibration", RST_NONE, compassSensorCalibrateAction, false );
	compSensorCal->addEntry( PROGMEM"Cancel");
	compSensorCal->addEntry( PROGMEM"Start");
	compSensorCal->addEntry( PROGMEM"Show");
	compSensorCal->addEntry( PROGMEM"Show Raw Data");
	compSensorCal->setHelp( PROGMEM "Calibrate Magnetic Sensor, mandatory for operation" );
	top->addEntry( compSensorCal );

	SetupMenuValFloat *cd = new SetupMenuValFloat( PROGMEM"Setup Declination",	"°",	-180, 180, 1.0, compassDeclinationAction, false, &compass_declination );
	cd->setHelp( PROGMEM "Set compass declination in degrees" );
	top->addEntry( cd );

	SetupMenuSelect * devMenuA = new SetupMenuSelect( PROGMEM"AutoDeviation", RST_NONE, 0, true, &compass_dev_auto );
	devMenuA->setHelp( PROGMEM"Automatic adaptive deviation and precise airspeed evaluation method using data from circling wind");
	devMenuA->addEntry( PROGMEM"Disable");
	devMenuA->addEntry( PROGMEM"Enable");
	top->addEntry( devMenuA );

	SetupMenu * devMenu = new SetupMenu( PROGMEM"Setup Deviations" );
	devMenu->setHelp( PROGMEM"Compass Deviations", 280 );
	top->addEntry( devMenu );
	devMenu->addCreator( options_menu_create_compasswind_compass_dev );

	// Show comapss deviations
	DisplayDeviations* smd = new DisplayDeviations( PROGMEM"Show Deviations" );
	top->addEntry( smd );

	SetupMenuSelect* sms = new SetupMenuSelect( PROGMEM"Reset Deviations ", RST_NONE, compassResetDeviationAction, false,	0 );
	sms->setHelp( PROGMEM"Reset all deviation data to zero" );
	sms->addEntry( PROGMEM"Cancel" );
	sms->addEntry( PROGMEM"Reset" );
	top->addEntry( sms );

	SetupMenu * nmeaMenu = new SetupMenu( PROGMEM"Setup NMEA" );
	top->addEntry( nmeaMenu );
	nmeaMenu->addCreator( options_menu_create_compasswind_compass_nmea );

	SetupMenuValFloat * compdamp = new SetupMenuValFloat( PROGMEM"Damping", "sec", 0.1, 10.0, 0.1, 0, false, &compass_damping );
	compdamp->setPrecision(1);
	top->addEntry( compdamp );
	compdamp->setHelp(PROGMEM "Compass or magnetic heading damping factor in seconds");

	SetupMenuValFloat * compi2c = new SetupMenuValFloat( PROGMEM"I2C Clock", "KHz", 10.0, 400.0, 10, 0, false, &compass_i2c_cl, RST_ON_EXIT );
	top->addEntry( compi2c );
	compi2c->setHelp(PROGMEM "Setup compass I2C Bus clock in KHz (reboots)");

	// Show compass settings
	ShowCompassSettings* scs = new ShowCompassSettings( PROGMEM"Show Settings" );
	top->addEntry( scs );
}

void SetupMenu::options_menu_create_compasswind_straightwind_filters( MenuEntry *top ){
	SetupMenuValFloat *smgsm = new SetupMenuValFloat( PROGMEM"Airspeed Lowpass", "", 0, 0.05, 0.001, nullptr, false, &wind_as_filter );
	smgsm->setPrecision(3);
	top->addEntry( smgsm );
	smgsm->setHelp(PROGMEM "Lowpass filter factor (per sec) for airspeed estimation from AS/Compass and GPS tracks");

	SetupMenuValFloat *devlp = new SetupMenuValFloat( PROGMEM"Deviation Lowpass", "", 0, 0.05, 0.001, nullptr, false, &wind_dev_filter );
	devlp->setPrecision(3);
	top->addEntry( devlp );
	devlp->setHelp(PROGMEM "Lowpass filter factor (per sec) for deviation table correction from AS/Compass and GPS tracks");

	SetupMenuValFloat *smgps = new SetupMenuValFloat( PROGMEM"GPS Lowpass", "sec", 0.1, 10.0, 0.1, nullptr, false, &wind_gps_lowpass );
	smgps->setPrecision(1);
	top->addEntry( smgps );
	smgps->setHelp(PROGMEM"Lowpass filter factor for GPS track and speed, to correlate with Compass latency");

	SetupMenuValFloat *wlpf = new SetupMenuValFloat( PROGMEM"Averager", "", 5, 120, 1, nullptr, false, &wind_filter_lowpass );
	wlpf->setPrecision(0);
	top->addEntry( wlpf );
	wlpf->setHelp(PROGMEM "Number of measurements (seconds) averaged in straight flight live wind estimation");
}

void SetupMenu::options_menu_create_compasswind_straightwind_limits( MenuEntry *top ){
	SetupMenuValFloat *smdev = new SetupMenuValFloat( PROGMEM"Deviation Limit", "°", 0.0, 180.0, 1.0,	nullptr, false, &wind_max_deviation );
	smdev->setHelp( PROGMEM "Maximum deviation change accepted when derived from AS/Compass and GPS tracks" );
	top->addEntry( smdev );

	SetupMenuValFloat *smslip = new SetupMenuValFloat( PROGMEM"Sideslip Limit", "°", 0, 45.0, 0.1, nullptr, false, &swind_sideslip_lim );
	smslip->setPrecision(1);
	top->addEntry( smslip );
	smslip->setHelp(PROGMEM "Maximum side slip estimation in ° accepted in straight wind calculation");

	SetupMenuValFloat *smcourse = new SetupMenuValFloat( PROGMEM"Course Limit", "°", 2.0, 30.0, 0.1, nullptr, false, &wind_straight_course_tolerance );
	smcourse->setPrecision(1);
	top->addEntry( smcourse );
	smcourse->setHelp(PROGMEM "Maximum delta angle in ° per second accepted for straight wind calculation");

	SetupMenuValFloat *aslim = new SetupMenuValFloat( PROGMEM"AS Delta Limit", "km/h", 1.0, 30.0, 1, nullptr, false, &wind_straight_speed_tolerance );
	aslim->setPrecision(0);
	top->addEntry( aslim );
	aslim->setHelp(PROGMEM "Maximum delta in airspeed estimation from wind and GPS during straight flight accepted for straight wind calculation");
}

void SetupMenu::options_menu_create_compasswind_straightwind( MenuEntry *top ){
	SetupMenu * strWindFM = new SetupMenu( PROGMEM"Filters" );
	top->addEntry( strWindFM );
	strWindFM->addCreator( options_menu_create_compasswind_straightwind_filters );
	SetupMenu * strWindLM = new SetupMenu( PROGMEM"Limits" );
	top->addEntry( strWindLM );
	strWindLM->addCreator( options_menu_create_compasswind_straightwind_limits );
	ShowStraightWind* ssw = new ShowStraightWind( PROGMEM"Straight Wind Status" );
	top->addEntry( ssw );
}

void SetupMenu::options_menu_create_compasswind_circlingwind( MenuEntry *top ){
	// Show Circling Wind Status
	ShowCirclingWind* scw = new ShowCirclingWind( PROGMEM"Circling Wind Status" );
	top->addEntry( scw );

	SetupMenuValFloat *cirwd = new SetupMenuValFloat( PROGMEM"Max Delta", "°", 0, 90.0, 1.0, nullptr, false, &max_circle_wind_diff );
	top->addEntry( cirwd );
	cirwd->setHelp(PROGMEM "Maximum accepted value for heading error in circling wind calculation");

	SetupMenuValFloat *cirlp = new SetupMenuValFloat( PROGMEM"Averager", "", 1, 10, 1, nullptr, false, &circle_wind_lowpass );
	cirlp->setPrecision(0);
	top->addEntry( cirlp );
	cirlp->setHelp(PROGMEM"Number of circles used for circling wind averager. A value of 1 means no average");

	SetupMenuValFloat *cirwsd = new SetupMenuValFloat( PROGMEM"Max Speed Delta", "km/h", 0.0, 20.0, 0.1, nullptr, false, &max_circle_wind_delta_speed );
	top->addEntry( cirwsd );
	cirwsd->setPrecision(1);
	cirwsd->setHelp(PROGMEM "Maximum wind speed delta from last measurement accepted for circling wind calculation");

	SetupMenuValFloat *cirwdd = new SetupMenuValFloat( PROGMEM"Max Dir Delta", "°", 0.0, 60.0, 0.1, nullptr, false, &max_circle_wind_delta_deg );
	top->addEntry( cirwdd );
	cirwdd->setPrecision(1);
	cirwdd->setHelp(PROGMEM "Maximum wind direction delta from last measurement accepted for circling wind calculation");
}

void SetupMenu::options_menu_create_compasswind( MenuEntry *top ){
	SetupMenu * compassMenu = new SetupMenu( PROGMEM"Compass" );
	top->addEntry( compassMenu );
	compassMenu->addCreator( options_menu_create_compasswind_compass );

	// Wind speed observation window
	SetupMenuSelect * windcal = new SetupMenuSelect( PROGMEM"Wind Calculation", RST_NONE, 0, true, &wind_enable );
	windcal->addEntry( PROGMEM"Disable");
	windcal->addEntry( PROGMEM"Straight");
	windcal->addEntry( PROGMEM"Circling");
	windcal->addEntry( PROGMEM"Both");
	windcal->setHelp(PROGMEM"Enable Wind calculation for straight flight (needs compass), circling, or both - display wind in retro display style");
	top->addEntry( windcal );

	// Display option
	SetupMenuSelect * winddis = new SetupMenuSelect( PROGMEM"Display", RST_NONE, 0, true, &wind_display );
	winddis->addEntry( PROGMEM"Disable");
	winddis->addEntry( PROGMEM"Wind Digits");
	winddis->addEntry( PROGMEM"Wind Arrow");
	winddis->addEntry( PROGMEM"Wind Both");
	winddis->addEntry( PROGMEM"Compass");
	winddis->setHelp( PROGMEM "What is to be displayed, as digits or arrow or both, on retro style screen. If no wind available, compass is shown");
	top->addEntry( winddis );

	// Wind speed observation window
	SetupMenuSelect * windref = new SetupMenuSelect( PROGMEM"Arrow Ref", RST_NONE, 0, true, &wind_reference );
	windref->addEntry( PROGMEM"North");
	windref->addEntry( PROGMEM"Mag Heading");
	windref->addEntry( PROGMEM"GPS Course");
	windref->setHelp( PROGMEM "Choose wind arrow relative to geographic north or to true aircraft heading");
	top->addEntry( windref );

	SetupMenu * strWindM = new SetupMenu( PROGMEM"Straight Wind" );
	top->addEntry( strWindM );
	strWindM->setHelp( PROGMEM "Straight flight wind calculation needs compass module active", 250 );
	strWindM->addCreator( options_menu_create_compasswind_straightwind );

	SetupMenu * cirWindM = new SetupMenu( PROGMEM"Circling Wind" );
	top->addEntry( cirWindM );
	cirWindM->addCreator( options_menu_create_compasswind_circlingwind );

	SetupMenuSelect * windlog = new SetupMenuSelect( PROGMEM"Wind Logging", RST_NONE, 0, true, &wind_logging );
	windlog->addEntry( PROGMEM"Disable");
	windlog->addEntry( PROGMEM"Enable WIND");
	windlog->addEntry( PROGMEM"Enable GYRO/MAG");
	windlog->addEntry( PROGMEM"Enable Both");
	windlog->setHelp(PROGMEM "Enable Wind logging NMEA output, e.g. to WIFI port");
	top->addEntry( windlog );
}

void SetupMenu::options_menu_create_wireless_routing( MenuEntry *top ){
	SetupMenuSelect * wloutxcv = new SetupMenuSelect( PROGMEM "XCVario", RST_NONE, 0, true, &rt_xcv_wl );
	wloutxcv->addEntry( PROGMEM "Disable");
	wloutxcv->addEntry( PROGMEM "Enable");
	top->addEntry( wloutxcv );
	SetupMenuSelect * wloutxs1 = new SetupMenuSelect( PROGMEM "S1-RS232", RST_NONE, update_routing, true, &rt_s1_wl );
	wloutxs1->addEntry( PROGMEM "Disable");
	wloutxs1->addEntry( PROGMEM "Enable");
	top->addEntry( wloutxs1 );
	SetupMenuSelect * wloutxs2 = new SetupMenuSelect( PROGMEM "S2-RS232", RST_NONE, update_routing, true, &rt_s2_wl );
	wloutxs2->addEntry( PROGMEM "Disable");
	wloutxs2->addEntry( PROGMEM "Enable");
	top->addEntry( wloutxs2 );
	SetupMenuSelect * wloutxcan = new SetupMenuSelect( PROGMEM "CAN-bus", RST_NONE, 0, true, &rt_wl_can );
	wloutxcan->addEntry( PROGMEM "Disable");
	wloutxcan->addEntry( PROGMEM "Enable");
	top->addEntry( wloutxcan );
}


void SetupMenu::options_menu_create_wireless_custom_id( MenuEntry *top ){
	SetupMenuChar * c1 = new SetupMenuChar( PROGMEM"Letter 1",	RST_NONE, update_id, false, custom_wireless_id.get().id, 0 );
	SetupMenuChar * c2 = new SetupMenuChar( PROGMEM"Letter 2",	RST_NONE, update_id, false, custom_wireless_id.get().id, 1 );
	SetupMenuChar * c3 = new SetupMenuChar( PROGMEM"Letter 3",	RST_NONE, update_id, false, custom_wireless_id.get().id, 2 );
	SetupMenuChar * c4 = new SetupMenuChar( PROGMEM"Letter 4",	RST_NONE, update_id, false, custom_wireless_id.get().id, 3 );
	SetupMenuChar * c5 = new SetupMenuChar( PROGMEM"Letter 5",	RST_NONE, update_id, false, custom_wireless_id.get().id, 4 );
	SetupMenuChar * c6 = new SetupMenuChar( PROGMEM"Letter 6",	RST_NONE, update_id, false, custom_wireless_id.get().id, 5 );
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
	SetupMenuSelect * btm = new SetupMenuSelect( PROGMEM"Wireless", RST_ON_EXIT, 0, true, &wireless_type );
	btm->setHelp( PROGMEM"Activate wireless interface type to connect navigation devices, or to another XCVario as client (reboots)", 220 );
	btm->addEntry( PROGMEM"Disable");
	btm->addEntry( PROGMEM"Bluetooth");
	btm->addEntry( PROGMEM"Wireless Master");
	btm->addEntry( PROGMEM"Wireless Client");
	btm->addEntry( PROGMEM"Wireless Standalone");
	btm->addEntry( PROGMEM"Bluetooth LE");
	top->addEntry( btm );

	SetupMenu * wlrt = new SetupMenu( PROGMEM "WL Routing" );
	top->addEntry( wlrt );
	wlrt->setHelp( PROGMEM"Select data source that is routed from/to Wireless BT or WIFI interface");
	wlrt->addCreator( options_menu_create_wireless_routing );

	SetupMenuValFloat *wifip = new SetupMenuValFloat( PROGMEM "WIFI Power", "%", 10.0, 100.0, 5.0, update_wifi_power, false, &wifi_max_power );
	wifip->setPrecision(0);
	top->addEntry( wifip );
	wifip->setHelp(PROGMEM"Maximum Wifi Power to be used 10..100% or 2..20dBm");

	SetupMenuSelect * wifimal = new SetupMenuSelect( PROGMEM "Lock Master", RST_NONE, master_xcv_lock, true, &master_xcvario_lock );
	wifimal->setHelp( PROGMEM "In wireless client role, lock this client to the scanned master XCVario ID above");
	wifimal->addEntry( PROGMEM"Unlock");
	wifimal->addEntry( PROGMEM"Lock");
	top->addEntry( wifimal );

	SetupMenuSelect * datamon = new SetupMenuSelect( PROGMEM "Monitor", RST_NONE, data_mon, true, &data_monitor );
	datamon->setHelp( PROGMEM "Short press button to start/pause, long press to terminate data monitor", 260);
	datamon->addEntry( PROGMEM"Disable");
	datamon->addEntry( PROGMEM"Bluetooth");
	datamon->addEntry( PROGMEM"Wifi 8880");
	datamon->addEntry( PROGMEM"Wifi 8881");
	datamon->addEntry( PROGMEM"Wifi 8882");
	datamon->addEntry( PROGMEM"RS232 S1");
	datamon->addEntry( PROGMEM"RS232 S2");
	datamon->addEntry( PROGMEM"CAN Bus");
	top->addEntry( datamon );

	SetupMenuSelect * datamonmod = new SetupMenuSelect( PROGMEM"Monitor Mode", RST_NONE, data_mon, true, &data_monitor_mode );
	datamonmod->setHelp( PROGMEM"Select data display as ASCII text or as binary hexdump");
	datamonmod->addEntry( PROGMEM"ASCII");
	datamonmod->addEntry( PROGMEM"Binary");
	top->addEntry( datamonmod );

	SetupMenu * cusid = new SetupMenu( PROGMEM PROGMEM"Custom-ID" );
	top->addEntry( cusid );
	cusid->setHelp( PROGMEM "Select custom ID (SSID) for wireless BT (or WIFI) interface, e.g. D-1234. Restart device to activate", 215);
	cusid->addCreator( options_menu_create_wireless_custom_id );
}

void SetupMenu::options_menu_create_gload( MenuEntry *top ){
	SetupMenuSelect * glmod = new SetupMenuSelect( PROGMEM"Activation Mode", RST_NONE, 0, true, &gload_mode );
	glmod->setHelp( PROGMEM "Switch off G-Force screen, or activate by threshold G-Force 'Dynamic', or static by 'Always-On'");
	glmod->addEntry( PROGMEM"Off");
	glmod->addEntry( PROGMEM"Dynamic");
	glmod->addEntry( PROGMEM"Always-On");
	top->addEntry( glmod );

	SetupMenuValFloat * gtpos = new SetupMenuValFloat( PROGMEM"Positive Threshold", "", 1.0, 8.0, 0.1, 0, false, &gload_pos_thresh );
	top->addEntry( gtpos );
	gtpos->setPrecision( 1 );
	gtpos->setHelp(PROGMEM"Positive threshold to launch G-Load display");

	SetupMenuValFloat * gtneg = new SetupMenuValFloat( PROGMEM"Negative Threshold", "", -8.0, 1.0, 0.1, 0, false, &gload_neg_thresh );
	top->addEntry( gtneg );
	gtneg->setPrecision( 1 );
	gtneg->setHelp(PROGMEM"Negative threshold to launch G-Load display");

	SetupMenuValFloat * glpos = new SetupMenuValFloat( PROGMEM"Red positive limit", "", 1.0, 8.0, 0.1, 0, false, &gload_pos_limit );
	top->addEntry( glpos );
	glpos->setPrecision( 1 );
	glpos->setHelp(PROGMEM"Positive g load factor limit the aircraft is able to handle below maneuvering speed, see manual");

	SetupMenuValFloat * glposl = new SetupMenuValFloat( PROGMEM"Yellow pos. Limit", "", 1.0, 8.0, 0.1, 0, false, &gload_pos_limit_low );
	top->addEntry( glposl );
	glposl->setPrecision( 1 );
	glposl->setHelp(PROGMEM"Positive g load factor limit the aircraft is able to handle above maneuvering speed, see manual");

	SetupMenuValFloat * glneg = new SetupMenuValFloat( PROGMEM"Red negative limit", "", -8.0, 1.0, 0.1, 0, false, &gload_neg_limit );
	top->addEntry( glneg );
	glneg->setPrecision( 1 );
	glneg->setHelp(PROGMEM"Negative g load factor limit the aircraft is able to handle below maneuvering speed, see manual");

	SetupMenuValFloat * glnegl = new SetupMenuValFloat( PROGMEM"Yellow neg. Limit", "", -8.0, 1.0, 0.1, 0, false, &gload_neg_limit_low );
	top->addEntry( glnegl );
	glnegl->setPrecision( 1 );
	glnegl->setHelp(PROGMEM"Negative g load factor limit the aircraft is able to handle above maneuvering speed, see manual");


	SetupMenuValFloat * gmpos = new SetupMenuValFloat( PROGMEM"Max Positive", "", 0.0, 0.0, 0.0, 0, false, &gload_pos_max );
	top->addEntry( gmpos );
	gmpos->setPrecision( 1 );
	gmpos->setHelp(PROGMEM"Maximum positive G-Load measured since last reset");

	SetupMenuValFloat * gmneg = new SetupMenuValFloat( PROGMEM"Max Negative", "", 0.0, 0.0, 0.0, 0, false, &gload_neg_max );
	top->addEntry( gmneg );
	gmneg->setPrecision( 1 );
	gmneg->setHelp(PROGMEM"Maximum negative G-Load measured since last reset");

	SetupMenuValFloat * gloadalvo = new SetupMenuValFloat( PROGMEM"Alarm Volume",  "%", 20, 100, 1, 0, false, &gload_alarm_volume  );
	gloadalvo->setHelp( PROGMEM "Maximum volume of G-Load alarm audio warning");
	top->addEntry( gloadalvo );

	SetupMenuSelect * gloadres = new SetupMenuSelect( PROGMEM"G-Load reset", RST_NONE, gload_reset, false, 0 );
	gloadres->setHelp(PROGMEM "Option to reset stored maximum positive and negative G-load values");
	gloadres->addEntry( PROGMEM"Reset");
	gloadres->addEntry( PROGMEM"Cancel");
	top->addEntry( gloadres );
}

void SetupMenu::options_menu_create( MenuEntry *opt ){
	if( student_mode.get() == 0 ) {
		SetupMenuSelect *stumo  = new SetupMenuSelect( PROGMEM"Student Mode", RST_ON_EXIT, 0, true, &student_mode );
		opt->addEntry( stumo );
		stumo->setHelp( PROGMEM "Student mode, disables all sophisticated setup to just basic pre-flight related items like MC, ballast or bugs  (reboots)");
		stumo->addEntry( PROGMEM"Disable");
		stumo->addEntry( PROGMEM"Enable");
	}
	Flap::setupMenue( opt );
	// Units
	SetupMenu * un = new SetupMenu( PROGMEM"Units" );
	opt->addEntry( un );
	un->setHelp( PROGMEM"Setup altimeter, airspeed indicator and variometer with European Metric, American, British or Australian units", 205);
	un->addCreator(options_menu_create_units);

	SetupMenuSelect * amode = new SetupMenuSelect( PROGMEM"Airspeed Mode",	RST_NONE, 0, true, &airspeed_mode );
	opt->addEntry( amode );
	amode->setHelp( PROGMEM "Select mode of Airspeed indicator to display IAS (Indicated AirSpeed), TAS (True AirSpeed) or CAS (calibrated airspeed)", 180 );
	amode->addEntry( PROGMEM"IAS");
	amode->addEntry( PROGMEM"TAS");
	amode->addEntry( PROGMEM"CAS");
	amode->addEntry( PROGMEM"Slip Angle");

	SetupMenuSelect * atl = new SetupMenuSelect( PROGMEM"Auto Transition",	RST_NONE, 0, true, &fl_auto_transition );
	opt->addEntry( atl );
	atl->setHelp( PROGMEM "Option to enable automatic altitude transition to QNH Standard (1013.25) above 'Transition Altitude'");
	atl->addEntry( PROGMEM"Disable");
	atl->addEntry( PROGMEM"Enable");

	SetupMenuSelect * altDisplayMode = new SetupMenuSelect( "Altitude Mode", RST_NONE, 0, true, &alt_display_mode );
	opt->addEntry( altDisplayMode );
	altDisplayMode->setHelp( PROGMEM "Select altitude display mode");
	altDisplayMode->addEntry( PROGMEM"QNH");
	altDisplayMode->addEntry( PROGMEM"QFE");

	SetupMenuValFloat * tral = new SetupMenuValFloat( PROGMEM"Transition Altitude", "FL", 0, 400, 10, 0, false, &transition_alt  );
	tral->setHelp(PROGMEM"Transition altitude (or transition height, when using QFE) is the altitude/height above which standard pressure (QNE) is set (1013.2 mb/hPa)", 100 );
	opt->addEntry( tral );

	SetupMenu * flarm = new SetupMenu( PROGMEM"FLARM" );
	opt->addEntry( flarm );
	flarm->setHelp( PROGMEM"Option to display FLARM Warnings depending on FLARM alarm level");
	flarm->addCreator(options_menu_create_flarm);

	SetupMenu * compassWindMenu = new SetupMenu( PROGMEM"Compass/Wind" );
	opt->addEntry( compassWindMenu );
	compassWindMenu->setHelp( PROGMEM"Setup Compass and Wind", 280 );
	compassWindMenu->addCreator(options_menu_create_compasswind);

	SetupMenu * wireless = new SetupMenu( PROGMEM PROGMEM"Wireless" );
	opt->addEntry( wireless );
	wireless->addCreator(options_menu_create_wireless);

	SetupMenu * gload = new SetupMenu( PROGMEM"G-Load Display" );
	opt->addEntry( gload );
	gload->addCreator(options_menu_create_gload);
}

void SetupMenu::system_menu_create_software( MenuEntry *top ){
	Version V;
	SetupMenuSelect * ver = new SetupMenuSelect( PROGMEM"Software Vers.", RST_NONE, 0, false );
	ver->addEntry( V.version() );
	top->addEntry( ver );

	SetupMenuSelect * upd = new SetupMenuSelect( PROGMEM"Software Update", RST_IMMEDIATE, 0, true, &software_update );
	upd->setHelp(PROGMEM"Software Update over the air (OTA). Starts Wifi AP 'ESP32 OTA' - connect and open http://192.168.4.1 in browser");
	upd->addEntry( PROGMEM"Cancel");
	upd->addEntry( PROGMEM"Start");
	top->addEntry( upd );
}

void SetupMenu::system_menu_create_battery( MenuEntry *top ){
	SetupMenuValFloat * blow = new SetupMenuValFloat( PROGMEM"Battery Low", "Volt ", 0.0, 28.0, 0.1, 0, false, &bat_low_volt );
	SetupMenuValFloat * bred = new SetupMenuValFloat( PROGMEM"Battery Red", "Volt ", 0.0, 28.0, 0.1, 0, false, &bat_red_volt  );
	SetupMenuValFloat * byellow = new SetupMenuValFloat( PROGMEM"Battery Yellow", "Volt ", 0.0, 28.0, 0.1, 0, false, &bat_yellow_volt );
	SetupMenuValFloat * bfull = new SetupMenuValFloat( PROGMEM"Battery Full", "Volt ", 0.0, 28.0, 0.1, 0, false, &bat_full_volt  );

	SetupMenuSelect * batv = new SetupMenuSelect( PROGMEM"Battery Display", RST_NONE, 0, true, &battery_display  );
	batv->setHelp(PROGMEM"Option to display battery charge state either in Percentage e.g. 75% or Voltage e.g. 12.5V");
	batv->addEntry( PROGMEM"Percentage");
	batv->addEntry( PROGMEM"Voltage");
	batv->addEntry( PROGMEM"Voltage Big");

	top->addEntry(blow);
	top->addEntry(bred);
	top->addEntry(byellow);
	top->addEntry(bfull);
	top->addEntry( batv );
}

void SetupMenu::system_menu_create_hardware_type( MenuEntry *top ){
	// UNIVERSAL, RAYSTAR_RFJ240L_40P, ST7789_2INCH_12P, ILI9341_TFT_18P
	SetupMenuSelect * dtype = new SetupMenuSelect( 	"HW Type", RST_NONE, 0, true, &display_type );
	dtype->setHelp( PROGMEM "Factory setup for corresponding display type used");
	dtype->addEntry( PROGMEM"UNIVERSAL");
	dtype->addEntry( PROGMEM"RAYSTAR");
	dtype->addEntry( PROGMEM"ST7789");
	dtype->addEntry( PROGMEM"ILI9341");
	top->addEntry( dtype );

	SetupMenuSelect * disty = new SetupMenuSelect( PROGMEM"Style", RST_NONE , 0, false, &display_style );
	top->addEntry( disty );
	disty->setHelp( PROGMEM"Display style in more digital airliner stype or retro mode with classic vario meter needle");
	disty->addEntry( PROGMEM"Airliner");
	disty->addEntry( PROGMEM"Retro");
	disty->addEntry( PROGMEM"UL");

	SetupMenuSelect * disva = new SetupMenuSelect( PROGMEM"Color Variant", RST_NONE , 0, false, &display_variant );
	top->addEntry( disva );
	disva->setHelp( PROGMEM "Display variant white on black (W/B) or black on white (B/W)");
	disva->addEntry( PROGMEM"W/B");
	disva->addEntry( PROGMEM"B/W");

	// Orientation   _display_orientation
	SetupMenuSelect * diso = new SetupMenuSelect( PROGMEM"Orientation", RST_ON_EXIT, 0, true, &display_orientation );
	top->addEntry( diso );
	diso->setHelp( PROGMEM"Display Orientation.  NORMAL means Rotary on right, TOPDOWN means Rotary on left  (reboots)");
	diso->addEntry( PROGMEM"NORMAL");
	diso->addEntry( PROGMEM"TOPDOWN");

	//
	SetupMenuSelect * drawp = new SetupMenuSelect( PROGMEM"Needle Alignment", RST_NONE, 0, true, &drawing_prio );
	top->addEntry( drawp );
	drawp->setHelp( PROGMEM "Alignment of the variometer needle either in front of the displayed info, or in background");
	drawp->addEntry( PROGMEM"Front");
	drawp->addEntry( PROGMEM"Back");

	SetupMenuSelect * dtest = new SetupMenuSelect( PROGMEM"Display Test", RST_NONE, do_display_test, true, &display_test );
	top->addEntry( dtest );
	dtest->setHelp( PROGMEM "Start display test screens, press rotary to cancel");
	dtest->addEntry( PROGMEM"Cancel");
	dtest->addEntry( PROGMEM"Start Test");
}


void SetupMenu::system_menu_create_hardware_rotary_screens( MenuEntry *top ){
	SetupMenuSelect * scrgmet = new SetupMenuSelect( PROGMEM"G-Meter", RST_NONE, upd_screens, true, &screen_gmeter );
	scrgmet->addEntry( PROGMEM"Disable");
	scrgmet->addEntry( PROGMEM"Enable");
	top->addEntry(scrgmet);
	if( gflags.ahrsKeyValid ){
		SetupMenuSelect * horizon = new SetupMenuSelect( PROGMEM"Horizon", RST_NONE, upd_screens, true, &screen_horizon );
		horizon->addEntry( PROGMEM"Disable");
		horizon->addEntry( PROGMEM"Enable");
		top->addEntry(horizon);
	}
}

void SetupMenu::system_menu_create_hardware_rotary( MenuEntry *top ){
	SetupMenu * screens = new SetupMenu( PROGMEM"Screens");
	top->addEntry( screens );
	screens->setHelp(PROGMEM"Select screens to be activated one after the other by short press");
	screens->addCreator( system_menu_create_hardware_rotary_screens );

	SetupMenuSelect * rotype;
	if( hardwareRevision.get() < XCVARIO_21 )
		rotype = new SetupMenuSelect( PROGMEM"Direction", RST_NONE , 0, false, &rotary_dir );
	else
		rotype = new SetupMenuSelect( PROGMEM"Direction", RST_NONE , 0, false, &rotary_dir_21 );
	top->addEntry( rotype );
	rotype->setHelp( PROGMEM"Select type of rotary switch, different brands may need adjustment");
	rotype->addEntry( PROGMEM"Clockwise");
	rotype->addEntry( PROGMEM"Counterclockwise");

	SetupMenuSelect * roinc = new SetupMenuSelect( PROGMEM"Sensitivity", RST_NONE , 0, false, &rotary_inc );
	top->addEntry( roinc );
	roinc->setHelp( PROGMEM"Select rotary sensitivity in number of Indent's for one increment, to your personal preference, 1 Indent is most sensitive");
	roinc->addEntry( PROGMEM"1 Indent");
	roinc->addEntry( PROGMEM"2 Indent");
	roinc->addEntry( PROGMEM"3 Indent");
	roinc->addEntry( PROGMEM"4 Indent");

	// Rotary Default
	SetupMenuSelect * rd = new SetupMenuSelect( PROGMEM"Rotation", RST_ON_EXIT, 0, true, &rot_default );
	top->addEntry( rd );
	rd->setHelp(PROGMEM"Select value to be altered at rotary movement outside of setup menu (reboots)");
	rd->addEntry( PROGMEM"Volume");
	rd->addEntry( PROGMEM"MC Value");

	SetupMenuSelect * sact = new SetupMenuSelect( PROGMEM"Setup Menu by", RST_NONE, 0, true, &menu_long_press );
	top->addEntry( sact);
	sact->setHelp(PROGMEM"Select Mode to activate setup menu either by short press or long press > 0.4 seconds");
	sact->addEntry( PROGMEM"Short Press");
	sact->addEntry( PROGMEM"Long Press");
}


static const char PROGMEM lkeys[][4] { "0","1","2","3","4","5","6","7","8","9",":",";","<","=",">","?","@","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};

void SetupMenu::system_menu_create_hardware_ahrs_lc( MenuEntry *top ){
	SetupMenuSelect * ahrslc1 = new SetupMenuSelect( PROGMEM"First    Letter",	RST_NONE, add_key, false, &ahrs_licence_dig1 );
	SetupMenuSelect * ahrslc2 = new SetupMenuSelect( PROGMEM"Second Letter",	RST_NONE, add_key, false, &ahrs_licence_dig2 );
	SetupMenuSelect * ahrslc3 = new SetupMenuSelect( PROGMEM"Third   Letter",	RST_NONE, add_key, false, &ahrs_licence_dig3 );
	SetupMenuSelect * ahrslc4 = new SetupMenuSelect( PROGMEM"Last     Letter",	RST_NONE, add_key, false, &ahrs_licence_dig4 );
	top->addEntry( ahrslc1 );
	top->addEntry( ahrslc2 );
	top->addEntry( ahrslc3 );
	top->addEntry( ahrslc4 );
	ahrslc1->addEntryList( lkeys, sizeof(lkeys)/4 );
	ahrslc2->addEntryList( lkeys, sizeof(lkeys)/4 );
	ahrslc3->addEntryList( lkeys, sizeof(lkeys)/4 );
	ahrslc4->addEntryList( lkeys, sizeof(lkeys)/4 );
}


void SetupMenu::system_menu_create_hardware_ahrs_parameter( MenuEntry *top ){
	SetupMenuValFloat * ahrsgf = new SetupMenuValFloat( PROGMEM"Gyro Max Trust", "x", 0, 100, 1, 0, false, &ahrs_gyro_factor  );
	ahrsgf->setPrecision( 0 );
	ahrsgf->setHelp(PROGMEM"Maximum Gyro trust factor in artifical horizon");
	top->addEntry( ahrsgf );

	SetupMenuValFloat * ahrsgfm = new SetupMenuValFloat( PROGMEM"Gyro Min Trust", "x", 0, 100, 1, 0, false, &ahrs_min_gyro_factor  );
	ahrsgfm->setPrecision( 0 );
	ahrsgfm->setHelp(PROGMEM"Minimum Gyro trust factor in artifical horizon");
	top->addEntry( ahrsgfm );

	SetupMenuValFloat * ahrsdgf = new SetupMenuValFloat( PROGMEM"Gyro Dynamics", "", 0.5, 10, 0.1, 0, false, &ahrs_dynamic_factor  );
	ahrsdgf->setHelp(PROGMEM"Gyro dynamics factor, higher value trusts gyro more when load factor is different from one");
	top->addEntry( ahrsdgf );

	SetupMenuValFloat * gyrog = new SetupMenuValFloat( PROGMEM"Gyro Gating", "°", 0, 10, 0.1, 0, false, &gyro_gating  );
	gyrog->setHelp( PROGMEM"Minimum accepted gyro rate in degree per second");
	top->addEntry( gyrog );

	SetupMenuValFloat * gyrocal = new SetupMenuValFloat( PROGMEM"Gyro Calibration", "", -0.5, 1.5, 0.01, 0, false, &ahrs_gyro_cal  );
	gyrocal->setHelp( PROGMEM"Gyro calibration factor to increase accuracy of gyro in %/100");
	top->addEntry( gyrocal );

}

void SetupMenu::system_menu_create_hardware_ahrs( MenuEntry *top ){
	SetupMenuSelect * ahrsid = new SetupMenuSelect( PROGMEM"AHRS ID", RST_NONE, 0, false );
	ahrsid->addEntry( Cipher::id() );
	top->addEntry( ahrsid );

	mpu = new SetupMenuSelect( PROGMEM"AHRS Option", RST_ON_EXIT , 0, true, &attitude_indicator );
	top->addEntry( mpu );
	mpu->setHelp( PROGMEM "Enable High Accuracy Attitude Sensor (AHRS) NMEA messages (need valid license key entered, reboots)");
	mpu->addEntry( PROGMEM"Disable");
	if( gflags.ahrsKeyValid )
		mpu->addEntry( PROGMEM"Enable");

	SetupMenuSelect * ahrsaz = new SetupMenuSelect( "AHRS Autozero", RST_IMMEDIATE , 0, true, &ahrs_autozero );
	top->addEntry( ahrsaz );
	ahrsaz->setHelp( PROGMEM "Start Autozero of AHRS Sensor; Preconditions: On ground; Wings 100% horizontal, fuselage in flight position! (reboots)");
	ahrsaz->addEntry( "Cancel");
	ahrsaz->addEntry( "Start");

	SetupMenu * ahrslc = new SetupMenu( PROGMEM"AHRS License Key" );
	ahrslc->setHelp( PROGMEM"Enter valid AHRS License Key, then AHRS feature can be enabled under 'AHRS Option'");
	top->addEntry( ahrslc );
	ahrslc->addCreator( system_menu_create_hardware_ahrs_lc );

	SetupMenu * ahrspa = new SetupMenu( PROGMEM"AHRS Parameters" );
	ahrspa->setHelp( PROGMEM"AHRS constants such as gyro trust and filtering", 275 );
	top->addEntry( ahrspa );
	ahrspa->addCreator( system_menu_create_hardware_ahrs_parameter );

	SetupMenuSelect * ahrsdef = new SetupMenuSelect( "AHRS Defaults", RST_NONE, 0, true, &ahrs_defaults );
	top->addEntry( ahrsdef );
	ahrsdef->setHelp( PROGMEM "Set optimum default values for all AHRS Parameters as determined to the best practice");
	ahrsdef->addEntry( "Cancel");
	ahrsdef->addEntry( "Set Defaults");

	SetupMenuSelect * rpyl = new SetupMenuSelect( PROGMEM"AHRS RPYL", RST_NONE , 0, true, &ahrs_rpyl_dataset );
	top->addEntry( rpyl );
	rpyl->setHelp( PROGMEM "Send LEVIL AHRS like $RPYL sentence for artifical horizon");
	rpyl->addEntry( PROGMEM"Disable");
	rpyl->addEntry( PROGMEM"Enable");

	SetupMenuValFloat * tcontrol = new SetupMenuValFloat( PROGMEM"AHRS Temp Control", "", -1, 60, 1, 0, false, &mpu_temperature  );
	tcontrol->setPrecision( 0 );
	tcontrol->setHelp( PROGMEM"Regulated target temperature of AHRS silicon chip, if supported in hardware (model > 2023), -1 means OFF");
	top->addEntry( tcontrol );
}



void SetupMenu::system_menu_create_hardware( MenuEntry *top ){
	SetupMenu * display = new SetupMenu( PROGMEM"DISPLAY Setup" );
	top->addEntry( display );
	display->addCreator( system_menu_create_hardware_type );

	SetupMenu * rotary = new SetupMenu( PROGMEM"Rotary Setup" );
	top->addEntry( rotary );
	rotary->addCreator( system_menu_create_hardware_rotary );

	SetupMenuSelect * gear = new SetupMenuSelect( PROGMEM"Gear Warn", RST_NONE , config_gear_warning, false, &gear_warning );
	top->addEntry( gear );
	gear->setHelp( PROGMEM "Enable gear warning on S2 flap sensor or serial RS232 pin (pos. or neg. signal) or by external command", 220 );
	gear->addEntry( PROGMEM"Disable");
	gear->addEntry( PROGMEM"S2 Flap positive");   // A positive signal, high signal or > 2V will start alarm
	gear->addEntry( PROGMEM"S2 RS232 positive");
	gear->addEntry( PROGMEM"S2 Flap negative");   // A negative signal, low signal or < 1V will start alarm
	gear->addEntry( PROGMEM"S2 RS232 negative");
	gear->addEntry( PROGMEM"External");           // A $g,w<n>*CS command from an external device

	if( hardwareRevision.get() >= XCVARIO_21 ){
		SetupMenu * ahrs = new SetupMenu( PROGMEM"AHRS Setup" );
		top->addEntry( ahrs );
		ahrs->addCreator( system_menu_create_hardware_ahrs );
	}

	SetupMenuSelect * pstype = new SetupMenuSelect( PROGMEM"AS Sensor type", RST_ON_EXIT, 0, false, &airspeed_sensor_type );
	top->addEntry( pstype );
	pstype->setHelp( PROGMEM "Factory default for type of pressure sensor, will not erase on factory reset (reboots)");
	pstype->addEntry( PROGMEM"ABPMRR");
	pstype->addEntry( PROGMEM"TE4525");
	pstype->addEntry( PROGMEM"MP5004");
	pstype->addEntry( PROGMEM"Autodetect");

	SetupMenuValFloat * met_adj = new SetupMenuValFloat( PROGMEM"Voltmeter Adjust", "%",	-25.0, 25.0, 0.01, factv_adj, false, &factory_volt_adjust,  RST_NONE, false, true);
	met_adj->setHelp(PROGMEM"Option to factory fine-adjust voltmeter");
	top->addEntry( met_adj );
}

void SetupMenu::system_menu_create_altimeter_airspeed_stallwa( MenuEntry *top ){
	SetupMenuSelect * stawaen = new SetupMenuSelect( PROGMEM"Stall Warning", RST_NONE, 0, false, &stall_warning );
	top->addEntry( stawaen );
	stawaen->setHelp( PROGMEM "Enable alarm sound when speed goes below configured stall speed (until 30% less)");
	stawaen->addEntry( PROGMEM"Disable");
	stawaen->addEntry( PROGMEM"Enable");

	SetupMenuValFloat * staspe = new SetupMenuValFloat( PROGMEM"Stall Speed", "", 20, 200, 1, 0, true, &stall_speed, RST_ON_EXIT  );
	staspe->setHelp(PROGMEM"Configure stalling speed for corresponding aircraft type and reboot (reboots)");
	top->addEntry( staspe );
}

void SetupMenu::system_menu_create_altimeter_airspeed( MenuEntry *top ){
	SetupMenuSelect * als = new SetupMenuSelect( PROGMEM"Altimeter Source", RST_NONE, 0, true, &alt_select );
	top->addEntry( als );
	als->setHelp( PROGMEM "Select source for barometric altitude, either TE sensor or Baro sensor (recommended) or an external source e.g. FLARM (if avail)");
	als->addEntry( PROGMEM"TE Sensor");
	als->addEntry( PROGMEM"Baro Sensor");
	als->addEntry( PROGMEM"External");

	SetupMenuValFloat * spc = new SetupMenuValFloat( PROGMEM"AS Calibration", "%", -100, 100, 1, speedcal_change, false, &speedcal  );
	spc->setHelp(PROGMEM"Calibration of airspeed sensor (AS). Normally not needed, unless pressure probes have systematic error");
	top->addEntry( spc );

	SetupMenuSelect * auze = new SetupMenuSelect( PROGMEM"AutoZero AS Sensor",	RST_IMMEDIATE, 0, true, &autozero );
	top->addEntry( auze );
	auze->setHelp( PROGMEM "Recalculate zero point for airspeed sensor on next power on");
	auze->addEntry( PROGMEM"Cancel");
	auze->addEntry( PROGMEM"Start");

	SetupMenuSelect * alq = new SetupMenuSelect( PROGMEM"Alt. Quantization", RST_NONE,  0, true, &alt_quantization );
	alq->setHelp( PROGMEM "Set altimeter mode with discrete steps and rolling last digits");
	alq->addEntry( PROGMEM"Disable");
	alq->addEntry( PROGMEM"2");
	alq->addEntry( PROGMEM"5");
	alq->addEntry( PROGMEM"10");
	alq->addEntry( PROGMEM"20");
	top->addEntry( alq );

	SetupMenu * stallwa = new SetupMenu( PROGMEM"Stall Warning");
	top->addEntry( stallwa );
	stallwa->setHelp( PROGMEM"Configure stall warning parameters");
	stallwa->addCreator( system_menu_create_altimeter_airspeed_stallwa );

	SetupMenuValFloat * vmax = new SetupMenuValFloat( PROGMEM"Maximum Speed", "", 70, 450, 1, 0, false, &v_max  );
	vmax->setHelp(PROGMEM"Configure maximum speed for corresponding aircraft type");
	top->addEntry( vmax );
}

void SetupMenu::system_menu_create_interfaceS1_routing( MenuEntry *top ){
	SetupMenuSelect * s1outxcv = new SetupMenuSelect( PROGMEM"XCVario", RST_NONE, update_routing, true, &rt_s1_xcv );
	s1outxcv->addEntry( PROGMEM"Disable");
	s1outxcv->addEntry( PROGMEM"Enable");
	top->addEntry( s1outxcv );

	SetupMenuSelect * s1outwl = new SetupMenuSelect( PROGMEM"Wireless", RST_NONE, update_routing, true, &rt_s1_wl );
	s1outwl->addEntry( PROGMEM"Disable");
	s1outwl->addEntry( PROGMEM"Enable");
	top->addEntry( s1outwl );

	SetupMenuSelect * s1outs1 = new SetupMenuSelect( PROGMEM"S2-RS232", RST_NONE, update_routing, true, &rt_s1_s2 );
	s1outs1->addEntry( PROGMEM"Disable");
	s1outs1->addEntry( PROGMEM"Enable");
	top->addEntry( s1outs1 );

	SetupMenuSelect * s1outcan = new SetupMenuSelect( PROGMEM"CAN-bus", RST_NONE, update_routing, true, &rt_s1_can );
	s1outcan->addEntry( PROGMEM"Disable");
	s1outcan->addEntry( PROGMEM"Enable");
	top->addEntry( s1outcan );
}

void SetupMenu::system_menu_create_interfaceS1( MenuEntry *top ){
	SetupMenuSelect * s2sp = new SetupMenuSelect( PROGMEM"Baudraute",	RST_ON_EXIT, 0, true, &serial1_speed );
	top->addEntry( s2sp );
	s2sp->addEntry( PROGMEM"OFF");
	s2sp->addEntry( PROGMEM"4800 baud");
	s2sp->addEntry( PROGMEM"9600 baud");
	s2sp->addEntry( PROGMEM"19200 baud");
	s2sp->addEntry( PROGMEM"38400 baud");
	s2sp->addEntry( PROGMEM"57600 baud");
	s2sp->addEntry( PROGMEM"115200 baud");

	SetupMenuSelect * s1in = new SetupMenuSelect( PROGMEM"Serial Loops", RST_NONE, 0, true, &serial1_rxloop );
	top->addEntry( s1in );
	s1in->setHelp( PROGMEM"Option to loop serial S1 RX to S1 TX, e.g. for unidirectional OV or Kobo connection" );
	s1in->addEntry( PROGMEM"Disable");     // 0
	s1in->addEntry( PROGMEM"Enable");      // 1

	SetupMenu * s1out = new SetupMenu( PROGMEM"S1 Routing");
	top->addEntry( s1out );
	s1out->setHelp( PROGMEM"Select data source to be routed from/to serial interface S1");
	s1out->addCreator( system_menu_create_interfaceS1_routing );

	SetupMenuSelect * stxi = new SetupMenuSelect( PROGMEM"TX Inversion", RST_ON_EXIT , 0, true, &serial1_tx_inverted );
	top->addEntry( stxi );
	stxi->setHelp( PROGMEM"Serial RS232 (TTL) logic, a '1' will be sent as zero voltage level (RS232 standard and default) or vice versa  (reboots)");
	stxi->addEntry( PROGMEM"Normal");
	stxi->addEntry( PROGMEM"Inverted");

	SetupMenuSelect * srxi = new SetupMenuSelect( PROGMEM"RX Inversion", RST_ON_EXIT, 0, true, &serial1_rx_inverted );
	top->addEntry( srxi );
	srxi->setHelp( PROGMEM"Serial RS232 (TTL) logic, a '1' will be received at zero voltage level (RS232 standard and default) or vice versa (reboots)");
	srxi->addEntry( PROGMEM"Normal");
	srxi->addEntry( PROGMEM"Inverted");

	SetupMenuSelect * srxtw1 = new SetupMenuSelect( PROGMEM"Twist RX/TX Pins", RST_ON_EXIT, 0, true, &serial1_pins_twisted );
	top->addEntry( srxtw1 );
	srxtw1->setHelp( PROGMEM"Option to swap RX and TX line for S1, e.g. for OpenVario. After change also a true power-cycle is needed  (reboots)");
	srxtw1->addEntry( PROGMEM"Normal");
	srxtw1->addEntry( PROGMEM"Twisted");

	SetupMenuSelect * stxdis1 = new SetupMenuSelect( PROGMEM"TX Line", RST_ON_EXIT, 0, true, &serial1_tx_enable );
	top->addEntry( stxdis1 );
	stxdis1->setHelp( PROGMEM"Option to switch off RS232 TX line in case active sending is not required, e.g. for multiple devices connected to one device  (reboots)");
	stxdis1->addEntry( PROGMEM"Disable");
	stxdis1->addEntry( PROGMEM"Enable");
}

void SetupMenu::system_menu_create_interfaceS2_routing( MenuEntry *top ){
	SetupMenuSelect * s2outxcv = new SetupMenuSelect( PROGMEM"XCVario", RST_NONE, update_routing, true, &rt_s2_xcv );
	s2outxcv->addEntry( PROGMEM"Disable");
	s2outxcv->addEntry( PROGMEM"Enable");
	top->addEntry( s2outxcv );
	SetupMenuSelect * s2outwl = new SetupMenuSelect( PROGMEM"Wireless", RST_NONE, update_routing, true, &rt_s2_wl );
	s2outwl->addEntry( PROGMEM"Disable");
	s2outwl->addEntry( PROGMEM"Enable");
	top->addEntry( s2outwl );
	SetupMenuSelect * s2outs2 = new SetupMenuSelect( PROGMEM"S1-RS232", RST_NONE, update_routing, true, &rt_s1_s2 );
	s2outs2->addEntry( PROGMEM"Disable");
	s2outs2->addEntry( PROGMEM"Enable");
	top->addEntry( s2outs2 );
	SetupMenuSelect * s2outcan = new SetupMenuSelect( PROGMEM"CAN-bus", RST_NONE, update_routing, true, &rt_s2_can );
	s2outcan->addEntry( PROGMEM"Disable");
	s2outcan->addEntry( PROGMEM"Enable");
	top->addEntry( s2outcan );
}

void SetupMenu::system_menu_create_interfaceS2( MenuEntry *top ){
	SetupMenuSelect * s2sp2 = new SetupMenuSelect( PROGMEM"Baudraute",	RST_ON_EXIT, 0, true, &serial2_speed );
	top->addEntry( s2sp2 );
	// s2sp->setHelp( "Serial RS232 (TTL) speed, pins RX:2, TX:3 on external RJ45 connector");
	s2sp2->addEntry( PROGMEM"OFF");
	s2sp2->addEntry( PROGMEM"4800 baud");
	s2sp2->addEntry( PROGMEM"9600 baud");
	s2sp2->addEntry( PROGMEM"19200 baud");
	s2sp2->addEntry( PROGMEM"38400 baud");
	s2sp2->addEntry( PROGMEM"57600 baud");
	s2sp2->addEntry( PROGMEM"115200 baud");

	SetupMenu * s2out = new SetupMenu( PROGMEM"S2 Routing" );
	s2out->setHelp( PROGMEM"Select data source to be routed from/to serial interface S2");
	top->addEntry( s2out );
	s2out->addCreator( system_menu_create_interfaceS2_routing );

	SetupMenuSelect * stxi2 = new SetupMenuSelect( PROGMEM "TX Inversion", RST_ON_EXIT , 0, true, &serial2_tx_inverted );
	top->addEntry( stxi2 );
	stxi2->setHelp( PROGMEM"Serial RS232 (TTL) logic, a '1' will be sent as zero voltage level (RS232 standard and default) or vice versa  (reboots)");
	stxi2->addEntry( PROGMEM"Normal");
	stxi2->addEntry( PROGMEM"Inverted");

	SetupMenuSelect * srxi2 = new SetupMenuSelect( PROGMEM"RX Inversion", RST_ON_EXIT, 0, true, &serial2_rx_inverted );
	top->addEntry( srxi2 );
	srxi2->setHelp( PROGMEM"Serial RS232 (TTL) logic, a '1' will be received at zero voltage level (RS232 standard and default) or vice versa (reboots)");
	srxi2->addEntry( PROGMEM"Normal");
	srxi2->addEntry( PROGMEM"Inverted");

	SetupMenuSelect * srxtw2 = new SetupMenuSelect( PROGMEM"Twist RX/TX Pins", RST_ON_EXIT, 0, true, &serial2_pins_twisted );
	top->addEntry( srxtw2 );
	srxtw2->setHelp( PROGMEM"Option to swap RX and TX line for S2, e.g. for OpenVario. After change also a true power-cycle is needed (reboots)");
	srxtw2->addEntry( PROGMEM"Normal");
	srxtw2->addEntry( PROGMEM"Twisted");

	SetupMenuSelect * stxdis2 = new SetupMenuSelect( PROGMEM"TX Line", RST_ON_EXIT, 0, true, &serial2_tx_enable );
	top->addEntry( stxdis2 );
	stxdis2->setHelp( PROGMEM"Option to switch off RS232 TX line in case active sending is not required, e.g. for multiple devices connected to one device (reboots)");
	stxdis2->addEntry( PROGMEM"Disable");
	stxdis2->addEntry( PROGMEM"Enable");
}

void SetupMenu::system_menu_create_interfaceCAN_routing( MenuEntry *top ){
	SetupMenuSelect * canoutxcv = new SetupMenuSelect( PROGMEM"XCVario", RST_NONE, 0, true, &rt_can_xcv );
	canoutxcv->addEntry( PROGMEM"Disable");
	canoutxcv->addEntry( PROGMEM"Enable");
	top->addEntry( canoutxcv );

	SetupMenuSelect * canoutwl = new SetupMenuSelect( PROGMEM"Wireless", RST_NONE, 0, true, &rt_wl_can );
	canoutwl->addEntry( PROGMEM"Disable");
	canoutwl->addEntry( PROGMEM"Enable");
	top->addEntry( canoutwl );

	SetupMenuSelect * canouts1 = new SetupMenuSelect( PROGMEM"S1-RS232", RST_NONE, update_routing, true, &rt_s1_can );
	canouts1->addEntry( PROGMEM"Disable");
	canouts1->addEntry( PROGMEM"Enable");
	top->addEntry( canouts1 );

	SetupMenuSelect * canouts2 = new SetupMenuSelect( PROGMEM"S2-RS232", RST_NONE, update_routing, true, &rt_s2_can );
	canouts2->addEntry( PROGMEM"Disable");
	canouts2->addEntry( PROGMEM"Enable");
	top->addEntry( canouts2 );
}

void SetupMenu::system_menu_create_interfaceCAN( MenuEntry *top ){
	SetupMenuSelect * canmode = new SetupMenuSelect( PROGMEM"Datarate", RST_ON_EXIT, 0, true, &can_speed );
	top->addEntry( canmode );
	canmode->setHelp( PROGMEM"Datarate on high speed serial CAN interace in kbit per second (reboots)");
	canmode->addEntry( PROGMEM"CAN OFF");
	canmode->addEntry( PROGMEM"250 kbit");
	canmode->addEntry( PROGMEM"500 kbit");
	canmode->addEntry( PROGMEM"1000 kbit");

	SetupMenu * canrt = new SetupMenu( PROGMEM"CAN Routing" );
	top->addEntry( canrt );
	canrt->setHelp( PROGMEM"Select data source that is routed from/to CAN interface");
	canrt->addCreator( system_menu_create_interfaceCAN_routing );

	SetupMenuSelect * devmod = new SetupMenuSelect( PROGMEM"Mode", RST_ON_EXIT , 0, false, &can_mode );
	top->addEntry( devmod );
	devmod->setHelp( PROGMEM"Select 'Standalone' for single seater, 'Master' in front, 'Client' for secondary device in rear (reboots)");
	devmod->addEntry( PROGMEM"Master");
	devmod->addEntry( PROGMEM"Client");
	devmod->addEntry( PROGMEM"Standalone");
}

void SetupMenu::system_menu_create( MenuEntry *sye ){
	SetupMenu * soft = new SetupMenu( PROGMEM"Software Update" );
	sye->addEntry( soft );
	soft->addCreator(system_menu_create_software);

	SetupMenuSelect * fa = new SetupMenuSelect( PROGMEM"Factory Reset", RST_IMMEDIATE, 0, false, &factory_reset );
	fa->setHelp(PROGMEM"Option to reset all settings to factory defaults, means metric system, 5 m/s vario range and more");
	fa->addEntry( PROGMEM"Cancel");
	fa->addEntry( PROGMEM"ResetAll");
	sye->addEntry( fa );

	SetupMenu * bat = new SetupMenu( PROGMEM"Battery Setup" );
	bat->setHelp( PROGMEM"Adjust corresponding voltage for battery symbol display low,red,yellow and full");
	sye->addEntry( bat );
	bat->addCreator(system_menu_create_battery);

	SetupMenu * hardware = new SetupMenu( PROGMEM"Hardware Setup" );
	hardware->setHelp( PROGMEM"Setup variometer hardware e.g. display, rotary, AS and AHRS sensor, voltmeter, etc", 240 );
	sye->addEntry( hardware );
	hardware->addCreator(system_menu_create_hardware);

	// Altimeter, IAS
	SetupMenu * aia = new SetupMenu( PROGMEM"Altimeter, Airspeed" );
	sye->addEntry( aia );
	aia->addCreator(system_menu_create_altimeter_airspeed);

	// _serial1_speed
	SetupMenu * rs232 = new SetupMenu( PROGMEM"RS232 Interface S1" );
	sye->addEntry( rs232 );
	rs232->addCreator(system_menu_create_interfaceS1);

	if( hardwareRevision.get() >= XCVARIO_21 ) {
		SetupMenu * rs232_2 = new SetupMenu( PROGMEM"RS232 Interface S2" );
		sye->addEntry( rs232_2 );
		rs232_2->addCreator(system_menu_create_interfaceS2);
	}
	if( hardwareRevision.get() >= XCVARIO_22 ){
		// Can Interface C1
		SetupMenu * can = new SetupMenu( PROGMEM"CAN Interface" );
		sye->addEntry( can );
		can->addCreator(system_menu_create_interfaceCAN);
	}

	// NMEA protocol of variometer
	SetupMenuSelect * nmea = new SetupMenuSelect( PROGMEM"NMEA Protocol", RST_NONE , 0, true, &nmea_protocol );
	sye->addEntry( nmea );
	nmea->setHelp( PROGMEM"Setup the protocol used for sending NMEA sentences. This needs to match the device driver chosen in XCSoar/LK8000");
	nmea->addEntry( PROGMEM"OpenVario");
	nmea->addEntry( PROGMEM"Borgelt");
	nmea->addEntry( PROGMEM"Cambridge");
	nmea->addEntry( PROGMEM"XCVario");
	nmea->addEntry( PROGMEM"Disable");
}

void SetupMenu::setup_create_root(MenuEntry *top ){
	ESP_LOGI(FNAME,"setup_create_root()");
	if( rot_default.get() == 0 ) {
		SetupMenuValFloat * mc = new SetupMenuValFloat( PROGMEM"MC", "",	0.0, 9.9, 0.1, 0, true, &MC );
		mc->setHelp(PROGMEM"Mac Cready value for optimum cruise speed or average climb rate, in same unit as the variometer");
		mc->setPrecision(1);
		top->addEntry( mc );
	}
	else {
		SetupMenuValFloat * vol = new SetupMenuValFloat( PROGMEM"Audio Volume", "%", 0.0, 100, 1, vol_adj, true, &audio_volume );
		vol->setHelp(PROGMEM"Audio volume level for variometer tone on internal and external speaker");
		top->addEntry( vol );
	}

	SetupMenuValFloat * bgs = new SetupMenuValFloat( PROGMEM"Bugs", "%", 0.0, 50, 1, bug_adj, true, &bugs  );
	bgs->setHelp(PROGMEM"Percent degradation of gliding performance due to bugs contamination");
	top->addEntry( bgs );

	SetupMenuValFloat * bal = new SetupMenuValFloat( PROGMEM"Ballast", "litre", 0.0, 500, 1, water_adj, true, &ballast_kg  );
	bal->setHelp(PROGMEM"Amount of water ballast added to the over all weight");
	bal->setPrecision(0);
	top->addEntry( bal );

	SetupMenuValFloat * crewball = new SetupMenuValFloat( PROGMEM"Crew Weight", "kg", 0, 300, 1, crew_weight_adj, false, &crew_weight );
	crewball->setPrecision(0);
	crewball->setHelp(PROGMEM"Weight of the pilot(s) including parachute (everything on top of the empty weight apart from ballast)");
	top->addEntry( crewball );

	SetupMenuValFloat *qnh_menu = SetupMenu::createQNHMenu();
	top->addEntry( qnh_menu );

	SetupMenuValFloat * afe = new SetupMenuValFloat( PROGMEM"Airfield Elevation", "", -1, 3000, 1, 0, true, &elevation );
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
		SetupMenuValFloat * passw = new SetupMenuValFloat( PROGMEM"Expert Password", "", 0, 1000, 1, 0, false, &password  );
		passw->setPrecision( 0 );
		passw->setHelp( PROGMEM"To exit from student mode enter expert password and restart device after expert password has been set correctly");
		top->addEntry( passw );
	}
	else
	{
		// Vario
		SetupMenu * va = new SetupMenu( PROGMEM"Vario and Speed 2 Fly" );
		top->addEntry( va );
		va->addCreator( vario_menu_create );

		// Audio
		SetupMenu * ad = new SetupMenu( PROGMEM"Audio" );
		top->addEntry( ad );
		ad->addCreator( audio_menu_create );

		// Glider Setup
		SetupMenu * po = new SetupMenu( PROGMEM"Glider Details" );
		top->addEntry( po );
		po->addCreator( glider_menu_create );

		// Options Setup
		SetupMenu * opt = new SetupMenu( PROGMEM"Options" );
		top->addEntry( opt );
		opt->addCreator( options_menu_create );

		// System Setup
		SetupMenu * sy = new SetupMenu( PROGMEM"System" );
		top->addEntry( sy );
		sy->addCreator( system_menu_create );
	}
}

SetupMenuValFloat * SetupMenu::createQNHMenu(){
	SetupMenuValFloat * qnh = new SetupMenuValFloat( PROGMEM"QNH", "", 900, 1100.0, 0.250, qnh_adj, true, &QNH );
	qnh->setHelp(PROGMEM"QNH pressure value from ATC. On ground you may adjust to airfield altitude above MSL", 180 );
	return qnh;
}

void SetupMenu::setup( )
{
	ESP_LOGI(FNAME,"SetupMenu setup()");
	SetupMenu * root = new SetupMenu( PROGMEM"Setup" );
	root->setRoot( root );
	root->addEntry( root );
	// Create static menues
	if( NEED_VOLTAGE_ADJUST && !SetupMenuValFloat::meter_adj_menu ){
		SetupMenuValFloat::meter_adj_menu = new SetupMenuValFloat( PROGMEM"Voltmeter Adjust", "%",	-25.0, 25.0, 0.01, factv_adj, false, &factory_volt_adjust,  RST_IMMEDIATE, false, true);
	}
	setup_create_root( root );
}
