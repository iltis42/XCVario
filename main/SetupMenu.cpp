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

static char rentry[25];
SetupMenuSelect * audio_range_sm = 0;
SetupMenuSelect * mpu = 0;

// Menu for flap setup

std::string vunit;
std::string sunit;

static bool focus = false;


// Compass menu handler
static CompassMenu compassMenuHandler( compass );

void update_vunit_str( int unit ){
	vunit = Units::VarioUnitLong( unit );
}

int update_vunit(SetupMenuSelect * p) {
	update_vunit_str( p->getSelect() );
	return 0;
}

void update_sunit_str( int unit ){
	sunit = Units::AirspeedUnitStr( unit );
}

int update_sunit(SetupMenuSelect * p) {
	update_sunit_str( p->getSelect() );
	return 0;
}

int gload_reset( SetupMenuSelect * p ){
	gload_pos_max.set(0);
	gload_neg_max.set(0);
	return 0;
}

int compass_ena( SetupMenuSelect * p ){
	if( compass_enable.get() )
		compass.begin();
	return 0;
}

int update_s2f_speed(SetupMenuValFloat * p)
{
	Switch::setCruiseSpeed( Units::Airspeed2Kmh( s2f_speed.get() ) );
	return 0;
}

int update_rentry(SetupMenuValFloat * p)
{
	char rentry[25];
	sprintf( rentry, "Variable (%d m/s)", (int)(range.get()) );
	audio_range_sm->updateEntry( rentry, 1 );
	return 0;
}

int update_wifi_power(SetupMenuValFloat * p)
{
	ESP_ERROR_CHECK(esp_wifi_set_max_tx_power( int(wifi_max_power.get()*80.0/100.0) ));
	return 0;
}

int data_mon( SetupMenuSelect * p ){
	ESP_LOGI(FNAME,"data_mon( %d ) ", p->getSelect() );
	if( p->getSelect() != MON_OFF ){
		DM.start();
	}
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
	// ESP_LOGI(FNAME,"qnh_adj");
	float alt=0;
	if( Flarm::validExtAlt() && alt_select.get() == AS_EXTERNAL )
		alt = alt_external + ( (*(p->_value)) - 1013.25)*8.2296;  // correct altitude according to ISA model = 27ft / hPa
	else{
		for( int i=0; i<6; i++ ) {
			bool ok;
			alt += p->_bmp->readAltitude( Units::Qnh( *(p->_value) ), ok );
			sleep(0.01);
		}
		alt = alt/6;
	}

	ESP_LOGI(FNAME,"Setup BA alt=%f QNH=%f hPa", alt, Units::Qnh( *(p->_value) )  );
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	p->ucg->setFont(ucg_font_fub25_hr);
	p->ucg->setPrintPos(1,110);
	String u;
	float altp;
	if( alt_unit.get() == 0 ){ // m
		u = "m";
		altp = alt;
	}
	else {
		u = "ft";
		altp = Units::meters2feet( alt );
	}
	if( qnh_unit.get() == QNH_INHG ){
		p->ucg->setPrintPos(1,150);
	}else
		p->ucg->setPrintPos(1,120);
	p->ucg->printf("%5d %s ", (int)(altp+0.5), u.c_str() );

	if( qnh_unit.get() == QNH_INHG ){
		p->ucg->setPrintPos(40,110);
		p->ucg->printf("%.2f inHg", Units::hPa2inHg( *(p->_value) ));
	}
	p->ucg->setFont(ucg_font_ncenR14_hr);
	xSemaphoreGive(spiMutex );
	return 0;
}

int elev_adj( SetupMenuValFloat * p )
{
	// ESP_LOGI(FNAME,"elev_adj");
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	p->ucg->setFont(ucg_font_fub25_hr);
	p->ucg->setPrintPos(1,120);
	String u;
	float elevp = elevation.get();
	if( alt_unit.get() == 0 ){ // m
		u = "m";
		p->ucg->printf("%4d %s ", (int)(elevp+0.5), u.c_str() );
	}
	else {
		u = "ft";
		elevp = Units::meters2feet(elevp);
		p->ucg->printf("%4d %s ", ((int)((elevp+2.5)/5))*5, u.c_str() );
	}
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

int polar_adj( SetupMenuValFloat * p )
{
	Speed2Fly.change_polar();
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
	Speed2Fly.select_polar();
	glider_type_index.set( Polars::getPolar( glider_type.get() ).index );
	return 0;
}

int bal_adj( SetupMenuValFloat * p )
{
	float loadinc = ((ballast.get() + fixed_ballast.get()) +100.0)/100.0;
	float newwl = polar_wingload.get() * loadinc;
	p->ucg->setFont(ucg_font_fub25_hr);
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	p->ucg->setPrintPos(1,110);
	p->ucg->printf("%0.2f kg/m2  ", newwl);
	p->ucg->setPrintPos(1,150);
	float refw=polar_wingload.get() * polar_wingarea.get();
	float curw=newwl * polar_wingarea.get();
	unsigned int liter=(curw-refw) + 0.5;
	p->ucg->printf("%u liter  ", liter);
	xSemaphoreGive(spiMutex );
	p->ucg->setFont(ucg_font_ncenR14_hr);
	return 0;
}

int bug_adj( SetupMenuValFloat * p ){
	return 0;
}

int mc_adj( SetupMenuValFloat * p )
{
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
	return compassMenuHandler.deviationAction( p );
}

static int compassResetDeviationAction( SetupMenuSelect *p )
{
	return compassMenuHandler.resetDeviationAction( p );
}

static int compassDeclinationAction( SetupMenuValFloat *p )
{
	return compassMenuHandler.declinationAction( p );
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
	if( p->getSelect() == 1 ){ // Start
		compassMenuHandler.sensorCalibrationAction( p );
	}
	p->setSelect( 0 );
	return 0;
}

SetupMenu::SetupMenu() : MenuEntry() {
	highlight = -1;
	_parent = 0;
	y = 0;
	helptext = 0;
	long_pressed = false;
}

SetupMenu::SetupMenu( std::string title ) : MenuEntry() {
	// ESP_LOGI(FNAME,"SetupMenu::SetupMenu( %s ) ", title.c_str() );
	_rotary->attach(this);
	_title = title;
	highlight = -1;
}
SetupMenu::~SetupMenu()
{
	_rotary->detach(this);
}

void SetupMenu::begin( IpsDisplay* display, ESPRotary * rotary, PressureSensor * bmp, AnalogInput *adc ){
	ESP_LOGI(FNAME,"SetupMenu() begin");
	_rotary = rotary;
	_bmp = bmp;
	_display = display;
	ucg = display->getDisplay();
	_adc = adc;
	setup();
	audio_volume.set( default_volume.get() );
}

void SetupMenu::catchFocus( bool activate ){
	focus = activate;
}

void SetupMenu::display( int mode ){
	if( (selected != this) || !inSetup || focus )
		return;
	ESP_LOGI(FNAME,"SetupMenu display( %s)", _title.c_str() );
	clear();
	y=25;
	ESP_LOGI(FNAME,"Title: %s y=%d", selected->_title.c_str(),y );
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setPrintPos(1,y);
	ucg->setFontPosBottom();
	ucg->printf("<< %s",selected->_title.c_str());
	ucg->drawFrame( 1,3,238,25 );
	for (int i=0; i<_childs.size(); i++ ){
		MenuEntry * child = _childs[i];
		ucg->setPrintPos(1,(i+1)*25+25);
		ucg->setColor( COLOR_HEADER_LIGHT );
		ucg->printf("%s",child->_title.c_str());
		if( child->value() ){
			int fl=ucg->getStrWidth( child->_title.c_str());
			ucg->setPrintPos(1+fl,(i+1)*25+25);
			ucg->printf(": ");
			ucg->setPrintPos(1+fl+ucg->getStrWidth( ": " ),(i+1)*25+25);
			ucg->setColor( COLOR_WHITE );
			ucg->setFont(ucg_font_fur14_hf);
			ucg->printf("%s",child->value());
			ucg->setFont(ucg_font_ncenR14_hr);
		}
		ucg->setColor( COLOR_WHITE );
		// ESP_LOGI(FNAME,"Child: %s y=%d",child->_title.c_str() ,y );
	}
	y+=170;
	xSemaphoreGive(spiMutex );
	showhelp( y );
}

void SetupMenu::down(int count){
	if( selected == this && !inSetup ) {
		// ESP_LOGI(FNAME,"root: down");
		if( rot_default.get() == 1) {	 // MC Value
			int mc = (int)(MC.get()*10);
			mc -= count;
			mc = std::max( mc, 0 );
			MC.set( ((float)mc)/10 );
		}
		else{  // Volume
			int vol = (int)audio_volume.get();
			vol -= count;
			vol = std::max( vol, 0 );
			audio_volume.set( vol );
		}
	}
	if( (selected != this) || !inSetup )
		return;
	// ESP_LOGI(FNAME,"down %d %d", highlight, _childs.size() );
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
	if( selected == this && !inSetup ) {
		// ESP_LOGI(FNAME,"root: up");
		if(rot_default.get() == 1) {	 // MC Value
			int mc = (int)(MC.get()*10);
			mc += count;
			mc = std::min( mc, 99 );
			MC.set( ((float)mc)/10 );
		}
		else{  // Volume
			int vol = (int)audio_volume.get();
			vol += count;
			vol = std::min( vol, 100 );
			audio_volume.set( (float)vol );
		}
	}

	if( (selected != this) || !inSetup )
		return;
	// ESP_LOGI(FNAME,"SetupMenu::up %d %d", highlight, _childs.size() );
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

void SetupMenu::showMenu( bool apressed ){
	ESP_LOGI(FNAME,"showMenu() p:%d h:%d parent:%x", pressed, highlight, (int)_parent );
	// default is not pressed, so just display, but we toogle pressed state at the end
	// so next time we either step up to parent,
	if( pressed )
	{
		if( highlight == -1 ) {
			ESP_LOGI(FNAME,"SetupMenu to parent");
			if( _parent != 0 ){
				selected = _parent;
				selected->highlight = -1;
				selected->pressed = true;
			}
		}
		else {
			ESP_LOGI(FNAME,"SetupMenu to child");
			if( (highlight >=0) && (highlight < (int)(_childs.size()) ) ){
				selected = _childs[highlight];
				selected->pressed = false;
			}
		}
	}
	if( (_parent == 0) && (highlight == -1) ) // entering setup menu root
	{
		if( !inSetup )
		{
			inSetup=true;
			ESP_LOGI(FNAME,"Start Setup Menu");
			_display->doMenu(true);
			// _rotary->attach(&menu_rotary_handler); // todo
			delay(200);  // fixme give display task time to finish drawing
		}
		else
		{
			ESP_LOGI(FNAME,"End Setup Menu");
			_display->clear();
			// _rotary->detach(&menu_rotary_handler);
			_display->doMenu(false);
			SetupCommon::commitNow();
			inSetup=false;
		}
	}
	ESP_LOGI(FNAME,"end showMenu()");
}

void SetupMenu::press(){
	if( selected == 0 )
		selected = root;
	if( (selected != this) || focus )
		return;
	if( !inSetup ){
		active_screen++;
		active_screen = active_screen%(menu_screens.get()+1);
		ESP_LOGI(FNAME,"short press() screen=%d", active_screen );
	}
	if( !menu_long_press.get() || inSetup )
		showMenu( true );
	if( pressed )
		pressed = false;
	else
		pressed = true;
}

void SetupMenu::longPress(){
	if( (selected != this) )
		return;
	// ESP_LOGI(FNAME,"longPress()");
	if( menu_long_press.get() )
	 	showMenu( true );
	if( long_pressed ){
		long_pressed = true;
	}
	else{
		long_pressed = false;
	}
}


void SetupMenu::setup( )
{
	ESP_LOGI(FNAME,"SetupMenu setup()");

	update_vunit_str( vario_unit.get() );
	update_sunit_str( ias_unit.get() );

	SetupMenu * root = new SetupMenu( "Setup" );
	MenuEntry* mm = root->addEntry( root );

	SetupMenuValFloat * mc = new SetupMenuValFloat( "MC", nullptr, vunit.c_str(),	0.0, 9.9, 0.1, mc_adj, true, &MC );
	mc->setHelp(PROGMEM"Default Mac Cready value for optimum cruise speed, or average climb rate to be provided in same units as variometer setting");
	mc->setPrecision(1);
	mm->addEntry( mc );

	SetupMenuValFloat * vol = new SetupMenuValFloat( "Audio Volume", 0, "%", 0.0, 100, 1, vol_adj, true, &audio_volume );
	vol->setHelp(PROGMEM"Set audio volume");
	mm->addEntry( vol );

	SetupMenuValFloat::qnh_menu = new SetupMenuValFloat( "QNH Setup", 0, "hPa", 900, 1100.0, 0.250, qnh_adj, true, &QNH );
	SetupMenuValFloat::qnh_menu->setHelp(PROGMEM"Setup QNH pressure value from next ATC. On ground you may adjust to airfield altitude above MSL.", 180 );
	mm->addEntry( SetupMenuValFloat::qnh_menu );

	SetupMenuValFloat * bal = new SetupMenuValFloat( "Ballast", 0, "%", 0.0, 100, 0.2, bal_adj, true, &ballast  );
	bal->setHelp(PROGMEM"Percent wing load increase by ballast");
	bal->setPrecision(1);
	mm->addEntry( bal );

	SetupMenuValFloat * bgs = new SetupMenuValFloat( "Bugs", 0, "%", 0.0, 50, 1, bug_adj, true, &bugs  );
	bgs->setHelp(PROGMEM"Percent of bugs contamination to indicate degradation of gliding performance");
	mm->addEntry( bgs );

	String elev_unit = "m";
	float step = 1;
	if( alt_unit.get() == 1 ){ // ft
		step = 5.0/Units::meters2feet(1);
	}

	SetupMenuValFloat * afe = new SetupMenuValFloat( "Airfield Elevation", 0, "m", -1, 3000, step, elev_adj, true, &elevation );
	afe->setHelp(PROGMEM"Set airfield elevation in meters for QNH auto adjust on ground according to this setting");
	mm->addEntry( afe );

	if( (int)(password.get()) == 271 ) {
		student_mode.set( 0 );
		password.set( 0 );
	}

	if( student_mode.get() )
	{
		SetupMenuValFloat * passw = new SetupMenuValFloat( "Expert Password", 0, "", 0, 1000, 1, 0, false, &password  );
		passw->setPrecision( 0 );
		passw->setHelp( PROGMEM"To exit from student mode enter expert password and restart device after expert password has been set correctly");
		mm->addEntry( passw );
		// Flap::setupMenue( NULL ); // fixme
	}
	else
	{
		// Vario
		SetupMenu * va = new SetupMenu( "Vario and S2F" );
		MenuEntry* vae = mm->addEntry( va );
		SetupMenuValFloat * vga = new SetupMenuValFloat( 	"Range", 0, vunit.c_str(),	1.0, 30.0, 1, update_rentry, true, &range );
		vga->setHelp(PROGMEM"Upper and lower value for Vario graphic display region");
		vga->setPrecision( 0 );
		vae->addEntry( vga );

		SetupMenuSelect *vlogscale = new SetupMenuSelect( "Log. Scale", false, 0, true, &log_scale );
		vlogscale->setHelp(PROGMEM"Use a logarithmic scale to the vario gauge");
		vlogscale->addEntry( "DISABLE" );
		vlogscale->addEntry( "ENABLE" );
		vae->addEntry( vlogscale );

		SetupMenuSelect * vamod = new SetupMenuSelect( 	"Mode", false, 0 , true, &vario_mode );
		vamod->setHelp(PROGMEM"Controls if vario considers polar sink (=Netto), or not (=Brutto), or if Netto vario applies only in Cruise Mode");
		vamod->addEntry( "Brutto");
		vamod->addEntry( "Netto");
		vamod->addEntry( "Cruise-Netto");
		vae->addEntry( vamod );

		SetupMenuSelect * nemod = new SetupMenuSelect( 	"Netto Mode", false, 0 , true, &netto_mode );
		nemod->setHelp(PROGMEM"In 'Relative' mode, also circling sink is considered also called 'Super-Netto' to show climb rate as if you were circling there");
		nemod->addEntry( "Normal");
		nemod->addEntry( "Relative");
		vae->addEntry( nemod );

		SetupMenuSelect * sink = new SetupMenuSelect( 	"Polar Sink", false, 0 , true, &ps_display );
		sink->setHelp(PROGMEM"Show polar sink rate together with TE in Vario bar when Vario is in Brutto Mode (else disabled)");
		sink->addEntry( "DISABLE");
		sink->addEntry( "ENABLE");
		vae->addEntry( sink );

		SetupMenuSelect * ncolor = new SetupMenuSelect( "Needle Color", false, 0 , true, &needle_color );
		ncolor->setHelp(PROGMEM"Choose the color of the vario needle");
		ncolor->addEntry( "White");
		ncolor->addEntry( "Orange");
		ncolor->addEntry( "Red");
		vae->addEntry( ncolor );

		SetupMenu * vdamp = new SetupMenu( "Vario Damping" );
		MenuEntry* vdampm = vae->addEntry( vdamp );

		SetupMenuValFloat * vda = new SetupMenuValFloat( 	"Damping", 0, "sec", 2.0, 10.0, 0.1, 0, false, &vario_delay );
		vda->setHelp(PROGMEM"Response time, time constant of Vario low pass kalman filter");
		vdampm->addEntry( vda );

		SetupMenuValFloat * vdav = new SetupMenuValFloat( 	"Averager", 0, "sec", 2.0, 60.0,	0.1, 0, false, &vario_av_delay );
		vdav->setHelp(PROGMEM"Response time, time constant of digital Average Vario Display");
		vdampm->addEntry( vdav );

		SetupMenu * meanclimb = new SetupMenu( "Mean Climb" );
		meanclimb->setHelp(PROGMEM"Mean Climb or MC recommendation by green/red rhombus displayed in vario scale adjustment");
		MenuEntry* meanclimbm = vae->addEntry( meanclimb );

		SetupMenuValFloat * vccm = new SetupMenuValFloat( "Minimum climb", 0, "m/s",	0.0, 2.0, 0.1, 0, false, &core_climb_min );
		vccm->setHelp(PROGMEM"Minimum climb rate that counts for arithmetic mean climb value");
		meanclimbm->addEntry( vccm );

		SetupMenuValFloat * vcch = new SetupMenuValFloat( "Duration", 0,	"min", 1, 300, 1, 0, false, &core_climb_history );
		vcch->setHelp(PROGMEM"Duration in minutes where samples for mean climb value are regarded, default is last 3 thermals or 45 min");
		meanclimbm->addEntry( vcch );

		SetupMenuValFloat * vcp = new SetupMenuValFloat( "Cycle", 0,	"sec", 10, 60, 1, 0, false, &core_climb_period );
		vcp->setHelp(PROGMEM"Cycle in number of seconds when mean climb value is recalculated, default is last 60 seconds");
		meanclimbm->addEntry( vcp);

		SetupMenu * s2fs = new SetupMenu( "S2F Settings" );
		MenuEntry* s2fse = vae->addEntry( s2fs );

		SetupMenuValFloat * vds2 = new SetupMenuValFloat( "Damping", 0, "sec", 0.10001, 10.0, 0.1, 0, false, &s2f_delay );
		vds2->setHelp(PROGMEM"Time constant of S2F low pass filter");
		s2fse->addEntry( vds2 );

		SetupMenuSelect * blck = new SetupMenuSelect( "Blockspeed", false, 0 , true, &s2f_blockspeed );
		blck->setHelp(PROGMEM"With Blockspeed enabled, vertical movement of airmass or G-load is not considered for speed to fly (S2F) calculation");
		blck->addEntry( "DISABLE");
		blck->addEntry( "ENABLE");
		s2fse->addEntry( blck );

		SetupMenuSelect * s2fmod = new SetupMenuSelect( "S2F Mode", false, 0 , true, &audio_mode );
		s2fmod->setHelp( PROGMEM"Select S2F mode either fixed, or controled by local switch or switch & airspeed combined or from external source" );
		s2fmod->addEntry( "Vario fix");
		s2fmod->addEntry( "Cruise fix");
		s2fmod->addEntry( "Switch");
		s2fmod->addEntry( "AutoSpeed");
		s2fmod->addEntry( "External");
		s2fse->addEntry( s2fmod );

		SetupMenuValFloat * autospeed = new SetupMenuValFloat( "S2F AutoSpeed", 0, sunit.c_str(), 20.0, 250.0, 1.0, update_s2f_speed, false, &s2f_speed );
		s2fse->addEntry( autospeed );
		autospeed->setHelp(PROGMEM"Transition speed when in AutoSpeed mode for audio to change from Vario to S2F mode");

		SetupMenuValFloat * s2fhy = new SetupMenuValFloat( "Hysteresis", 0, sunit.c_str(),	-20, 20, 1, 0, false, &s2f_hysteresis );
		s2fhy->setHelp(PROGMEM"Hysteresis for auto S2F transition at autospeed +- this value");
		s2fse->addEntry( s2fhy );

		SetupMenuSelect * s2fnc = new SetupMenuSelect( "Arrow Color", false, 0 , true, &s2f_arrow_color );
		s2fnc->setHelp( PROGMEM"Select color of the S2F arrow's when painted in Up/Down position" );
		s2fnc->addEntry( "White/White");
		s2fnc->addEntry( "Blue/Blue");
		s2fnc->addEntry( "Green/Red");
		s2fse->addEntry( s2fnc );


		SetupMenu * elco = new SetupMenu( "Electronic Compensation" );
		vae->addEntry( elco );
		SetupMenuSelect * enac = new SetupMenuSelect( "eCompensation", false, 0 , false, &te_comp_enable );
		enac->setHelp(PROGMEM"Enable/Disable electronic TE compensation option; Enable only when TE pressure is connected to ST (static) pressure");
		enac->addEntry( "DISABLE");
		enac->addEntry( "ENABLE");
		elco->addEntry( enac );

		SetupMenuValFloat * elca = new SetupMenuValFloat( "Adjustment", 0, "%",	-100, 100, 0.1, 0, false, &te_comp_adjust );
		elca->setHelp(PROGMEM"Adjustment option for electronic compensation in %. This affects in % the energy altitude calculated from airspeed");
		elco->addEntry( elca );

		// Audio
		SetupMenu * ad = new SetupMenu( "Audio" );
		MenuEntry* audio = mm->addEntry( ad );

		SetupMenuValFloat * dv = new SetupMenuValFloat( "Default Volume", 0, "%", 0, 100, 1.0, 0, false, &default_volume );
		audio->addEntry( dv );
		dv->setHelp(PROGMEM"Default volume for Audio when device is switched on");

		SetupMenuValFloat * mv = new SetupMenuValFloat( "Max Volume", 0, "%", 0, 100, 1.0, 0, false, &max_volume );
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

		SetupMenuValFloat * cf = new SetupMenuValFloat( "CenterFreq", 0,	"Hz", 200.0, 2000.0, 10.0, 0, false, &center_freq );
		cf->setHelp(PROGMEM"Center frequency for Audio at zero Vario or zero S2F delta");
		audios->addEntry( cf );

		SetupMenuValFloat * oc = new SetupMenuValFloat( "Octaves", 0, "fold", 1.5, 4, 0.1, 0, false, &tone_var );
		oc->setHelp(PROGMEM"Maximum tone frequency variation");
		audios->addEntry( oc );

		SetupMenuSelect * dt = new SetupMenuSelect( "Dual Tone", false, 0 , true, &dual_tone );
		dt->setHelp(PROGMEM"Select dual tone modue aka ilec sound, (di/da/di) or single tone (di/di/di) mode");
		dt->addEntry( "Disable");       // 0
		dt->addEntry( "Enable");        // 1
		audios->addEntry( dt );

		SetupMenuValFloat * htv = new SetupMenuValFloat( "Dual Tone Pich", 0, "%", 0, 50, 1.0, 0, false, &high_tone_var );
		htv->setHelp(PROGMEM"Tone variation in Dual Tone mode, percent of frequency pitch up for second tone");
		audios->addEntry( htv );

		SetupMenuSelect * tch = new SetupMenuSelect( "Chopping", false, eval_chop, true, &chopping_mode );
		tch->setHelp(PROGMEM"Select tone chopping option on positive values for Vario and or S2F");
		tch->addEntry( "Disabled");             // 0
		tch->addEntry( "Vario only");           // 1
		tch->addEntry( "S2F only");             // 2
		tch->addEntry( "Vario and S2F");        // 3  default
		audios->addEntry( tch );

		SetupMenuSelect * tchs = new SetupMenuSelect( "Chopping Style", false, 0 , true, &chopping_style );
		tchs->setHelp(PROGMEM"Select style of tone chopping either hard, or soft with fadein/fadeout");
		tchs->addEntry( "Soft");              // 0  default
		tchs->addEntry( "Hard");              // 1
		audios->addEntry( tchs );

		SetupMenuSelect * advarto = new SetupMenuSelect( "Variable Tone", false, 0 , true, &audio_variable_frequency );
		advarto->setHelp(PROGMEM"Option to enable audio frequency updates within climbing tone intervals, disable keeps frequency constant");
		advarto->addEntry( "Disable");       // 0
		advarto->addEntry( "Enable");        // 1
		audios->addEntry( advarto );

		SetupMenuSelect * ar = new SetupMenuSelect( "Range", false, 0 , true, &audio_range  );
		audio_range_sm = ar;
		sprintf( rentry, "Variable (=%d m/s)", (int)(range.get()) );
		ar->addEntry( "Fix 5 m/s");
		ar->addEntry( "Fix 10 m/s");
		ar->addEntry( rentry );
		ar->setHelp(PROGMEM"Select either fixed or variable Audio range according to current Vario setting");
		audio->addEntry( ar );

		SetupMenu * db = new SetupMenu( "Deadbands" );
		MenuEntry* dbe = audio->addEntry( db );
		dbe->setHelp(PROGMEM"Audio dead band limits within Audio remains silent in metric scale. 0,1 m/s equals roughly 20 ft/min or 0.2 knots");

		SetupMenuValFloat * dbminlv = new SetupMenuValFloat( "Lower Vario", 0,	"m/s", -5.0, 0, 0.1, 0 , false, &deadband_neg  );
		dbminlv->setHelp(PROGMEM"Lower deadband limit (sink) for Audio mute function when in Vario mode");
		dbe->addEntry( dbminlv );

		SetupMenuValFloat * dbmaxlv = new SetupMenuValFloat( "Upper Vario", 0,	"m/s", 0, 5.0, 0.1, 0 , false, &deadband );
		dbmaxlv->setHelp(PROGMEM"Upper deadband limit (climb) for Audio mute function when in Vario mode");
		dbe->addEntry( dbmaxlv );

		SetupMenuValFloat * dbmaxls2fn = new SetupMenuValFloat(	"Lower S2F", 	0, "km/h", -25.0, 0, 1, 0 , false, &s2f_deadband_neg );
		dbmaxls2fn->setHelp(PROGMEM"Negative deadband limit in speed (too slow) deviation when in S2F mode");
		dbe->addEntry( dbmaxls2fn );

		SetupMenuValFloat * dbmaxls2f = new SetupMenuValFloat( "Upper S2F", 	0, "km/h", 0, 25.0, 1, 0 , false, &s2f_deadband );
		dbmaxls2f->setHelp(PROGMEM"Positive deadband limit in speed (too high) deviation when in S2F mode");
		dbe->addEntry( dbmaxls2f );

		SetupMenuValFloat * afac = new SetupMenuValFloat( 	"Audio Exponent", 	0, "", 0.1, 2, 0.025, 0 , false, &audio_factor );
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

		SetupMenuValFloat * frqr = new SetupMenuValFloat( "Frequency Response", 0,	"%", -70.0, 70.0, 1.0, 0, false, &frequency_response );
		frqr->setHelp(PROGMEM"Setup frequency response, double frequency will be attenuated by the factor given, half frequency will be amplified");
		audio->addEntry( frqr );

		// Polar Setup
		SetupMenu * po = new SetupMenu( "Polar" );
		po->setHelp( PROGMEM"Polar setup to match performance of glider");
		MenuEntry* poe = mm->addEntry( po );

		SetupMenuSelect * glt = new SetupMenuSelect( "Glider-Type",	false, polar_select, true, &glider_type );
		poe->addEntry( glt );

		ESP_LOGI(FNAME, "Number of Polars installed: %d", Polars::numPolars() );
		for( int x=0; x< Polars::numPolars(); x++ ){
			glt->addEntry( Polars::getPolar(x).type );
		}

		SetupMenu * pa = new SetupMenu( "PolarAdjust" );
		pa->setHelp(PROGMEM "Adjust Polar at 3 points of selected polar in commonly used metric system for Polars", 230 );
		poe->addEntry( pa );

		SetupMenuValFloat * wil = new SetupMenuValFloat( "Wingload", 0, "kg/m2", 10.0, 100.0, 0.1, polar_adj, false, &polar_wingload );
		wil->setHelp(PROGMEM"Wingload that corresponds to the 3 value pairs for speed/sink of polar");
		pa->addEntry( wil );
		SetupMenuValFloat * pov1 = new SetupMenuValFloat( "Speed 1", 0, "km/h", 50.0, 120.0, 1, polar_adj, false, &polar_speed1);
		pov1->setHelp(PROGMEM"Speed 1, near to minimum sink from polar e.g. 80 km/h");
		pa->addEntry( pov1 );
		SetupMenuValFloat * pos1 = new SetupMenuValFloat( "Sink  1", 0, "m/s", -3.0, 0.0, 0.01, polar_adj, false, &polar_sink1 );
		pos1->setHelp(PROGMEM"Sink indication at Speed 1 from polar");
		pa->addEntry( pos1 );
		SetupMenuValFloat * pov2 = new SetupMenuValFloat( "Speed 2", 0, "km/h", 70.0, 180.0, 1, polar_adj, false, &polar_speed2 );
		pov2->setHelp(PROGMEM"Speed 2 for a moderate cruise from polar e.g. 120 km/h");
		pa->addEntry( pov2 );
		SetupMenuValFloat * pos2 = new SetupMenuValFloat( "Sink  2", 0, "m/s", -5.0, 0.0, 0.01, polar_adj, false, &polar_sink2 );
		pos2->setHelp(PROGMEM"Sink indication at Speed 2 from polar");
		pa->addEntry( pos2 );
		SetupMenuValFloat * pov3 = new SetupMenuValFloat( "Speed 3", 0, "km/h", 100.0, 250.0, 1, polar_adj, false, &polar_speed3 );
		pov3->setHelp(PROGMEM"Speed 3 for a fast cruise from polar e.g. 170 km/h");
		pa->addEntry( pov3 );
		SetupMenuValFloat * pos3 = new SetupMenuValFloat( "Sink  3", 0, "m/s", -6.0, 0.0, 0.01, polar_adj, false, &polar_sink3 );
		pos3->setHelp(PROGMEM"Sink indication at Speed 3 from polar");
		pa->addEntry( pos3 );

		SetupMenuValFloat * maxbal = new SetupMenuValFloat(	"Max Ballast", 0, "liters", 0, 500, 1, 0, false, &polar_max_ballast );
		maxbal->setHelp(PROGMEM"Maximum water ballast for selected glider to allow sync from XCSoar using fraction of max ballast");
		poe->addEntry( maxbal );

		SetupMenuValFloat * wingarea = new SetupMenuValFloat( "Wing Area", 0, "m2", 0, 50, 0.1, 0, false, &polar_wingarea );
		wingarea->setHelp(PROGMEM"Wingarea for the selected glider, to allow adjustments to support wing extensions or new types in square meters");
		poe->addEntry( wingarea );

		SetupMenuValFloat * fixball = new SetupMenuValFloat( "Fixed Ballast", 0, "%", 0, 100, 0.2, bal_adj, false, &fixed_ballast );
		fixball->setPrecision(1);
		fixball->setHelp(PROGMEM"Add here fixed a fixed weight to your glider, once empty glider is more heavy than given the reference polar wingload");
		poe->addEntry( fixball );

		SetupMenu * opt = new SetupMenu( "Options" );

		mm->addEntry( opt );
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
		SetupMenuSelect * alu = new SetupMenuSelect( "Altimeter", false,  0, true, &alt_unit );
		alu->addEntry( "Meter (m)");
		alu->addEntry( "Foot  (ft)");
		alu->addEntry( "FL    (FL)");
		un->addEntry( alu );

		SetupMenuSelect * iau = new SetupMenuSelect( "Airspeed", false , update_sunit, true, &ias_unit );
		iau->addEntry( "Kilom. (Km/h)");
		iau->addEntry( "Miles  (mph)");
		iau->addEntry( "Knots  (kt)");
		un->addEntry( iau );
		SetupMenuSelect * vau = new SetupMenuSelect( "Vario", false , update_vunit, true, &vario_unit );
		vau->addEntry( "Meters/sec (m/s)");
		vau->addEntry( "100ft/min (cft/min)");
		vau->addEntry( "Knots     (knots)");
		un->addEntry( vau );
		SetupMenuSelect * teu = new SetupMenuSelect( "Temperature", false , 0, true, &temperature_unit );
		teu->addEntry( "Celcius");
		teu->addEntry( "Fahrenheit");
		teu->addEntry( "Kelvin");
		un->addEntry( teu );
		SetupMenuSelect * qnhi = new SetupMenuSelect( "QNH", false, 0, true, &qnh_unit );
		un->addEntry( qnhi );
		qnhi->addEntry( "Hectopascal");
		qnhi->addEntry( "InchMercury");


		SetupMenuSelect * amode = new SetupMenuSelect( "Airspeed Mode",	false, 0, true, &airspeed_mode );
		opt->addEntry( amode );
		amode->setHelp( PROGMEM "Select mode of Airspeed indicator to display IAS (Indicated AirSpeed, default) or TAS (True AirSpeed) considering air density", 130 );
		amode->addEntry( "IAS");
		amode->addEntry( "TAS");

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

		SetupMenuValFloat * tral = new SetupMenuValFloat( "Transition Altitude", 0, "FL", 0, 400, 10, 0, false, &transition_alt  );
		tral->setHelp(PROGMEM"Transition altitude (or transition height, when using QFE) is the altitude/height above which standard pressure (QNE) is set (1013.2 mb/hPa)", 100 );
		opt->addEntry( tral );

		SetupMenu * flarm = new SetupMenu( "FLARM" );
		opt->addEntry( flarm );
		flarm->setHelp( PROGMEM "Option to display FLARM Warnings depending on FLARM alarm level");

		SetupMenuSelect * flarml = new SetupMenuSelect( "Alarm Level",	false, 0, true, &flarm_warning );
		flarm->addEntry( flarml );
		flarml->setHelp( PROGMEM "Enable FLARM Alarm level 1 is lowest with 13-18 sec, 2 medium 9-12 sec and 3 highest with 0-8 sec until impact");
		flarml->addEntry( "Disable");
		flarml->addEntry( "Level 1");
		flarml->addEntry( "Level 2");
		flarml->addEntry( "Level 3");

		SetupMenuValFloat * flarmv = new SetupMenuValFloat("Alarm Volume",  0, "%", 20, 125, 1, 0, false, &flarm_volume  );
		flarmv->setHelp(PROGMEM "Maximum volume FLARM alarm audio warning");
		flarm->addEntry( flarmv );

		SetupMenuSelect * flarms = new SetupMenuSelect( "FLARM Simulation",	false, 0, true, &flarm_sim );
		flarm->addEntry( flarms );
		flarms->setHelp( PROGMEM "Simulate an airplane crossing from left to right with different alarm levels and vertical distance 5 seconds after pressed (leave setup!)");
		flarms->addEntry( "Disable");
		flarms->addEntry( "Start Sim");

		SetupMenu * compassWindMenu = new SetupMenu( "Compass/Wind" );
		compassWindMenu->setHelp( PROGMEM "Setup Compass and Wind", 280 );
		MenuEntry* compassWindME = opt->addEntry( compassWindMenu );

		SetupMenu * compassMenu = new SetupMenu( "Compass" );
		MenuEntry* compassME = compassWindME->addEntry( compassMenu );


		SetupMenuSelect * compSensor = new SetupMenuSelect( "Sensor Option", false, compass_ena, true, &compass_enable);
		compSensor->addEntry( "Disable");
		compSensor->addEntry( "Enable I2C");
		compSensor->addEntry( "Enable I2C, no Tilt Corr.");
		compSensor->addEntry( "Enable CAN Sensor");

		compSensor->setHelp( PROGMEM "Option to enable/disable the Compass Sensor" );
		compassME->addEntry( compSensor );


		SetupMenuSelect * compSensorCal = new SetupMenuSelect( "Sensor Calibration", false, compassSensorCalibrateAction, false );
		compSensorCal->addEntry( "Cancel");
		compSensorCal->addEntry( "Start");
		compSensorCal->setHelp( PROGMEM "Calibrate Magnetic Sensor, mandatory for operation" );
		compassME->addEntry( compSensorCal );

		SetupMenuValFloat *cd = new SetupMenuValFloat( "Setup Declination",
				0,
				"\260",
				-180,
				180,
				1.0,
				compassDeclinationAction,
				false,
				&compass_declination );

		cd->setHelp( PROGMEM "Set compass declination in degrees" );
		compassME->addEntry( cd );

		SetupMenuSelect * devMenuA = new SetupMenuSelect( "AutoDeviation", false, 0, true, &compass_dev_auto );
		devMenuA->setHelp( "Automatic adaptive deviation and precise airspeed evaluation method using data from circling wind");
		devMenuA->addEntry( "Disable");
		devMenuA->addEntry( "Enable");
		compassME->addEntry( devMenuA );

		SetupMenu * devMenu = new SetupMenu( "Setup Deviations" );
		devMenu->setHelp( "Compass Deviations", 280 );
		MenuEntry* dme = compassMenu->addEntry( devMenu );

		// Calibration menu is requested
		const short skydirs[8] =
		{ 0, 45, 90, 135, 180, 225, 270, 315 };

		for( int i = 0; i < 8; i++ )
		{
			char buffer[20];
			SetupMenuSelect* sms = new SetupMenuSelect( "Direction ",
					false,
					compassDeviationAction,
					false,
					0 );

			sms->setHelp( "Push button to start deviation action" );
			sprintf( buffer, "%03d", skydirs[i] );
			sms->addEntry( buffer );
			dme->addEntry( sms );
		}

		// Show comapss deviations
		DisplayDeviations* smd = new DisplayDeviations( "Show Deviations" );
		compassME->addEntry( smd );

		SetupMenuSelect* sms = new SetupMenuSelect( "Reset Deviations ",
				false,
				compassResetDeviationAction,
				false,
				0 );

		sms->setHelp( "Reset all deviation data to zero" );
		sms->addEntry( "Cancel" );
		sms->addEntry( "Reset" );
		compassME->addEntry( sms );

		SetupMenu * nmeaMenu = new SetupMenu( "Setup NMEA" );
		compassME->addEntry( nmeaMenu );

		SetupMenuSelect * nmeaHdm = new SetupMenuSelect( "Magnetic Heading", false, 0, true, &compass_nmea_hdm );
		nmeaHdm->addEntry( "Disable");
		nmeaHdm->addEntry( "Enable");
		nmeaHdm->setHelp( PROGMEM "Enable/disable NMEA '$HCHDM' sentence generation for magnetic heading" );
		nmeaMenu->addEntry( nmeaHdm );

		SetupMenuSelect * nmeaHdt = new SetupMenuSelect( "True Heading", false, 0, true, &compass_nmea_hdt );
		nmeaHdt->addEntry( "Disable");
		nmeaHdt->addEntry( "Enable");
		nmeaHdt->setHelp( PROGMEM "Enable/disable NMEA '$HCHDT' sentence generation for true heading" );
		nmeaMenu->addEntry( nmeaHdt );

		SetupMenuValFloat * compdamp = new SetupMenuValFloat( "Damping", 0, "sec", 0.1, 10.0, 0.1, 0, false, &compass_damping );
		compdamp->setPrecision(1);
		compassME->addEntry( compdamp );
		compdamp->setHelp(PROGMEM "Compass or magnetic heading damping factor in seconds");

		SetupMenuValFloat * compi2c = new SetupMenuValFloat( "I2C Clock", 0, "KHz", 10.0, 400.0, 10, 0, false, &compass_i2c_cl, true );
		compassME->addEntry( compi2c );
		compi2c->setHelp(PROGMEM "Setup compass I2C Bus clock in KHz");

		// Show compass settings
		ShowCompassSettings* scs = new ShowCompassSettings( "Show Settings" );
		compassME->addEntry( scs );

		// Wind speed observation window
		SetupMenuSelect * windcal = new SetupMenuSelect( "Wind Calculation", false, 0, true, &wind_enable );
		windcal->addEntry( "Disable");
		windcal->addEntry( "Straight");
		windcal->addEntry( "Circling");
		windcal->addEntry( "Both");
		windcal->setHelp(PROGMEM "Enable Wind calculation for straight flight (needs compass), circling or both and display wind in reto display style");
		compassWindME->addEntry( windcal );

		// Display option
		SetupMenuSelect * winddis = new SetupMenuSelect( "Display", false, 0, true, &wind_display );
		winddis->addEntry( "Disable");
		winddis->addEntry( "Wind Digits");
		winddis->addEntry( "Wind Arrow");
		winddis->addEntry( "Wind Both");
		winddis->addEntry( "Compass");
		winddis->setHelp( PROGMEM "Control what is to be displayed, either as digits or by arrow or both on retro style screen");
		compassWindME->addEntry( winddis );

		// Wind speed observation window
		SetupMenuSelect * windref = new SetupMenuSelect( "Arrow Ref", false, 0, true, &wind_reference );
		windref->addEntry( "North");
		windref->addEntry( "Mag Heading");
		windref->addEntry( "GPS Course");
		windref->setHelp( PROGMEM "Wind arrow related either to geographic north or related to true airplane heading");
		compassWindME->addEntry( windref );

		SetupMenu * strWindM = new SetupMenu( "Straight Wind" );
		compassWindME->addEntry( strWindM );
		strWindM->setHelp( PROGMEM "Straight flight wind calculation needs compass module active", 250 );

		SetupMenuValFloat *smdev = new SetupMenuValFloat( "Deviation tolerance", nullptr, "\xb0", 0.0, 180.0, 1.0,	nullptr, false, &wind_max_deviation );
		smdev->setHelp( PROGMEM "Setup maximum deviation accepted for a wind measurement" );
		strWindM->addEntry( smdev );

		SetupMenuValFloat *smgsm = new SetupMenuValFloat( "Airspeed Lowpass", nullptr, "", 0, 1.0, 0.001, nullptr, false, &wind_as_filter );
		smgsm->setPrecision(3);
		strWindM->addEntry( smgsm );
		smgsm->setHelp(PROGMEM "Lowpass factor for airspeed correction from reverse wind calculation");

		SetupMenuValFloat *devlp = new SetupMenuValFloat( "Deviation Lowpass", nullptr, "", 0, 1.0, 0.001, nullptr, false, &wind_dev_filter );
		devlp->setPrecision(3);
		strWindM->addEntry( devlp );
		devlp->setHelp(PROGMEM "Lowpass factor for deviation table correction from reverse wind calculation");

		SetupMenuValFloat *wlpf = new SetupMenuValFloat( "Averager", nullptr, "", 5, 300, 1, nullptr, false, &wind_filter_lowpass );
		wlpf->setPrecision(0);
		strWindM->addEntry( wlpf );
		wlpf->setHelp(PROGMEM "Number of measurements used for straight flight live wind averager");

		SetupMenuValFloat *smgps = new SetupMenuValFloat( "GPS Lowpass", nullptr, "sec", 0.1, 10.0, 0.1, nullptr, false, &wind_gps_lowpass );
		smgps->setPrecision(1);
		strWindM->addEntry( smgps );
		smgps->setHelp(PROGMEM "Lowpass factor for GPS info TC and GS, should correlate with compass lowpass less GPS latency");


		ShowStraightWind* ssw = new ShowStraightWind( "Straight Wind Status" );
		strWindM->addEntry( ssw );

		//		sms = new SetupMenuSelect( "Reset",	false, windResetAction, false, 0 );
		//		sms->setHelp( "Reset all wind data to defaults" );
		//		sms->addEntry( "Cancel" );
		//		sms->addEntry( "Reset" );
		//		strWindM->addMenu( sms );

		SetupMenu * cirWindM = new SetupMenu( "Circling Wind" );
		compassWindME->addEntry( cirWindM );

		// Show Circling Wind Status
		ShowCirclingWind* scw = new ShowCirclingWind( "Circling Wind Status" );
		cirWindM->addEntry( scw );

		SetupMenuValFloat *cirwd = new SetupMenuValFloat( "Max Delta", nullptr, "\260", 0, 90.0, 1.0, nullptr, false, &max_circle_wind_diff );
		cirWindM->addEntry( cirwd );
		cirwd->setHelp(PROGMEM "Maximum accepted delta accepted value for heading error in circling wind calculation");


		SetupMenuValFloat *cirlp = new SetupMenuValFloat( "Averager", nullptr, "", 1, 10, 1, nullptr, false, &circle_wind_lowpass );
		cirlp->setPrecision(0);
		cirWindM->addEntry( cirlp );
		cirlp->setHelp(PROGMEM "Number of circles used for circling wind averager. A value of 1 means no average");


		SetupMenuSelect * windlog = new SetupMenuSelect( "Wind Logging", false, 0, true, &wind_logging );
		windlog->addEntry( "Disable");
		windlog->addEntry( "Enable");
		windlog->setHelp(PROGMEM "Enable Wind logging NMEA output to WIFI port 8882");
		compassWindME->addEntry( windlog );


		SetupMenu * wireless = new SetupMenu( "Wireless" );
		MenuEntry* wirelessM = opt->addEntry( wireless );

		SetupMenuSelect * btm = new SetupMenuSelect( "Wireless", true, 0, true, &wireless_type );
		btm->setHelp( PROGMEM "Activate wireless interface type to connect navigation devices, or to another XCVario as client");
		btm->addEntry( "Disable");
		btm->addEntry( "Bluetooth");
		btm->addEntry( "Wireless Master");
		btm->addEntry( "Wireless Client");
		btm->addEntry( "Wireless Standalone");
		// btm->addEntry( "Wireless Master");
		wirelessM->addEntry( btm );

		SetupMenuValFloat *wifip = new SetupMenuValFloat( "WIFI Power", 0, "%", 10.0, 100.0, 5.0, update_wifi_power, false, &wifi_max_power );
		wifip->setPrecision(0);
		wirelessM->addEntry( wifip );
		wifip->setHelp(PROGMEM "Maximum Wifi Power to be used 10..100% or 2..20dBm");

		SetupMenuSelect * wifimal = new SetupMenuSelect( "Lock Master", false, master_xcv_lock, true, &master_xcvario_lock );
		wifimal->setHelp( PROGMEM "In wireless client role, lock the scanned master XCVario ID above to this client");
		wifimal->addEntry( "Unlock");
		wifimal->addEntry( "Lock");
		wirelessM->addEntry( wifimal );

		SetupMenuSelect * datamon = new SetupMenuSelect( "Monitor", false, data_mon, true, &data_monitor );
		datamon->setHelp( PROGMEM "Short press button to pause, long press for stop data monitor", 260);
		datamon->addEntry( "Disable");
		datamon->addEntry( "Bluetooth");
		datamon->addEntry( "Wifi 8880");
		datamon->addEntry( "Wifi 8881");
		datamon->addEntry( "Wifi 8882");
		datamon->addEntry( "S1");
		datamon->addEntry( "S2");
		datamon->addEntry( "CAN");
		wirelessM->addEntry( datamon );

		SetupMenu * gload = new SetupMenu( "G-Load Display" );
		MenuEntry* gloadME = opt->addEntry( gload );

		SetupMenuSelect * glmod = new SetupMenuSelect( "Activation Mode", false, 0, true, &gload_mode );
		glmod->setHelp( PROGMEM "Switch off G-Force screen, or activate G-Force screen by threshold 'Dynamic', or static by 'Always-On'");
		glmod->addEntry( "Off");
		glmod->addEntry( "Dynamic");
		glmod->addEntry( "Always-On");
		gloadME->addEntry( glmod );

		SetupMenuValFloat * gtpos = new SetupMenuValFloat( "Positive Threshold", 0, "", 1.0, 8.0, 0.1, 0, false, &gload_pos_thresh );
		gloadME->addEntry( gtpos );
		gtpos->setHelp(PROGMEM "Positive threshold to launch G-Load display");

		SetupMenuValFloat * gtneg = new SetupMenuValFloat( "Negative Threshold", 0, "", -8.0, 1.0, 0.1, 0, false, &gload_neg_thresh );
		gloadME->addEntry( gtneg );
		gtneg->setHelp(PROGMEM "Negative threshold to launch G-Load display");

		SetupMenuValFloat * glpos = new SetupMenuValFloat( "Positive Limit", 0, "", 1.0, 8.0, 0.1, 0, false, &gload_pos_limit );
		gloadME->addEntry( glpos );
		glpos->setHelp(PROGMEM "Positive g load factor limit the structure of airplane is able to handle according to manual");

		SetupMenuValFloat * glneg = new SetupMenuValFloat( "Negative Limit", 0, "", -8.0, 1.0, 0.1, 0, false, &gload_neg_limit );
		gloadME->addEntry( glneg );
		glneg->setHelp(PROGMEM "Negative g load factor limit the structure of airplane is able to handle according to manual");

		SetupMenuValFloat * gmpos = new SetupMenuValFloat( "Max Positive", 0, "", 0.0, 0.0, 0.0, 0, false, &gload_pos_max );
		gloadME->addEntry( gmpos );
		gmpos->setHelp(PROGMEM "Maximum positive G-Load measured since last reset");

		SetupMenuValFloat * gmneg = new SetupMenuValFloat( "Max Negative", 0, "", 0.0, 0.0, 0.0, 0, false, &gload_neg_max );
		gloadME->addEntry( gmneg );
		gmneg->setHelp(PROGMEM "Maximum negative G-Load measured since last reset");

		SetupMenuSelect * gloadres = new SetupMenuSelect( "G-Load reset", false, gload_reset, false, 0 );
		gloadres->setHelp(PROGMEM "Option to reset stored maximum positive and negative G-load values");
		gloadres->addEntry( "Reset");
		gloadres->addEntry( "Cancel");
		gloadME->addEntry( gloadres );

		SetupMenu * sy = new SetupMenu( "System" );
		MenuEntry* sye = mm->addEntry( sy );

		SetupMenu * soft = new SetupMenu( "Software Update" );
		sye->addEntry( soft );

		Version V;

		SetupMenuSelect * ver = new SetupMenuSelect( "Software Vers.", false, 0, false );
		ver->addEntry( V.version() );
		soft->addEntry( ver );

		SetupMenuSelect * upd = new SetupMenuSelect( "Software Update", true, 0, true, &software_update );
		soft->addEntry( upd );
		upd->setHelp(PROGMEM "Software Update over the air (OTA). Start Wifi AP, then connect to Wifi 'ESP32 OTA' and open http://192.168.0.1 to upload firmware");
		upd->addEntry( "Cancel");
		upd->addEntry( "Start");

		SetupMenuSelect * fa = new SetupMenuSelect( "Factory Reset", true, 0, false, &factory_reset );
		fa->setHelp(PROGMEM "Option to reset all settings to factory defaults, means metric system, 5m/s vario range and more");
		fa->addEntry( "Cancel");
		fa->addEntry( "ResetAll");
		sye->addEntry( fa );

		SetupMenu * bat = new SetupMenu( "Battery Setup" );
		bat->setHelp( PROGMEM "Adjust corresponding voltage for battery symbol display low,red,yellow and full");
		sye->addEntry( bat );

		SetupMenuValFloat * blow = new SetupMenuValFloat( "Battery Low", 0, "Volt ", 0.0, 28.0, 0.1, 0, false, &bat_low_volt );
		SetupMenuValFloat * bred = new SetupMenuValFloat( "Battery Red", 0, "Volt ", 0.0, 28.0, 0.1, 0, false, &bat_red_volt  );
		SetupMenuValFloat * byellow = new SetupMenuValFloat( "Battery Yellow", 0, "Volt ", 0.0, 28.0, 0.1, 0, false, &bat_yellow_volt );
		SetupMenuValFloat * bfull = new SetupMenuValFloat( "Battery Full", 0, "Volt ", 0.0, 28.0, 0.1, 0, false, &bat_full_volt  );

		SetupMenuSelect * batv = new SetupMenuSelect( "Battery Display", false, 0, true, &battery_display  );
		batv->setHelp(PROGMEM "Option to display battery charge state either in Percentage e.g. 75% or Voltage e.g. 12.5V");
		batv->addEntry( "Percentage");
		batv->addEntry( "Voltage");
		batv->addEntry( "Voltage Big");

		bat->addEntry(blow);
		bat->addEntry(bred);
		bat->addEntry(byellow);
		bat->addEntry(bfull);
		bat->addEntry( batv );

		SetupMenu * hardware = new SetupMenu( "Hardware Setup" );
		hardware->setHelp( PROGMEM "Setup variometer hardware like display, rotary, AHRS sensor", 200 );
		sye->addEntry( hardware );

		SetupMenu * display = new SetupMenu( "DISPLAY Setup" );
		hardware->addEntry( display );
		// UNIVERSAL, RAYSTAR_RFJ240L_40P, ST7789_2INCH_12P, ILI9341_TFT_18P
		if( display_type.get() == UNIVERSAL )
		{
			SetupMenuSelect * dtype = new SetupMenuSelect( 	"HW Type", false, 0, true, &display_type );
			dtype->setHelp( PROGMEM "Factory setup for corresponding display type used");
			dtype->addEntry( "UNIVERSAL");
			dtype->addEntry( "RAYSTAR");
			dtype->addEntry( "ST7789");
			dtype->addEntry( "ILI9341");
			display->addEntry( dtype );
		}

		SetupMenuSelect * disty = new SetupMenuSelect( "Style", false , 0, false, &display_style );
		display->addEntry( disty );
		disty->setHelp( PROGMEM "Display style in more digital airliner stype or retro mode with classic vario meter needle");
		disty->addEntry( "Airliner");
		disty->addEntry( "Retro");
		disty->addEntry( "UL");

		SetupMenuSelect * disva = new SetupMenuSelect( "Color Variant", false , 0, false, &display_variant );
		display->addEntry( disva );
		disva->setHelp( PROGMEM "Display variant white on black (W/B) or black on white(B/W)");
		disva->addEntry( "W/B");
		disva->addEntry( "B/W");


		// Orientation   _display_orientation
		SetupMenuSelect * diso = new SetupMenuSelect( "Orientation", true, 0, true, &display_orientation );
		display->addEntry( diso );
		diso->setHelp( PROGMEM "Display Orientation either NORMAL means Rotary is right, or TOPDOWN means Rotary is left");
		diso->addEntry( "NORMAL");
		diso->addEntry( "TOPDOWN");

		SetupMenu * rotary = new SetupMenu( "Rotary Setup" );
		hardware->addEntry( rotary );
		SetupMenuSelect * rotype;
		if( hardwareRevision.get() < 3 )
			rotype = new SetupMenuSelect( "Direction", false , 0, false, &rotary_dir );
		else
			rotype = new SetupMenuSelect( "Direction", false , 0, false, &rotary_dir_21 );
		rotary->addEntry( rotype );
		rotype->setHelp( PROGMEM "Select type of rotary switch, different brands may need adjustment");
		rotype->addEntry( "Clockwise");
		rotype->addEntry( "Counterclockwise");

		SetupMenuSelect * roinc = new SetupMenuSelect( "Sensitivity", false , 0, false, &rotary_inc );
		rotary->addEntry( roinc );
		roinc->setHelp( PROGMEM "Select rotary sensitivity in number of Indent's for one increment, to your personal preference, 1 Indent is most sensitive");
		roinc->addEntry( "1 Indent");
		roinc->addEntry( "2 Indent");
		roinc->addEntry( "3 Indent");
		roinc->addEntry( "4 Indent");

		SetupMenu * rotarya = new SetupMenu( "Rotary Actions" );
		hardware->addEntry( rotarya );
		// Rotary Default
		SetupMenuSelect * rd = new SetupMenuSelect( "Rotation", false, 0, true, &rot_default );
		rotarya->addEntry( rd );
		rd->setHelp(PROGMEM "Select value to be altered at rotary movement outside of setup menu");
		rd->addEntry( "Volume");
		rd->addEntry( "MC Value");

		SetupMenuSelect * sact = new SetupMenuSelect( "Setup Activ.", false, 0, true, &menu_long_press );
		rotarya->addEntry( sact);
		sact->setHelp(PROGMEM "Select Mode to activate setup menu either by short press or long press > 0.4 seconds");
		sact->addEntry( "Short Press");
		sact->addEntry( "Long Press");

		SetupMenuSelect * screens = new SetupMenuSelect( "Screens", false, 0, true, &menu_screens );
		rotarya->addEntry( screens );
		screens->setHelp(PROGMEM "Select screens to be activated one after each other by short press");
		screens->addEntry( "Variometer");
		screens->addEntry( "+ G-Meter");
		/*
				screens->addEntry( "+ Traffic Alert");
				screens->addEntry( "+ Thermal Assistant");
		 */


		SetupMenuSelect * s2fsw = new SetupMenuSelect( "S2F Switch", false , 0, false, &s2f_switch_type );
		hardware->addEntry( s2fsw );
		s2fsw->setHelp( PROGMEM "Select S2F hardware switch type, what can be an normal switch or a push button without lock toggling S2F mode any time pressed");
		s2fsw->addEntry( "Switch");
		s2fsw->addEntry( "Push Button");
		s2fsw->addEntry( "Switch Inverted");


		if( hardwareRevision.get() >= 3 ){
			SetupMenu * ahrs = new SetupMenu( "AHRS Setup" );
			hardware->addEntry( ahrs );
			mpu = new SetupMenuSelect( "AHRS Option", true , 0, true, &attitude_indicator );
			ahrs->addEntry( mpu );
			mpu->setHelp( PROGMEM "Enable High Accuracy Attitude Sensor (AHRS) NMEA messages (need valid license key entered)");
			mpu->addEntry( "Disable");
			if( ahrsKeyValid )
				mpu->addEntry( "Enable");

			SetupMenuSelect * ahrsaz = new SetupMenuSelect( "AHRS Autozero", true , 0, true, &ahrs_autozero );
			ahrs->addEntry( ahrsaz );
			ahrsaz->setHelp( PROGMEM "Start Autozero of AHRS Sensor; Preconditions: On ground; Wings 100% horizontal, fuselage in flight position !");
			ahrsaz->addEntry( "Cancel");
			ahrsaz->addEntry( "Start");

			SetupMenu * ahrslc = new SetupMenu( "AHRS License Key" );
			ahrslc->setHelp( PROGMEM "Enter valid AHRS License Key, then with valid key under 'AHRS Option', AHRS feature can be enabled");
			ahrs->addEntry( ahrslc );

			SetupMenuSelect * ahrslc1 = new SetupMenuSelect( "First    Letter",	false, add_key, false, &ahrs_licence_dig1 );
			SetupMenuSelect * ahrslc2 = new SetupMenuSelect( "Second Letter",	false, add_key, false, &ahrs_licence_dig2 );
			SetupMenuSelect * ahrslc3 = new SetupMenuSelect( "Third   Letter",	false, add_key, false, &ahrs_licence_dig3 );
			SetupMenuSelect * ahrslc4 = new SetupMenuSelect( "Last     Letter",	false, add_key, false, &ahrs_licence_dig4 );
			ahrslc->addEntry( ahrslc1 );
			ahrslc->addEntry( ahrslc2 );
			ahrslc->addEntry( ahrslc3 );
			ahrslc->addEntry( ahrslc4 );
			for( int x=48; x<= 90; x++ ){
				char e[2];
				e[0]=char(x);
				e[1]='\0';
				ahrslc1->addEntry( e );
				ahrslc2->addEntry( e );
				ahrslc3->addEntry( e );
				ahrslc4->addEntry( e );
			}
			SetupMenuValFloat * ahrsgf = new SetupMenuValFloat( "AHRS Gyro", 0, "%", 0, 100, 0.1, 0, false, &ahrs_gyro_factor  );
			ahrsgf->setHelp(PROGMEM"Gyro factor in artifical horizont bank and pitch (more instant movement), zero disables Gyro");
			ahrs->addEntry( ahrsgf );

		}

		SetupMenuSelect * pstype = new SetupMenuSelect( "AS Sensor type", true , 0, false, &airspeed_sensor_type );
		hardware->addEntry( pstype );
		pstype->setHelp( PROGMEM "Factory default for type of pressure sensor, will not erase on factory reset");
		pstype->addEntry( "ABPMRR");
		pstype->addEntry( "TE4525");
		pstype->addEntry( "MP5004");
		pstype->addEntry( "Autodetect");

		float fva = factory_volt_adjust.get();
		if( abs(fva - 0.00815) < 0.00001 ) {
			ESP_LOGI(FNAME,"Add option to Factory adjust ADC; not yet done");
			SetupMenuValFloat * fvoltadj = new SetupMenuValFloat( 	"Factory Voltmeter Adj", 0, "%",	-25.0, 25.0, 0.01, factv_adj, false, &factory_volt_adjust );
			fvoltadj->setHelp(PROGMEM "Option to fine factory adjust voltmeter");
			sye->addEntry( fvoltadj );
		}

		// Altimeter, IAS
		SetupMenu * aia = new SetupMenu( "Altimeter, Airspeed" );
		sye->addEntry( aia );

		SetupMenuSelect * als = new SetupMenuSelect( "Altimeter Source", false, 0, true, &alt_select );
		aia->addEntry( als );
		als->setHelp( PROGMEM "Select source for barometric altitude, either TE sensor or Baro sensor (recommended) or an external source e.g. FLARM (if avail)");
		als->addEntry( "TE Sensor");
		als->addEntry( "Baro Sensor");
		als->addEntry( "External");

		SetupMenuValFloat * spc = new SetupMenuValFloat( "AS Calibration", 0, "%", -60, 60, 1, 0, false, &speedcal  );
		spc->setHelp(PROGMEM"Calibration of airspeed sensor (AS). Normally not needed, hence pressure probes may have systematic error");
		aia->addEntry( spc );

		SetupMenuSelect * auze = new SetupMenuSelect( "AutoZero AS Sensor",	true, 0, true, &autozero );
		aia->addEntry( auze );
		auze->setHelp( PROGMEM "Recalculate zero point for airspeed sensor on next power on");
		auze->addEntry( "Cancel");
		auze->addEntry( "Start");

		SetupMenuSelect * alq = new SetupMenuSelect( "Alt. Quantization", false,  0, true, &alt_quant );
		alq->setHelp( PROGMEM "Set altimeter mode with discrete steps and rolling wheel in the last digits");
		alq->addEntry( "Disable");
		alq->addEntry( "Enable");
		aia->addEntry( alq );

		SetupMenu * stallwa = new SetupMenu( "Stall Warning");
		aia->addEntry( stallwa );
		stallwa->setHelp( PROGMEM "Configure stall warning");

		SetupMenuSelect * stawaen = new SetupMenuSelect( "Stall Warning", false, 0, false, &stall_warning );
		stallwa->addEntry( stawaen );
		stawaen->setHelp( PROGMEM "Enable alarm sound when speed goes below configured stall speed and below (until 30% less)");
		stawaen->addEntry( "Disable");
		stawaen->addEntry( "Enable");

		SetupMenuValFloat * staspe = new SetupMenuValFloat( "Stall Speed", 0, sunit.c_str(), 20, 200, 1, polar_adj, true, &stall_speed  );
		staspe->setHelp(PROGMEM"Configure stalling speed for corresponding airplane type and reboot");
		stallwa->addEntry( staspe );

		SetupMenuValFloat * vmax = new SetupMenuValFloat( "Maximum Speed", 0, sunit.c_str(), 70, 450, 1, 0, false, &v_max  );
		vmax->setHelp(PROGMEM"Configure maximum speed for corresponding airplane type");
		aia->addEntry( vmax );


		// _serial1_speed
		SetupMenu * rs232 = new SetupMenu( "RS232 Interface S1" );
		sye->addEntry( rs232 );
		SetupMenuSelect * s2sp = new SetupMenuSelect( PROGMEM "Baudraute",	true, 0, true, &serial1_speed );
		rs232->addEntry( s2sp );
		// s2sp->setHelp( "Serial RS232 (TTL) speed, pins RX:2, TX:3 on external RJ45 connector");
		s2sp->addEntry( "OFF");
		s2sp->addEntry( "4800 baud");
		s2sp->addEntry( "9600 baud");
		s2sp->addEntry( "19200 baud");
		s2sp->addEntry( "38400 baud");
		s2sp->addEntry( "57600 baud");
		s2sp->addEntry( "115200 baud");

		SetupMenuSelect * s1in = new SetupMenuSelect( PROGMEM "Serial Loops", false, 0, true, &serial1_rxloop );
		rs232->addEntry( s1in );
		s1in->setHelp( "Option to loop serial S1 RX to S1 TX, e.g. for unidirectional OV or Kobo connection" );
		s1in->addEntry( "Disable");     // 0
		s1in->addEntry( "Enable");      // 1

		SetupMenuSelect * s1out = new SetupMenuSelect( PROGMEM "Routing", false, 0, true, &serial1_tx );
		rs232->addEntry( s1out );
		s1out->setHelp( "Select devices routed to serial interface S1");
		s1out->addEntry( "Disable all");
		s1out->addEntry( "XCVario");                // 1    XCVario NMEA Data
		s1out->addEntry( "Wireless-XCSoar");        // 2    XCSoar Data
		s1out->addEntry( "WL-XCSoar, XCV");         // 3
		s1out->addEntry( "S2-XCSoar");              // 4

		SetupMenuSelect * stxi = new SetupMenuSelect( PROGMEM "TX Inversion", true , 0, true, &serial1_tx_inverted );
		rs232->addEntry( stxi );
		stxi->setHelp( "Serial RS232 (TTL) option for negative logic, means a '1' will be sent at zero level (RS232 standard and default) and vice versa");
		stxi->addEntry( "Normal");
		stxi->addEntry( "Inverted");

		SetupMenuSelect * srxi = new SetupMenuSelect( PROGMEM "RX Inversion", true, 0, true, &serial1_rx_inverted );
		rs232->addEntry( srxi );
		srxi->setHelp( "Serial RS232 (TTL) option for negative logic, means a '1' will be received at zero level (RS232 standard and default) and vice versa");
		srxi->addEntry( "Normal");
		srxi->addEntry( "Inverted");

		SetupMenuSelect * srxtw1 = new SetupMenuSelect( PROGMEM "Twist RX/TX Pins", true, 0, true, &serial1_pins_twisted );
		rs232->addEntry( srxtw1 );
		srxtw1->setHelp( "Option to twist RX and TX line for S1, e.g. for OpenVario. After change also a true power-cycle is needed");
		srxtw1->addEntry( "Normal");
		srxtw1->addEntry( "Twisted");

		SetupMenuSelect * stxdis1 = new SetupMenuSelect( PROGMEM "TX Line", true, 0, true, &serial1_tx_enable );
		rs232->addEntry( stxdis1 );
		stxdis1->setHelp( "Option to switch off RS232 TX line in case active sending is not required, e.g. for multiple devices connected to one device");
		stxdis1->addEntry( "Disable");
		stxdis1->addEntry( "Enable");


		if( hardwareRevision.get() >= 3 ) {
			SetupMenu * rs232_2 = new SetupMenu( "RS232 Interface S2" );
			sye->addEntry( rs232_2 );
			SetupMenuSelect * s2sp2 = new SetupMenuSelect( PROGMEM "Baudraute",	true, 0, true, &serial2_speed );
			rs232_2->addEntry( s2sp2 );
			// s2sp->setHelp( "Serial RS232 (TTL) speed, pins RX:2, TX:3 on external RJ45 connector");
			s2sp2->addEntry( "OFF");
			s2sp2->addEntry( "4800 baud");
			s2sp2->addEntry( "9600 baud");
			s2sp2->addEntry( "19200 baud");
			s2sp2->addEntry( "38400 baud");
			s2sp2->addEntry( "57600 baud");
			s2sp2->addEntry( "115200 baud");

			SetupMenuSelect * s1out2 = new SetupMenuSelect( PROGMEM "Routing", false, 0, true, &serial2_tx );
			rs232_2->addEntry( s1out2 );
			s1out2->setHelp( "Select devices routed to serial interface S2", 220 );
			s1out2->addEntry( "Disable all");
			s1out2->addEntry( "XCVario");                       // 1    XCVario NMEA Data bidir
			s1out2->addEntry( "Wireless-XCSoar");               // 2    XCSoar Data bidir
			s1out2->addEntry( "WL-XCSoar, XCV");            // 3
			s1out2->addEntry( "S1");                         // 4
			s1out2->addEntry( "XCVario, S1");                // 5

			SetupMenuSelect * stxi2 = new SetupMenuSelect( PROGMEM "TX Inversion", true , 0, true, &serial2_tx_inverted );
			rs232_2->addEntry( stxi2 );
			stxi2->setHelp( "Serial RS232 (TTL) option for negative logic, means a '1' will be sent at zero level (RS232 standard and default) and vice versa");
			stxi2->addEntry( "Normal");
			stxi2->addEntry( "Inverted");

			SetupMenuSelect * srxi2 = new SetupMenuSelect( PROGMEM "RX Inversion", true, 0, true, &serial2_rx_inverted );
			rs232_2->addEntry( srxi2 );
			srxi2->setHelp( "Serial RS232 (TTL) option for negative logic, means a '1' will be received at zero level (RS232 standard and default) and vice versa");
			srxi2->addEntry( "Normal");
			srxi2->addEntry( "Inverted");

			SetupMenuSelect * srxtw2 = new SetupMenuSelect( PROGMEM "Twist RX/TX Pins", true, 0, true, &serial2_pins_twisted );
			rs232_2->addEntry( srxtw2 );
			srxtw2->setHelp( "Option to twist RX and TX line for S2, e.g. for OpenVario. After change also a true power-cycle is needed");
			srxtw2->addEntry( "Normal");
			srxtw2->addEntry( "Twisted");

			SetupMenuSelect * stxdis2 = new SetupMenuSelect( PROGMEM "TX Line", true, 0, true, &serial2_tx_enable );
			rs232_2->addEntry( stxdis2 );
			stxdis2->setHelp( "Option to switch off RS232 TX line in case active sending is not required, e.g. for multiple devices connected to one device");
			stxdis2->addEntry( "Disable");
			stxdis2->addEntry( "Enable");

		}
		// Can Interface C1
		SetupMenu * can = new SetupMenu( "CAN Interface" );
		sye->addEntry( can );
		SetupMenuSelect * canmode = new SetupMenuSelect( PROGMEM "Datarate",	true, 0, true, &can_speed );
		can->addEntry( canmode );
		canmode->setHelp( "Datarate on high speed serial CAN interace in kbit per second");
		canmode->addEntry( "CAN OFF");
		canmode->addEntry( "250 kbit");
		canmode->addEntry( "500 kbit");
		canmode->addEntry( "1000 kbit");

		SetupMenuSelect * canrt = new SetupMenuSelect( PROGMEM "Routing", false , 0, false, &can_tx );
		can->addEntry( canrt );
		canrt->setHelp( PROGMEM "Select data source that is routed from/to CAN interface");
		canrt->addEntry( "Disable");
		canrt->addEntry( "XCVario");

		SetupMenuSelect * devmod = new SetupMenuSelect( PROGMEM "Mode", true , 0, false, &can_mode );
		can->addEntry( devmod );
		devmod->setHelp( PROGMEM "Select 'Standalone' for single seater or 'Master' in front, for secondary device in rear 'Client'");
		devmod->addEntry( "Master");
		devmod->addEntry( "Client");
		devmod->addEntry( "Standalone");

		SetupMenuSelect * nmea = new SetupMenuSelect( PROGMEM "NMEA Protocol", false , 0, true, &nmea_protocol );
		sye->addEntry( nmea );
		nmea->setHelp( "Setup the protocol used for sending NMEA sentences. This needs to be inline with the device driver chosen in XCSoar/LK8000");
		nmea->addEntry( "OpenVario");
		nmea->addEntry( "Borgelt");
		nmea->addEntry( "Cambridge");
		nmea->addEntry( "XCVario");

	}
	SetupMenu::display();
}
