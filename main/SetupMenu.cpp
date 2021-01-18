/*
 * SetupMenu.cpp
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#include "SetupMenu.h"
#include "IpsDisplay.h"
#include <inttypes.h>
#include <iterator>
#include <algorithm>
#include "ESPAudio.h"
#include "BMPVario.h"
#include "S2F.h"
#include "Version.h"
#include "Polars.h"
#include <logdef.h>
#include <sensor.h>
#include "Cipher.h"
#include "Units.h"
#include "Switch.h"
#include "Flap.h"

IpsDisplay* MenuEntry::_display = 0;
MenuEntry* MenuEntry::root = 0;
MenuEntry* MenuEntry::selected = 0;
ESPRotary* MenuEntry::_rotary = 0;
AnalogInput* MenuEntry::_adc = 0;
BME280_ESP32_SPI *MenuEntry::_bmp = 0;
float MenuEntry::volume;
bool      MenuEntry::_menu_enabled = false;
extern S2F Speed2Fly;
extern xSemaphoreHandle spiMutex;
Ucglib_ILI9341_18x240x320_HWSPI *MenuEntry::ucg = 0;
static char rentry[25];
SetupMenuSelect * audio_range_sm = 0;
SetupMenuSelect * mpu = 0;

// Menu for flap setup
MenuEntry* wkm = 0;
SetupMenuValFloat * plus3 = 0;
SetupMenuValFloat * plus2 = 0;
SetupMenuValFloat * plus1 = 0;
SetupMenuValFloat * min1 = 0;
SetupMenuValFloat * min2 = 0;
SetupMenuValFloat * min3 = 0;

String vunit;
String sunit;

void update_vunit_str( int unit ){
	vunit = Units::VarioUnitLong( unit );
}

int update_vunit(SetupMenuSelect * p) {
	update_vunit_str( p->getSelect() );
	return 0;
}

void update_sunit_str( int unit ){
	sunit = Units::AirspeedUnit( unit );
}

int update_sunit(SetupMenuSelect * p) {
	update_sunit_str( p->getSelect() );
	return 0;
}

int update_s2f_speed(SetupMenuValFloat * p)
{
	Switch::setCruiseSpeed( Units::Airspeed2Kmh( *(p->_value) ) );
	return 0;
}

int update_rentry(SetupMenuValFloat * p)
{
	char rentry[25];
	sprintf( rentry, "Variable (%d m/s)", (int)(range.get()) );
	audio_range_sm->updateEntry( rentry, 1 );
	return 0;
}

void showWk(SetupMenuSelect * p){
	p->ucg->setPrintPos(1,120);
	p->ucg->printf("Sensor: %d    ", Flap::getSensorRaw() );
	delay(20);
}


void wk_cal_show( SetupMenuSelect * p, int wk ){
	p->ucg->setPrintPos(1,60);
	p->ucg->printf("Set Flap %+d   ", wk );
	delay(1000);
	while( !p->_rotary->readSwitch() )
		showWk(p);
}

// Action Routines
int wk_cal( SetupMenuSelect * p )
{
	ESP_LOGI(FNAME,"WK calibaration ( %d ) ", p->getSelect() );
	if( !Flap::haveSensor() ){
		p->clear();
		p->ucg->setPrintPos(1,60);
		p->ucg->printf("No Sensor, Abort");
		delay(2000);
		ESP_LOGI(FNAME,"Abort calibration, no signal");
		return 0;
	}
	if( p->getSelect() ){
		p->clear();
		p->ucg->setFont(ucg_font_fub25_hr);
		if( flap_pos_max.get() > 2 ){
			wk_cal_show( p,3 );
			wk_sens_pos_plus_3.set( Flap::getSensorRaw() );
		}
		if( flap_pos_max.get() > 1 ){
			wk_cal_show( p,2 );
			wk_sens_pos_plus_2.set( Flap::getSensorRaw() );
		}
		if( flap_pos_max.get() > 0 ){
			wk_cal_show( p,1 );
			wk_sens_pos_plus_1.set( Flap::getSensorRaw() );
		}
		wk_cal_show( p,0 );
		wk_sens_pos_0.set( Flap::getSensorRaw() );

		if( flap_neg_max.get() < 0 ){
			wk_cal_show( p,-1 );
			wk_sens_pos_minus_1.set( Flap::getSensorRaw() );
		}
		if( flap_neg_max.get() < -1 ){
			wk_cal_show( p,-2 );
			wk_sens_pos_minus_2.set( Flap::getSensorRaw() );
		}
		if( flap_neg_max.get() < -2 ){
			wk_cal_show( p,-3 );
			wk_sens_pos_minus_3.set( Flap::getSensorRaw() );
		}

		p->ucg->setPrintPos(1,60);
		p->ucg->printf("Saved        ");
		Flap::initSensor();
		delay(2000);
		ESP_LOGI(FNAME,"Push Button pressed");
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


void wkm_clear(){
	wkm->delMenu( plus3 );
	wkm->delMenu( plus2 );
	wkm->delMenu( plus1 );
	wkm->delMenu( min1 );
	wkm->delMenu( min2 );
	wkm->delMenu( min3 );
}


int flap_pos_act( SetupMenuValFloat * p ){
	wkm_clear();
	if( (int)flap_pos_max.get() > 2 )
		wkm->addMenu( plus3 );
	if( (int)flap_pos_max.get() > 1 )
		wkm->addMenu( plus2 );
	if( (int)flap_pos_max.get() > 0 )
		wkm->addMenu( plus1 );
	if( (int)flap_neg_max.get() < 0 )
		wkm->addMenu( min1 );
	if( (int)flap_neg_max.get() < -1 )
		wkm->addMenu( min2 );
	if( (int)flap_neg_max.get() < -2 )
		wkm->addMenu( min3 );
	Flap::initSensor();
	return 0;
}

int flap_neg_act( SetupMenuValFloat * p ){
	flap_pos_act(p);
	return 0;
}

int qnh_adj( SetupMenuValFloat * p )
{
	// ESP_LOGI(FNAME,"qnh_adj");
	float alt=0;
	for( int i=0; i<6; i++ ) {
		alt += p->_bmp->readAltitude( *(p->_value) );
		sleep(0.01);
	}
	alt = alt/6;

	ESP_LOGI(FNAME,"Setup BA alt=%f QNH=%f", alt, *(p->_value)  );
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	p->ucg->setFont(ucg_font_fub25_hr);
	p->ucg->setPrintPos(1,120);
	String u;
	float altp;
	if( alt_unit.get() == 0 ){ // m
		u = "m";
		altp = alt;
	}
	else {
		u = "ft";
		altp = alt*3.28084;
	}

	p->ucg->printf("%4d %s ", (int)(altp+0.5), u.c_str() );
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
	}
	else {
		u = "ft";
		elevp = elevp*3.28084;
	}
	p->ucg->printf("%4d %s ", (int)(elevp+0.5), u.c_str() );
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

int polar_select( SetupMenuSelect * p )
{
	Speed2Fly.select_polar();
	return 0;
}

int bal_adj( SetupMenuValFloat * p )
{
	Speed2Fly.change_mc_bal();
	float loadinc = (ballast.get() +100.0)/100.0;
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
	Speed2Fly.change_mc_bal();
	return 0;
}


int mc_adj( SetupMenuValFloat * p )
{
	Speed2Fly.change_mc_bal();
#ifdef MC_FROM_XC
	char mc[30];
	sprintf(mc, "$POV,?,MC");
	int cs = Protocols::getCheckSum(&mc[1]);
	int i = strlen(mc);
	sprintf( &mc[i], "*%02X\n", cs );
	btsender.send(mc);
#endif
	return 0;
}

int vol_adj( SetupMenuValFloat * p ){
	Audio::setVolume( (int(p->volume*1.270)));
	return 0;
}

SetupMenu::SetupMenu(){
	highlight = -1;
	_parent = 0;
	y = 0;
	helptext = 0;
	long_pressed = false;
}

SetupMenu::SetupMenu( String title ) {
	// ESP_LOGI(FNAME,"SetupMenu::SetupMenu( %s ) ", title.c_str() );
	_rotary->attach(this);
	_title = title;
	highlight = -1;
}

void SetupMenu::begin( IpsDisplay* display, ESPRotary * rotary, BME280_ESP32_SPI * bmp, AnalogInput *adc ){
	ESP_LOGI(FNAME,"SetupMenu() begin");
	_rotary = rotary;
	_bmp = bmp;
	_display = display;
	ucg = display->getDisplay();
	_adc = adc;
	setup();
	volume = default_volume.get();
}

void MenuEntry::uprintf( int x, int y, const char* format, ...) {
	if( ucg == 0 ) {
		ESP_LOGE(FNAME,"Error UCG not initialized !");
		return;
	}
	va_list argptr;
	va_start(argptr, format);
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setPrintPos(x,y);
	ucg->printf( format, argptr );
	xSemaphoreGive(spiMutex );
	va_end(argptr);
}

void MenuEntry::uprint( int x, int y, const char* str ) {
	if( ucg == 0 ) {
		ESP_LOGE(FNAME,"Error UCG not initialized !");
		return;
	}
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setPrintPos(x,y);
	ucg->print( str );
	xSemaphoreGive(spiMutex );
}


MenuEntry* MenuEntry::addMenu( MenuEntry * item ) {
	// ESP_LOGI(FNAME,"MenuEntry addMenu() title %s", item->_title.c_str() );
	if( root == 0 ){
		ESP_LOGI(FNAME,"Init root menu");
		root = item;
		item->_parent = 0;
		selected = item;
		return item;
	}
	else{
		// ESP_LOGI(FNAME,"add to childs");
		item->_parent = this;
		_childs.push_back( item );
		return item;
	}
}

void MenuEntry::delMenu( MenuEntry * item ) {
	ESP_LOGI(FNAME,"MenuEntry delMenu() title %s", item->_title.c_str() );
	std::vector<MenuEntry *>::iterator position = std::find(_childs.begin(), _childs.end(), item );
	if (position != _childs.end()) { // == myVector.end() means the element was not found
		ESP_LOGI(FNAME,"found entry, now erase" );
		_childs.erase(position);
	}

}


MenuEntry* MenuEntry::findMenu( String title, MenuEntry* start )
{
	ESP_LOGI(FNAME,"MenuEntry findMenu() %s %x", title.c_str(), (uint32_t)start );
	if( start->_title == title ) {
		ESP_LOGI(FNAME,"Menu entry found for start %s", title.c_str() );
		return start;
	}
	for(MenuEntry* child : start->_childs) {
		if( child->_title == title )
			return child;
		MenuEntry* m = child->findMenu( title, child );
		if( m != 0 ) {
			ESP_LOGI(FNAME,"Menu entry found for %s", title.c_str() );
			return m;
		}
	};
	ESP_LOGW(FNAME,"Menu entry not found for %s", title.c_str() );
	return 0;
}


void SetupMenu::display( int mode ){
	if( (selected != this) || !_menu_enabled )
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
		ucg->printf("%s",child->_title.c_str());
		ESP_LOGI(FNAME,"Child: %s y=%d",child->_title.c_str() ,y );
	}
	y+=170;
	xSemaphoreGive(spiMutex );
	showhelp( y );
}

void MenuEntry::showhelp( int y ){
	if( helptext != 0 ){
		int w=0;
		char buf[512];
		memset(buf, 0, 512);
		memcpy( buf, helptext, strlen(helptext));
		char *p = strtok (buf, " ");
		char *words[100];
		while (p != NULL)
		{
			words[w++] = p;
			p = strtok (NULL, " ");
		}
		// ESP_LOGI(FNAME,"showhelp number of words: %d", w);
		int x=1;
		int y=hypos;
		ucg->setFont(ucg_font_ncenR14_hr);
		for( int p=0; p<w; p++ )
		{
			int len = ucg->getStrWidth( words[p] );
			// ESP_LOGI(FNAME,"showhelp pix len word #%d = %d, %s ", p, len, words[p]);
			if( x+len > 239 ) {   // does still fit on line
				y+=25;
				x=1;
			}
			xSemaphoreTake(spiMutex,portMAX_DELAY );
			ucg->setPrintPos(x, y);
			ucg->print( words[p] );
			xSemaphoreGive(spiMutex );
			x+=len+5;
		}
	}
}

void dec_volume( int count ) {
	Audio::decVolume(count);
}

void inc_volume( int count ) {
	Audio::incVolume(count);
}


void SetupMenu::down(int count){
	if( selected == this && !_menu_enabled ) {
		ESP_LOGI(FNAME,"root: down");
		float &mc = MC.getRef();
		if( rot_default.get() == 1) {	 // MC Value
			if( mc > 0.1 ) {
				mc -= 0.1;
				Speed2Fly.change_mc_bal();
			}
		}
		else
			inc_volume( count );
	}
	if( (selected != this) || !_menu_enabled )
		return;
	ESP_LOGI(FNAME,"down %d %d", highlight, _childs.size() );
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
	if( selected == this && !_menu_enabled ) {
		ESP_LOGI(FNAME,"root: up");
		float &mc = MC.getRef();
		if(rot_default.get() == 1) {	 // MC Value
			if( mc < 9.9 ) {
				mc += 0.1;
				Speed2Fly.change_mc_bal();
			}
		}
		else
			dec_volume( count );
	}

	if( (selected != this) || !_menu_enabled )
		return;
	ESP_LOGI(FNAME,"SetupMenu::up %d %d", highlight, _childs.size() );
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

void SetupMenu::longPress(){
	long_pressed = true;
	ESP_LOGI(FNAME,"SetupMenue: longPress");
}


void SetupMenu::press(){
	if( selected == 0 )
		selected = root;
	if( (selected != this) )
		return;
	ESP_LOGI(FNAME,"SetupMenu press p:%d h:%d parent:%x", pressed, highlight, (int)_parent );
	// main menue behavior, extra class maybe
	// for now here we catch this
	// this is just for the main menu,
	if( (_parent == 0) && (highlight == -1) )
	{
		if( !pressed )
		{
			ESP_LOGI(FNAME,"!pressed");
			inSetup=true;
			delay( 500 );
			_menu_enabled = true;
		}
		else
		{
			ESP_LOGI(FNAME,"pressed");
			_display->setup();
			_display->clear();
			Audio::setup();
			bmpVario.setup();
			_menu_enabled = false;
			inSetup=false;
		}
	}
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
		pressed = false;
	}
	else
	{
		pressed = true;
	}
	ESP_LOGI(FNAME,"~SetupMenu press()");
}




void SetupMenu::setup( )
{
	ESP_LOGI(FNAME,"SetupMenu setup()");

	update_vunit_str( vario_unit.get() );
	update_sunit_str( ias_unit.get() );

	SetupMenu * root = new SetupMenu( "Setup" );
	MenuEntry* mm = root->addMenu( root );

	SetupMenuValFloat * mc = new SetupMenuValFloat( "MC", 0, vunit.c_str(),	0.0, 9.9, 0.1, mc_adj, true, &MC );
	mc->setHelp(PROGMEM"Default Mac Cready value for optimum cruise speed, or average climb rate to be provided in same units as variometer setting");
	mm->addMenu( mc );

	SetupMenuValFloat * vol = new SetupMenuValFloat( "Audio Volume", &volume, "%", 0.0, 100, 1, vol_adj, true );
	vol->setHelp(PROGMEM"Set audio volume");
	mm->addMenu( vol );

	SetupMenuValFloat::qnh_menu = new SetupMenuValFloat( "QNH Setup", 0, "hPa", 900.0, 1100.0, 0.250, qnh_adj, true, &QNH );
	SetupMenuValFloat::qnh_menu->setHelp(PROGMEM"Setup QNH pressure value from next ATC. On ground you may adjust to airfield altitude above MSL.");
	mm->addMenu( SetupMenuValFloat::qnh_menu );

	SetupMenuValFloat * bal = new SetupMenuValFloat( "Ballast", 0, "%", 0.0, 100, 0.2, bal_adj, true, &ballast  );
	bal->setHelp(PROGMEM"Percent wing load increase by ballast");
	mm->addMenu( bal );

	SetupMenuValFloat * bgs = new SetupMenuValFloat( "Bugs", 0, "% bugs", 0.0, 50, 1, bug_adj, true, &bugs  );
	bgs->setHelp(PROGMEM"Percent of bugs contamination to indicate degradation of gliding performance");
	mm->addMenu( bgs );


	String elev_unit = "m";
	int step = 1;
	if( alt_unit.get() == 1 ){ // ft
		step = 5;
	}

	SetupMenuValFloat * afe = new SetupMenuValFloat( "Airfield Elevation", 0, "m", -1, 3000, step, elev_adj, true, &elevation );
	afe->setHelp(PROGMEM"Set airfield elevation in meters for QNH auto adjust on ground according to this setting");
	mm->addMenu( afe );


	if( (int)(password.get()) == 271 ) {
		student_mode.set( 0 );
		password.set( 0 );
	}

	if( student_mode.get() )
	{
		SetupMenuValFloat * passw = new SetupMenuValFloat( "Expert Password", 0, "", 0, 1000, 1, 0, false, &password  );
		mm->addMenu( passw );
	}
	else
	{
		// Vario
		SetupMenu * va = new SetupMenu( "Vario" );
		MenuEntry* vae = mm->addMenu( va );


		SetupMenuValFloat * vga = new SetupMenuValFloat( 	"Range", 0, vunit.c_str(),	1.0, 30.0, 1, update_rentry, true, &range );
		vga->setHelp(PROGMEM"Upper and lower value for Vario graphic display region");
		vga->setPrecision( 0 );
		vae->addMenu( vga );

		SetupMenuValFloat * vda = new SetupMenuValFloat( 	"Vario Bar Damping", 0, "sec", 2.0, 10.0, 0.1, 0, false, &vario_delay );
		vda->setHelp(PROGMEM"Response time, time constant of Vario low pass kalman filter");
		vae->addMenu( vda );



		SetupMenuValFloat * vds2 = new SetupMenuValFloat( 	"S2F Damping", 0, "sec", 0.10001, 10.0, 0.1, 0, false, &s2f_delay );
		vds2->setHelp(PROGMEM"Time constant of S2F low pass filter");
		vae->addMenu( vds2 );

		SetupMenuValFloat * vdav = new SetupMenuValFloat( 	"Average Vario Damping", 0, "sec", 2.0, 60.0,	0.1, 0, false, &vario_av_delay );
		vdav->setHelp(PROGMEM"Response time, time constant of digital Average Vario Display");
		vae->addMenu( vdav );


		SetupMenuValFloat * vccm = new SetupMenuValFloat( "Mean Climb Minimum", 0, "m/s",	0.0, 2.0, 0.1, 0, false, &core_climb_min );
		vccm->setHelp(PROGMEM"Minimum climb rate that counts for arithmetic mean climb value (red rhombus left of TE bar)");
		vae->addMenu( vccm );

		SetupMenuValFloat * vcch = new SetupMenuValFloat( "Mean Climb Minutes", 0,	"min", 1, 300, 1, 0, false, &core_climb_history );
		vcch->setHelp(PROGMEM"Number of minutes where samples for mean climb value are regarded, default is last 3 thermals or 45 min");
		vae->addMenu( vcch );

		SetupMenuValFloat * vcp = new SetupMenuValFloat( "Mean Climb Period", 0,	"sec", 10, 60, 1, 0, false, &core_climb_period );
		vcp->setHelp(PROGMEM"Number of seconds when mean climb value is recalculated, default is last 60 seconds");
		vae->addMenu( vcp);

		SetupMenuSelect * sink = new SetupMenuSelect( 	"Polar Sink Display", 0, false, 0 , true, &ps_display );
		sink->setHelp(PROGMEM"Show polar sink rate together with TE in Vario bar");
		sink->addEntry( "DISABLE");
		sink->addEntry( "ENABLE");
		vae->addMenu( sink );

		SetupMenu * elco = new SetupMenu( "Electronic Compensation" );
		vae->addMenu( elco );
		SetupMenuSelect * enac = new SetupMenuSelect(  "Enable/Disable", 0, false, 0 , false, &te_comp_enable );
		enac->setHelp(PROGMEM"Enable/Disable electronic TE compensation option; Enable only when TE pressure is connected to ST (static) pressure");
		enac->addEntry( "DISABLE");
		enac->addEntry( "ENABLE");
		elco->addMenu( enac );

		SetupMenuValFloat * elca = new SetupMenuValFloat( "Adjustment Factor", 0, "%",	-100, 100, 0.1, 0, false, &te_comp_adjust );
		elca->setHelp(PROGMEM"Adjustment option for electronic compensation in %. This affects in % the enegry altitude calculated from airspeed");
		elco->addMenu( elca );



		// Audio
		SetupMenu * ad = new SetupMenu( "Audio" );
		MenuEntry* audio = mm->addMenu( ad );

		SetupMenuValFloat * dv = new SetupMenuValFloat( "Default Volume", 0, "%", 0, 100, 1.0, 0, false, &default_volume );
		audio->addMenu( dv );
		dv->setHelp(PROGMEM"Default volume for Audio when device is switched on");

		SetupMenuSelect * abnm = new SetupMenuSelect( "Brutto/Netto Mode", 0, false, 0 , true, &audio_netto_mode );
		abnm->setHelp(PROGMEM"Select either brutto vario, or netto vario (excluding polar sink) as source for audio");
		abnm->addEntry( "Brutto Audio");       // 0
		abnm->addEntry( "Netto Audio");        // 1
		audio->addMenu( abnm );

		SetupMenu * audios = new SetupMenu( "Tone Styles" );
		audio->addMenu( audios );
		audios->setHelp( PROGMEM "Configure audio style in terms of center frequency, octaves, single/dual tone, pitch and chopping");

		SetupMenuValFloat * cf = new SetupMenuValFloat( "CenterFreq", 0,	"Hz", 200.0, 2000.0, 10.0, 0, false, &center_freq );
		cf->setHelp(PROGMEM"Center frequency for Audio at zero Vario or zero S2F delta");
		audios->addMenu( cf );

		SetupMenuValFloat * oc = new SetupMenuValFloat( "Octaves", 0, "fold", 1.5, 4, 0.1, 0, false, &tone_var );
		oc->setHelp(PROGMEM"Maximum tone frequency variation");
		audios->addMenu( oc );

		SetupMenuSelect * dt = new SetupMenuSelect( "Single/Dual Tone", 0, false, 0 , true, &dual_tone );
		dt->setHelp(PROGMEM"Select dual tone aka ilec sound, (di/da/di) or single tone (di-di-di) mode");
		dt->addEntry( "Single Tone");      // 0
		dt->addEntry( "Dual Tone");        // 1
		audios->addMenu( dt );

		SetupMenuValFloat * htv = new SetupMenuValFloat( "Dual Tone Pich", 0, "%", 0, 50, 1.0, 0, false, &high_tone_var );
		htv->setHelp(PROGMEM"Tone variation in Dual Tone mode, percent of frequency pitch up for second tone");
		audios->addMenu( htv );

		SetupMenuSelect * tch = new SetupMenuSelect( "Tone Chopping", 0, false, 0 , true, &chopping_mode );
		tch->setHelp(PROGMEM"Select tone chopping option on positive values for Vario and or S2F");
		tch->addEntry( "Disabled");             // 0
		tch->addEntry( "Vario only");           // 1
		tch->addEntry( "S2F only");             // 2
		tch->addEntry( "Vario and S2F");        // 3  default
		audios->addMenu( tch );

		SetupMenuSelect * am = new SetupMenuSelect( "Audio Mode", 0, false, 0 , true, &audio_mode );
		am->setHelp( PROGMEM"Selects audio source. Audio either follows Vario, or S2F exclusively, controlled by external switch or automatically by speed" );
		am->addEntry( "Vario");
		am->addEntry( "S2F");
		am->addEntry( "Switch");
		am->addEntry( "AutoSpeed");
		audio->addMenu( am );

		SetupMenuValFloat * ts = new SetupMenuValFloat( "S2F AutoSpeed", 0, sunit.c_str(), 20.0, 250.0, 1.0, update_s2f_speed, false, &s2f_speed );
		audio->addMenu( ts );
		ts->setHelp(PROGMEM"Transition speed in AutoSpeed mode when audio changes from Vario to S2F mode");

		SetupMenuSelect * ar = new SetupMenuSelect( "Audio Range", 0, false, 0 , true, &audio_range  );

		audio_range_sm = ar;
		sprintf( rentry, "Variable (%d m/s)", (int)(range.get()) );
		ar->addEntry( "Max eq. 5 m/s");
		ar->addEntry( "Max eq. 10 m/s");
		ar->addEntry( rentry );
		ar->setHelp(PROGMEM"Select either fix (5m/s) or variable Audio range according to current vario setting");
		audio->addMenu( ar );



		SetupMenu * db = new SetupMenu( "Deadbands" );
		MenuEntry* dbe = audio->addMenu( db );
		dbe->setHelp(PROGMEM"Audio dead band limits within Audio remains silent in metric scale. 0,1 m/s equals roughly 20 ft/min or 0.2 knots");

		SetupMenuValFloat * dbminlv = new SetupMenuValFloat( "Lower Value Vario", 0,	"m/s", -5.0, 0, 0.1, 0 , false, &deadband_neg  );
		dbminlv->setHelp(PROGMEM"Lower limit for Audio mute function");
		dbe->addMenu( dbminlv );

		SetupMenuValFloat * dbmaxlv = new SetupMenuValFloat( "Upper Value Vario", 0,	"m/s", 0, 5.0, 0.1, 0 , false, &deadband );
		dbmaxlv->setHelp(PROGMEM"Upper limit for Audio mute function");
		dbe->addMenu( dbmaxlv );

		SetupMenuValFloat * dbmaxls2fn = new SetupMenuValFloat(	"Lower Value S2F", 	0, "+-km/h", 0, 25.0, 1, 0 , false, &s2f_deadband_neg );
		dbmaxls2fn->setHelp(PROGMEM"Negative limit in speed deviation for S2F mute function");
		dbe->addMenu( dbmaxls2fn );

		SetupMenuValFloat * dbmaxls2f = new SetupMenuValFloat( "Upper Value S2F", 	0, "+-km/h", 0, 25.0, 1, 0 , false, &s2f_deadband );
		dbmaxls2f->setHelp(PROGMEM"Positive limit in speed deviation for S2F mute function");
		dbe->addMenu( dbmaxls2f );

		SetupMenuValFloat * afac = new SetupMenuValFloat( 	"Audio Exponent", 	0, "", 0.1, 2, 0.025, 0 , false, &audio_factor );
		afac->setHelp(PROGMEM"Exponential factor < 1 gives a logarithmic, and > 1 exponential characteristic for frequency of audio signal");
		audio->addMenu( afac);



		// Polar Setup
		SetupMenu * po = new SetupMenu( "Polar" );
		po->setHelp( PROGMEM"Polar setup to match performance of glider");
		MenuEntry* poe = mm->addMenu( po );


		SetupMenuSelect * glt = new SetupMenuSelect( 	"Glider-Type",	0, false, polar_select, true, &glider_type );
		poe->addMenu( glt );

		ESP_LOGI(FNAME, "Number of Polars installed: %d", Polars::numPolars() );
		for( int x=0; x< Polars::numPolars(); x++ ){
			glt->addEntry( Polars::getPolar(x).type );
		}

		SetupMenu * pa = new SetupMenu( "PolarAdjust" );
		pa->setHelp(PROGMEM "Adjust Polar at 3 points of selected polar in commonly used metric system for Polars", 230 );
		poe->addMenu( pa );

		SetupMenuValFloat * wil = new SetupMenuValFloat( "Wingload", 0, "kg/m2", 10.0, 100.0, 0.1, polar_adj, false, &polar_wingload );
		wil->setHelp(PROGMEM"Wingload that corresponds to the 3 value pairs for speed/sink of polar");
		pa->addMenu( wil );
		SetupMenuValFloat * pov1 = new SetupMenuValFloat( "Speed 1", 0, "km/h", 50.0, 120.0, 1, polar_adj, false, &polar_speed1);
		pov1->setHelp(PROGMEM"Speed 1, near to minimum sink from polar e.g. 80 km/h");
		pa->addMenu( pov1 );
		SetupMenuValFloat * pos1 = new SetupMenuValFloat( "Sink  1", 0, "m/s", -3.0, 0.0, 0.01, polar_adj, false, &polar_sink1 );
		pos1->setHelp(PROGMEM"Sink indication at Speed 1 from polar");
		pa->addMenu( pos1 );
		SetupMenuValFloat * pov2 = new SetupMenuValFloat( "Speed 2", 0, "km/h", 70.0, 180.0, 1, polar_adj, false, &polar_speed2 );
		pov2->setHelp(PROGMEM"Speed 2 for a moderate cruise from polar e.g. 120 km/h");
		pa->addMenu( pov2 );
		SetupMenuValFloat * pos2 = new SetupMenuValFloat( "Sink  2", 0, "m/s", -5.0, 0.0, 0.01, polar_adj, false, &polar_sink2 );
		pos2->setHelp(PROGMEM"Sink indication at Speed 2 from polar");
		pa->addMenu( pos2 );
		SetupMenuValFloat * pov3 = new SetupMenuValFloat( "Speed 3", 0, "km/h", 100.0, 250.0, 1, polar_adj, false, &polar_speed3 );
		pov3->setHelp(PROGMEM"Speed 3 for a fast cruise from polar e.g. 170 km/h");
		pa->addMenu( pov3 );
		SetupMenuValFloat * pos3 = new SetupMenuValFloat( "Sink  3", 0, "m/s", -6.0, 0.0, 0.01, polar_adj, false, &polar_sink3 );
		pos3->setHelp(PROGMEM"Sink indication at Speed 3 from polar");
		pa->addMenu( pos3 );

		SetupMenuValFloat * maxbal = new SetupMenuValFloat(	"Max Ballast", 0, "liters", 0, 500, 1, 0, false, &polar_max_ballast );
		maxbal->setHelp(PROGMEM"Maximum water ballast for selected glider to allow sync from XCSoar using fraction of max ballast");
		poe->addMenu( maxbal );

		SetupMenuValFloat * wingarea = new SetupMenuValFloat( "Wing Area", 0, "m2", 0, 50, 0.1, 0, false, &polar_wingarea );
		wingarea->setHelp(PROGMEM"Wingarea for the selected glider, to allow adjustments to support wing extensions or new types in square meters");
		poe->addMenu( wingarea );

		SetupMenu * opt = new SetupMenu( "Options" );
		mm->addMenu( opt );
		if( student_mode.get() == 0 ) {
			SetupMenuSelect *stumo  = new SetupMenuSelect( "Student Mode", 0, true, 0, true, &student_mode );
			opt->addMenu( stumo );
			stumo->setHelp( PROGMEM "Student mode, disables all sophisticated setup to just basic pre-flight related items like MC, ballast or bugs");
			stumo->addEntry( "Disable");
			stumo->addEntry( "Enable");
		}

		SetupMenu * wk = new SetupMenu( "Flap (WK) Indicator" );
		wkm = opt->addMenu( wk );

		SetupMenuSelect * wke = new SetupMenuSelect( "Flap Indicator Option", 0, false, 0, true, &flap_enable );
		wke->addEntry( "Disable");
		wke->addEntry( "Enable");
		wke->setHelp(PROGMEM"Option to enable Flap (WK) Indicator to assist optimum flap setting depending on speed and ballast");
		wkm->addMenu( wke );

		SetupMenuSelect * wkes = new SetupMenuSelect( "Flap Sensor Option", 0, true, 0, true, &flap_sensor );
		wkes->addEntry( "Disable");
		wkes->addEntry( "Enable IO-2");
		wkes->addEntry( "Enable IO-34");

		wkes->setHelp(PROGMEM"Option to enable Flap sensor on corresponding IO pin, for now its IO-2, later 2021 series will use IO-34");
		wkm->addMenu( wkes );

		SetupMenuSelect * wkcal = new SetupMenuSelect( "Flap Sensor Calibration", 0, true, wk_cal, false, &dummy );
		wkcal->addEntry( "Cancel");
		wkcal->addEntry( "Start Calibration");
		wkcal->setHelp( PROGMEM "Option to calibrate flap Sensor (WK), to indicate current flap setting: Press button after each setting" );
		wkm->addMenu( wkcal );


		SetupMenuValFloat * nflpos = new SetupMenuValFloat("Max positive flap setting", 0, "", 0, 3, 1, flap_pos_act, false, &flap_pos_max  );
		nflpos->setHelp(PROGMEM"Maximum positive flap position. Restart XCVario to adjust speed menu entries");
		wkm->addMenu( nflpos );

		SetupMenuValFloat * nflneg = new SetupMenuValFloat("Max negative flap setting", 0, "", -3, 0, 1, flap_pos_act, false, &flap_neg_max  );
		nflneg->setHelp(PROGMEM"Maximum negative flap position, default -2. Restart XCVario to adjust speed menu entries");
		wkm->addMenu( nflneg );

		plus3 = new SetupMenuValFloat("Speed +3 to +2", 0, "km/h",  50, 150, 1, 0, false, &flap_plus_2  );
		plus3->setHelp(PROGMEM"Speed for transition from +3 to +3 flap setting");
		if( (int)flap_pos_max.get() > 2 )
			wkm->addMenu( plus3 );

		plus2 = new SetupMenuValFloat("Speed +2 to +1", 0, "km/h",  50, 150, 1, 0, false, &flap_plus_1  );
		plus2->setHelp(PROGMEM"Speed for transition from +2 to +1 flap setting");
		if( (int)flap_pos_max.get() > 1 )
			wkm->addMenu( plus2 );

		plus1 = new SetupMenuValFloat("Speed +1 to 0", 0, "km/h",  50, 150, 1, 0, false, &flap_0  );
		plus1->setHelp(PROGMEM"Speed for transition from +1 to 0 flap setting");
		if( (int)flap_pos_max.get() > 0 )
			wkm->addMenu( plus1 );

		min1 = new SetupMenuValFloat("Speed 0 to -1", 0, "km/h",  80, 180, 1, 0, false, &flap_minus_1  );
		min1->setHelp(PROGMEM"Speed for transition from 0 to -1 flap setting");
		if( (int)flap_neg_max.get() < 0 )
			wkm->addMenu( min1 );

		min2 = new SetupMenuValFloat("Speed -1 to -2", 0, "km/h",  100, 280, 1, 0, false, &flap_minus_2  );
		min2->setHelp(PROGMEM"Speed for transition from -1 to -2 flap setting");
		if( (int)flap_neg_max.get() < -1 )
			wkm->addMenu( min2 );

		min3 = new SetupMenuValFloat("Speed -2 to -3", 0, "km/h",  100, 280, 1, 0, false, &flap_minus_2  );
		min3->setHelp(PROGMEM"Speed for transition from -2 to -3 flap setting");
		if( (int)flap_neg_max.get() < -2 )
			wkm->addMenu( min3 );




		// Units
		SetupMenu * un = new SetupMenu( "Units" );
		un->setHelp( PROGMEM "Setup altimeter, airspeed indicator and variometer with European Metric, American, British or Australian units");
		SetupMenuSelect * alu = new SetupMenuSelect( 	"Altimeter",	0, false,  0, true, &alt_unit );
		alu->addEntry( "Meter       (m)");
		alu->addEntry( "Foot        (ft)");
		alu->addEntry( "FlightLevel (FL)");
		un->addMenu( alu );
		SetupMenuSelect * iau = new SetupMenuSelect( "Indicated Airspeed", 0, false , update_sunit, true, &ias_unit );
		iau->addEntry( "Km per hour     (Km/h)");
		iau->addEntry( "Miles per hour (mph)");
		iau->addEntry( "Knots           (kt)");
		un->addMenu( iau );
		SetupMenuSelect * vau = new SetupMenuSelect( "Vario", 0, false , update_vunit, true, &vario_unit );
		vau->addEntry( "Meters/sec  (m/s)");
		vau->addEntry( "x100 ft/min (cft/min)");
		vau->addEntry( "Knots       (knots)");
		un->addMenu( vau );
		opt->addMenu( un );



		SetupMenuSelect * amode = new SetupMenuSelect( "Airspeed Mode",	0, false, 0, true, &airspeed_mode );
		opt->addMenu( amode );
		amode->setHelp( PROGMEM "Select mode of Airspeed indicator to display IAS (Indicated AirSpeed, default) or TAS (True AirSpeed) considering air density");
		amode->addEntry( "IAS");
		amode->addEntry( "TAS");



		SetupMenuSelect * atl = new SetupMenuSelect( "Automatic Transition",	0, false, 0, true, &fl_auto_transition );
		opt->addMenu( atl );
		atl->setHelp( PROGMEM "Option to enable automatic altitude transition to QNH Standard (1013.25) above 'Transition Altitude'");
		atl->addEntry( "Disable");
		atl->addEntry( "Enable");

		SetupMenuValFloat * tral = new SetupMenuValFloat( "Transition Altitude", 0, "FL", 0, 400, 10, 0, false, &transition_alt  );
		tral->setHelp(PROGMEM"Transition altitude (or transition height, when using QFE) is the altitude/height above which standard pressure (QNE) is set (1013.2 mb/hPa)");
		opt->addMenu( tral );

		SetupMenu * flarm = new SetupMenu( "FLARM" );
		opt->addMenu( flarm );
		flarm->setHelp( PROGMEM "Option to diplay FLARM Warnings depending on FLARM alarm level");

		SetupMenuSelect * flarml = new SetupMenuSelect( "FLARM Level",	0, false, 0, true, &flarm_warning );
		flarm->addMenu( flarml );
		flarml->setHelp( PROGMEM "Enable FLARM Alarm level 1 is lowest with 13-18 sec, 2 medium 9-12 sec and 3 highest with 0-8 sec until impact");
		flarml->addEntry( "Disable");
		flarml->addEntry( "Alarm Level 1");
		flarml->addEntry( "Alarm Level 2");
		flarml->addEntry( "Alarm Level 3");

		SetupMenuValFloat * flarmv = new SetupMenuValFloat("Alarm Volume",  0, "%", 20, 125, 1, 0, false, &flarm_volume  );
		flarmv->setHelp(PROGMEM "Maximum volume FLARM alarm audio warning");
		flarm->addMenu( flarmv );

		SetupMenuSelect * flarms = new SetupMenuSelect( "FLARM Simulation",	0, false, 0, true, &flarm_sim );
		flarm->addMenu( flarms );
		flarms->setHelp( PROGMEM "Simulate an airplane crossing from left to right with different alarm levels and vertical distance");
		flarms->addEntry( "Disable");
		flarms->addEntry( "Start Sim");

		SetupMenu * sy = new SetupMenu( "System" );
		MenuEntry* sye = mm->addMenu( sy );

		SetupMenu * soft = new SetupMenu( "Software Update" );
		sye->addMenu( soft );

		Version V;
		static int select_dummy = 0;
		SetupMenuSelect * ver = new SetupMenuSelect( "Software Version", &select_dummy, false, 0, false );
		ver->addEntry( V.version() );
		soft->addMenu( ver );

		SetupMenuSelect * upd = new SetupMenuSelect( "Software Update", 0, true, 0, true, &software_update );
		soft->addMenu( upd );
		upd->setHelp(PROGMEM "Software Update over the air (OTA). Start Wifi AP, then connect to Wifi 'ESP32 OTA' and open http://192.161.0.1 to upload firmware");
		upd->addEntry( "Cancel");
		upd->addEntry( "Start Wifi AP");

		SetupMenuSelect * fa = new SetupMenuSelect( "Factory Reset", 0, true, 0, false, &factory_reset );
		fa->setHelp(PROGMEM "Option to reset all settings to factory defaults, means metric system, 5m/s vario range and more");
		fa->addEntry( "Cancel");
		fa->addEntry( "ResetAll");
		sye->addMenu( fa );

		SetupMenu * bat = new SetupMenu( "Battery Setup" );
		bat->setHelp( PROGMEM "Adjust corresponding voltage for battery symbol display low,red,yellow and full");
		sye->addMenu( bat );

		SetupMenuValFloat * blow = new SetupMenuValFloat( "Battery Low", 0, "Volt ", 0.0, 28.0, 0.1, 0, false, &bat_low_volt );
		SetupMenuValFloat * bred = new SetupMenuValFloat( "Battery Red", 0, "Volt ", 0.0, 28.0, 0.1, 0, false, &bat_red_volt  );
		SetupMenuValFloat * byellow = new SetupMenuValFloat( "Battery Yellow", 0, "Volt ", 0.0, 28.0, 0.1, 0, false, &bat_yellow_volt );
		SetupMenuValFloat * bfull = new SetupMenuValFloat( "Battery Full", 0, "Volt ", 0.0, 28.0, 0.1, 0, false, &bat_full_volt  );

		SetupMenuSelect * batv = new SetupMenuSelect( "Battery Display", 0, false, 0, true, &battery_display  );
		batv->setHelp(PROGMEM "Option to display battery charge state either in Percentage e.g. 75% or Voltage e.g. 12.5V");
		batv->addEntry( "Percentage");
		batv->addEntry( "Voltage");

		bat->addMenu(blow);
		bat->addMenu(bred);
		bat->addMenu(byellow);
		bat->addMenu(bfull);
		bat->addMenu( batv );

		SetupMenu * hardware = new SetupMenu( "Hardware Setup" );
		hardware->setHelp( PROGMEM "Setup variometer hardware like display, rotary");
		sye->addMenu( hardware );

		SetupMenu * display = new SetupMenu( "DISPLAY Setup" );
		hardware->addMenu( display );
		// UNIVERSAL, RAYSTAR_RFJ240L_40P, ST7789_2INCH_12P, ILI9341_TFT_18P
		if( display_type.get() == UNIVERSAL )
		{
			SetupMenuSelect * dtype = new SetupMenuSelect( 	"Display Type", 0, false, 0, true, &display_type );
			dtype->setHelp( PROGMEM "Factory setup for corresponding display type used");
			dtype->addEntry( "UNIVERSAL");
			dtype->addEntry( "RAYSTAR_RFJ240L_40P");
			dtype->addEntry( "ST7789_2INCH_12P");
			dtype->addEntry( "ILI9341_TFT_18P");
			display->addMenu( dtype );
		}

		SetupMenuSelect * disty = new SetupMenuSelect( "Display Style", 0, false , 0, false, &display_style );
		display->addMenu( disty );
		disty->setHelp( PROGMEM "Display style in more digital airliner stype or retro mode with classic vario meter needle");
		disty->addEntry( "Airliner");
		disty->addEntry( "Retro");

		// Orientation   _display_orientation
		SetupMenuSelect * diso = new SetupMenuSelect( "Display Orientation", 0, true, 0, true, &display_orientation );
		display->addMenu( diso );
		diso->setHelp( PROGMEM "Display Orientation either NORMAL means control panel is right, or TOPDOWN means control panel is left");
		diso->addEntry( "NORMAL (Rotary left)");
		diso->addEntry( "TOPDOWN (Rotary right)");



		SetupMenu * rotary = new SetupMenu( "Rotary Setup" );
		hardware->addMenu( rotary );
		SetupMenuSelect * rotype;
		if( hardwareRevision.get() < 3 )
			rotype = new SetupMenuSelect( "Rotary Direction", 0, false , 0, false, &rotary_dir );
		else
			rotype = new SetupMenuSelect( "Rotary Direction", 0, false , 0, false, &rotary_dir_21 );
		rotary->addMenu( rotype );
		rotype->setHelp( PROGMEM "Select type of rotary switch, different brands may need adjustment");
		rotype->addEntry( "Clockwise");
		rotype->addEntry( "Counterclockwise");

		SetupMenuSelect * roinc = new SetupMenuSelect( "Rotary Increment", 0, false , 0, false, &rotary_inc );
		rotary->addMenu( roinc );
		roinc->setHelp( PROGMEM "Select rotary sensitivity in increment's per detent, for personal preference");
		roinc->addEntry( "1 Indent per Increment");
		roinc->addEntry( "2 Indent per Increment");
		roinc->addEntry( "3 Indent per Increment");
		roinc->addEntry( "4 Indent per Increment");

		SetupMenuSelect * s2fsw = new SetupMenuSelect( "S2F Switch Type", 0, false , 0, false, &s2f_switch_type );
		hardware->addMenu( s2fsw );
		s2fsw->setHelp( PROGMEM "Select S2F hardware switch type, what can be an normal switch or a push button without lock toggling mode any time pressed");
		s2fsw->addEntry( "Switch");
		s2fsw->addEntry( "Push Button");

		if( hardwareRevision.get() >= 3 ){
			SetupMenu * ahrs = new SetupMenu( "AHRS Setup" );
			hardware->addMenu( ahrs );
			mpu = new SetupMenuSelect( "AHRS Option", 0, true , 0, true, &attitude_indicator );
			ahrs->addMenu( mpu );
			mpu->setHelp( PROGMEM "Enable High Accuracy Attitude Sensor (AHRS) with valid license key entered");
			mpu->addEntry( "Disable");
			if( ahrsKeyValid )
				mpu->addEntry( "Enable");

			SetupMenuSelect * ahrsaz = new SetupMenuSelect( "AHRS Autozero", 0, true , 0, true, &ahrs_autozero );
			ahrs->addMenu( ahrsaz );
			ahrsaz->setHelp( PROGMEM "Start Autozero of AHRS Sensor; Preconditions: On ground; Wings 100% horizontal, fuselage in flight position !");
			ahrsaz->addEntry( "Cancel");
			ahrsaz->addEntry( "Start AHRS Autozero");

			SetupMenu * ahrslc = new SetupMenu( "AHRS License Key" );
			ahrslc->setHelp( PROGMEM "Enter valid AHRS License Key, then with valid key under 'AHRS Option', AHRS feature can be enabled");
			ahrs->addMenu( ahrslc );

			SetupMenuSelect * ahrslc1 = new SetupMenuSelect( "First  Letter",	0, false, add_key, false, &ahrs_licence_dig1 );
			SetupMenuSelect * ahrslc2 = new SetupMenuSelect( "Second Letter",	0, false, add_key, false, &ahrs_licence_dig2 );
			SetupMenuSelect * ahrslc3 = new SetupMenuSelect( "Third  Letter",	0, false, add_key, false, &ahrs_licence_dig3 );
			SetupMenuSelect * ahrslc4 = new SetupMenuSelect( "Last   Letter",	0, false, add_key, false, &ahrs_licence_dig4 );
			ahrslc->addMenu( ahrslc1 );
			ahrslc->addMenu( ahrslc2 );
			ahrslc->addMenu( ahrslc3 );
			ahrslc->addMenu( ahrslc4 );
			for( int x=48; x<= 90; x++ ){
				char e[2];
				e[0]=char(x);
				e[1]='\0';
				ahrslc1->addEntry( e );
				ahrslc2->addEntry( e );
				ahrslc3->addEntry( e );
				ahrslc4->addEntry( e );
			}
		}

		float fva = factory_volt_adjust.get();
		if( abs(fva - 0.00815) < 0.00001 ) {
			ESP_LOGI(FNAME,"Add option to Factory adjust ADC; not yet done");
			SetupMenuValFloat * fvoltadj = new SetupMenuValFloat( 	"Factory Voltmeter Adj", 0, "%",	-25.0, 25.0, 0.01, factv_adj, false, &factory_volt_adjust );
			fvoltadj->setHelp(PROGMEM "Option to fine factory adjust voltmeter");
			sye->addMenu( fvoltadj );
		}

		// Altimeter, IAS
		SetupMenu * aia = new SetupMenu( "Altimeter, Airspeed" );
		sye->addMenu( aia );
		SetupMenuSelect * als = new SetupMenuSelect( "Altimeter Source",	0, false, 0, true, &alt_select );
		aia->addMenu( als );
		als->setHelp( PROGMEM "Select Source of altimeter to either barometric or static pressure sensor (default), or TE sensor what results in an 'energy' altitude");
		als->addEntry( "TE   Sensor");
		als->addEntry( "Baro Sensor");

		SetupMenuValFloat * spc = new SetupMenuValFloat( "IAS Calibration", 0, "%", -10, 10, 1, 0, false, &speedcal  );
		spc->setHelp(PROGMEM"Calibration of indicated airspeed (IAS). Normally not needed, hence pressure probes may have systematic error");
		aia->addMenu( spc );

		SetupMenuSelect * auze = new SetupMenuSelect( "AutoZero AS Sensor",	0, true, 0, true, &autozero );
		aia->addMenu( auze );
		auze->setHelp( PROGMEM "Recalculate zero point for airspeed sensor on next power on");
		auze->addEntry( "Cancel");
		auze->addEntry( "Start Autozero");

		SetupMenu * ausw = new SetupMenu( "Stall Warning");
		aia->addMenu( ausw );
		ausw->setHelp( PROGMEM "Configure stall warning");

		SetupMenuSelect * auswo = new SetupMenuSelect( "Stall Warning Option", 0, false, 0, false, &stall_warning );
		ausw->addMenu( auswo );
		auswo->setHelp( PROGMEM "Enable alarm sound when speed goes below configured stall speed and below (until 30% less)");
		auswo->addEntry( "Disable");
		auswo->addEntry( "Enable");

		SetupMenuValFloat * ausws = new SetupMenuValFloat( "Stall Speed", 0, sunit.c_str(), 20, 200, 1, 0, false, &stall_speed  );
		ausws->setHelp(PROGMEM"Configure stalling speed for corresponding airplane type");
		ausw->addMenu( ausws );

		// Bluetooth or Wifi
		String btname="Wireless ";
		btname += SetupCommon::getID();

		SetupMenuSelect * btm = new SetupMenuSelect(  btname, 0, true, 0, true, &blue_enable );
		btm->setHelp( PROGMEM "Activate type wireless interface to connect navigation devices running e.g. XCSoar, or to another XCVario as client");
		btm->addEntry( "Disable");
		btm->addEntry( "Bluetooth");
		btm->addEntry( "Wireless LAN");
		btm->addEntry( "Wireless Client");
		opt->addMenu( btm );

		// Rotary Default
		SetupMenuSelect * rd = new SetupMenuSelect( "Rotary Default", 0, false, 0, true, &rot_default );
		sye->addMenu( rd );
		rd->setHelp(PROGMEM "Select value to be altered at rotary movement outside of setup menu");
		rd->addEntry( "Volume");
		rd->addEntry( "MC Value");

		// _serial1_speed
		SetupMenu * rs232 = new SetupMenu( "RS232 Interface ttyS1" );
		sye->addMenu( rs232 );
		SetupMenuSelect * s2sp = new SetupMenuSelect( PROGMEM "Baudraute",	0, true, 0, true, &serial1_speed );
		rs232->addMenu( s2sp );
		// s2sp->setHelp( "Serial RS232 (TTL) speed, pins RX:2, TX:3 on external RJ45 connector");
		s2sp->addEntry( "OFF");
		s2sp->addEntry( "4800 baud");
		s2sp->addEntry( "9600 baud");
		s2sp->addEntry( "19200 baud");
		s2sp->addEntry( "38400 baud");
		s2sp->addEntry( "57600 baud");
		s2sp->addEntry( "115200 baud");

		SetupMenuSelect * s1in = new SetupMenuSelect( PROGMEM "Serial Loops", 0, false, 0, true, &serial1_rxloop );
		rs232->addMenu( s1in );
		s1in->setHelp( "Option to loop serial ttyS1 RX to ttyS1 TX, e.g. for unidirectional OV or Kobo connection" );
		s1in->addEntry( "Disable");     // 0
		s1in->addEntry( "Enable");      // 1

		SetupMenuSelect * s1out = new SetupMenuSelect( PROGMEM "Serial Routing", 0, false, 0, true, &serial1_tx );
		rs232->addMenu( s1out );
		s1out->setHelp( "Select devices routed to serial interface ttyS1");
		s1out->addEntry( "Disable all");
		s1out->addEntry( "XCVario");                 // 1    XCVario NMEA Data
		s1out->addEntry( "Wireless-XCSoar");         // 2    XCSoar Data
		s1out->addEntry( "WL-XCSoar, XCVario");      // 3

		SetupMenuSelect * stxi = new SetupMenuSelect( PROGMEM "Serial TX Inversion", 0, true , 0, true, &serial1_tx_inverted );
		rs232->addMenu( stxi );
		stxi->setHelp( "Serial RS232 (TTL) option for negative logic, means a '1' will be sent at zero level (RS232 standard and default) and vice versa");
		stxi->addEntry( "Normal");
		stxi->addEntry( "Inverted");

		SetupMenuSelect * srxi = new SetupMenuSelect( PROGMEM "Serial RX Inversion", 0, true, 0, true, &serial1_rx_inverted );
		rs232->addMenu( srxi );
		srxi->setHelp( "Serial RS232 (TTL) option for negative logic, means a '1' will be received at zero level (RS232 standard and default) and vice versa");
		srxi->addEntry( "Normal");
		srxi->addEntry( "Inverted");

		if( hardwareRevision.get() >= 3 ) {
			SetupMenu * rs232_2 = new SetupMenu( "RS232 Interface ttyS2" );
			sye->addMenu( rs232_2 );
			SetupMenuSelect * s2sp2 = new SetupMenuSelect( PROGMEM "Baudraute",	0, true, 0, true, &serial2_speed );
			rs232_2->addMenu( s2sp2 );
			// s2sp->setHelp( "Serial RS232 (TTL) speed, pins RX:2, TX:3 on external RJ45 connector");
			s2sp2->addEntry( "OFF");
			s2sp2->addEntry( "4800 baud");
			s2sp2->addEntry( "9600 baud");
			s2sp2->addEntry( "19200 baud");
			s2sp2->addEntry( "38400 baud");
			s2sp2->addEntry( "57600 baud");
			s2sp2->addEntry( "115200 baud");

			SetupMenuSelect * s1out2 = new SetupMenuSelect( PROGMEM "Routing", 0, false, 0, true, &serial2_tx );
			rs232_2->addMenu( s1out2 );
			s1out2->setHelp( "Select devices routed to serial interface ttyS2", 220 );
			s1out2->addEntry( "Disable all");
			s1out2->addEntry( "XCVario");                       // 1    XCVario NMEA Data bidir
			s1out2->addEntry( "Wireless-XCSoar");               // 2    XCSoar Data bidir
			s1out2->addEntry( "WL-XCSoar, XCVario");            // 3
			s1out2->addEntry( "ttyS1");                         // 4
			s1out2->addEntry( "XCVARIO, ttyS1");                // 5


			SetupMenuSelect * stxi2 = new SetupMenuSelect( PROGMEM "Serial TX Inversion", 0, true , 0, true, &serial2_tx_inverted );
			rs232_2->addMenu( stxi2 );
			stxi2->setHelp( "Serial RS232 (TTL) option for negative logic, means a '1' will be sent at zero level (RS232 standard and default) and vice versa");
			stxi2->addEntry( "Normal");
			stxi2->addEntry( "Inverted");

			SetupMenuSelect * srxi2 = new SetupMenuSelect( PROGMEM "Serial RX Inversion", 0, true, 0, true, &serial2_rx_inverted );
			rs232_2->addMenu( srxi2 );
			srxi2->setHelp( "Serial RS232 (TTL) option for negative logic, means a '1' will be received at zero level (RS232 standard and default) and vice versa");
			srxi2->addEntry( "Normal");
			srxi2->addEntry( "Inverted");

			SetupMenuSelect * srxtw2 = new SetupMenuSelect( PROGMEM "Twist RX/TX Pins", 0, true, 0, true, &serial2_pins_twisted );
			rs232_2->addMenu( srxtw2 );
			srxtw2->setHelp( "Option to twist RX and TX line for ttyS2. Useful to support different types of peers. After restart also a true power-cycle is needed");
			srxtw2->addEntry( "Normal  (4:TX 3:RX)");
			srxtw2->addEntry( "Twisted (4:RX 3:TX)");

		}

		SetupMenuSelect * nmea = new SetupMenuSelect( PROGMEM "NMEA Protocol", 0, false , 0, true, &nmea_protocol );
		sye->addMenu( nmea );
		nmea->setHelp( "Setup the protocol used for sending NMEA sentences. This needs to be inline with the device driver chosen in XCSoar/LK8000");
		nmea->addEntry( "OpenVario");
		nmea->addEntry( "Borgelt");
		nmea->addEntry( "Cambridge");
		nmea->addEntry( "XCVario");
	}
	SetupMenu::display();
}

SetupMenuValFloat::SetupMenuValFloat( String title, float *value, const char *unit, float min, float max, float step, int (*action)( SetupMenuValFloat *p ), bool end_menu, SetupNG<float> *anvs ) {
	// ESP_LOGI(FNAME,"SetupMenuValFloat( %s ) ", title.c_str() );
	_rotary->attach(this);
	_title = title;
	highlight = -1;
	_nvs = 0;
	if( value )
		_value = value;
	_unit = unit;
	_min = min;
	_max = max;
	_step = step;
	_action = action;
	_end_menu = end_menu;
	_precision = 2;
	if( step >= 1 )
		_precision = 0;
	if( anvs ) {
		_nvs = anvs;
		_value = _nvs->getPtr();
	}

}

void SetupMenuValFloat::setPrecision( int prec ){
	_precision = prec;
}


void MenuEntry::clear()
{
	ESP_LOGI(FNAME,"MenuEntry::clear");
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setColor(COLOR_BLACK);
	ucg->drawBox( 0,0,240,320 );
	// ucg->begin(UCG_FONT_MODE_SOLID);
	ucg->setFont(ucg_font_ncenR14_hr);
	ucg->setPrintPos( 1, 30 );
	ucg->setColor(COLOR_WHITE);
	xSemaphoreGive(spiMutex );
}


SetupMenuValFloat * SetupMenuValFloat::qnh_menu = 0;

void SetupMenuValFloat::showQnhMenu(){
	ESP_LOGI(FNAME,"SetupMenuValFloat::showQnhMenu()");
	if( qnh_menu ) {
		ESP_LOGI(FNAME,"qnh_menu = true");
		_menu_enabled = true;
		selected = qnh_menu;
		inSetup=true;
		qnh_menu->clear();
		qnh_menu->display();
		qnh_menu->pressed = true;
	}
}

void SetupMenuValFloat::display( int mode ){
	if( (selected != this) || !_menu_enabled )
		return;
	// ESP_LOGI(FNAME,"SetupMenuValFloat display() %d %x", pressed, (int)this);
	uprintf( 5,25, selected->_title.c_str() );
	displayVal();
	y= 75;
	if( _action != 0 )
		(*_action)( this );

	showhelp( y );
	if(mode == 1){
		y+=24;
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		ucg->setPrintPos( 1, 300 );
		ucg->print("Saved");
		xSemaphoreGive(spiMutex );
	}
	y=0;

	if( mode == 1 )
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	ESP_LOGI(FNAME,"~SetupMenuValFloat display");
}

void SetupMenuValFloat::displayVal()
{
	ucg->setFont(ucg_font_fub25_hr);
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setPrintPos( 1, 70 );
	if( _unit )
		ucg->printf("%0.*f %s   ", _precision, *_value, _unit);
	xSemaphoreGive(spiMutex );
	ucg->setFont(ucg_font_ncenR14_hr);
}

void SetupMenuValFloat::down( int count ){
	if( (selected != this) || !_menu_enabled )
		return;
	ESP_LOGI(FNAME,"val down %d times ", count );
	while( (*_value > _min) && count ) {
		*_value -= _step;
		count --;
	}
	if( *_value < _min )
		*_value = _min;
	displayVal();
	if( _action != 0 )
		(*_action)( this );
}

void SetupMenuValFloat::up( int count ){
	if( (selected != this) || !_menu_enabled )
		return;
	ESP_LOGI(FNAME,"val up %d times ", count );
	while( (*_value < _max) && count ) {
		*_value += _step;
		count--;
	}
	if( *_value > _max )
		*_value = _max;
	displayVal();
	if( _action != 0 )
		(*_action)( this );
}

void SetupMenuValFloat::press(){
	if( selected != this )
		return;
	ESP_LOGI(FNAME,"SetupMenuValFloat press");
	if ( pressed ){
		display( 1 );
		if( _end_menu )
			selected = root;
		else if( _parent != 0 )
			selected = _parent;
		selected->highlight = -1;  // to topmost selection when back
		selected->pressed = true;
		if( _nvs )
			_nvs->commit();
		pressed = false;
		BMPVario::setHolddown( 150 );  // so seconds stop average
		if( _end_menu )
			selected->press();
	}
	else
	{
		pressed = true;
		clear();
		display();
	}
}


SetupMenuSelect::SetupMenuSelect( String title, int *select, bool restart, int (*action)(SetupMenuSelect *p), bool save, SetupNG<int> *anvs ) {
	ESP_LOGI(FNAME,"SetupMenuSelect( %s ) ", title.c_str() );
	_rotary->attach(this);
	_title = title;
	_nvs = 0;
	highlight = -1;
	if( select ) {
		_select = select;
		_select_save = *select;
	}
	_numval = 0;
	_restart = restart;
	_action = action;
	_save = save;
	if( anvs ) {
		_nvs = anvs;
		ESP_LOGI(FNAME,"_nvs->key(): %s val: %d", _nvs->key(), (int)(_nvs->get()) );
		_select = _nvs->getPtr();
		_select_save = _nvs->get();
	}
}


void SetupMenuSelect::display( int mode ){
	if( (selected != this) || !_menu_enabled )
		return;
	ESP_LOGI(FNAME,"SetupMenuSelect display() %d %x", pressed, (int)this);
	clear();
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setPrintPos(1,25);
	ESP_LOGI(FNAME,"Title: %s y=%d", _title.c_str(),y );
	ucg->printf("<< %s",_title.c_str());
	xSemaphoreGive(spiMutex );
	ESP_LOGI(FNAME,"select=%d numval=%d size=%d val=%s", *_select, _numval, _values.size(), _values[*_select].c_str() );
	if( _numval > 9 ){
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		ucg->setPrintPos( 1, 50 );
		ucg->printf( "%s                ", _values[*_select].c_str() );
		xSemaphoreGive(spiMutex );
	}else
	{
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		for( int i=0; i<_numval && i<+10; i++ )	{
			ucg->setPrintPos( 1, 50+25*i );
			ucg->print( _values[i].c_str() );
		}
		ucg->drawFrame( 1,(*_select+1)*25+3,238,25 );
		xSemaphoreGive(spiMutex );
	}

	y=_numval*25+50;
	showhelp( y );
	if(mode == 1 && _save == true ){
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		ucg->setPrintPos( 1, 300 );
		ucg->print("Saved !" );
		if( _select_save != *_select )
			if( _restart ) {
				ucg->setColor(COLOR_BLACK);
				ucg->drawBox( 0,160,240,160 );
				ucg->setPrintPos( 1, 250  );
				ucg->setColor(COLOR_WHITE);
				ucg->print("Now Restart" );
			}
		xSemaphoreGive(spiMutex );
	}
	if( mode == 1 )
		delay(1000);
}

void SetupMenuSelect::down(int count){
	if( (selected != this) || !_menu_enabled )
		return;

	if( _numval > 9 ){
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		while( count ) {
			if( (*_select) > 0 )
				(*_select)--;
			count--;
		}
		ucg->setPrintPos( 1, 50 );
		ucg->printf("%s                  ",_values[*_select].c_str());
		xSemaphoreGive(spiMutex );
	}else {
		ucg->setColor(COLOR_BLACK);
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		ucg->drawFrame( 1,(*_select+1)*25+3,238,25 );  // blank old frame
		ucg->setColor(COLOR_WHITE);
		if( (*_select) >  0 )
			(*_select)--;
		ESP_LOGI(FNAME,"val down %d", *_select );
		ucg->drawFrame( 1,(*_select+1)*25+3,238,25 );  // draw new frame
		xSemaphoreGive(spiMutex );
	}
}

void SetupMenuSelect::up(int count){
	if( (selected != this) || !_menu_enabled )
		return;
	if( _numval > 9 )
	{
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		while( count ) {
			if( (*_select) <  _numval-1 )
				(*_select)++;
			count--;
		}
		ucg->setPrintPos( 1, 50 );
		ucg->printf("%s                   ", _values[*_select].c_str());
		xSemaphoreGive(spiMutex );
	}else {
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		ucg->setColor(COLOR_BLACK);
		ucg->drawFrame( 1,(*_select+1)*25+3,238,25 );  // blank old frame
		ucg->setColor(COLOR_WHITE);
		if ( (*_select) < _numval-1 )
			(*_select)++;
		ESP_LOGI(FNAME,"val up %d", *_select );
		ucg->drawFrame( 1,(*_select+1)*25+3,238,25 );  // draw new frame
		xSemaphoreGive(spiMutex );
	}
}

void SetupMenuSelect::press(){
	if( selected != this )
		return;
	ESP_LOGI(FNAME,"SetupMenuSelect press");
	if ( pressed ){
		display( 1 );
		if( _parent != 0)
			selected = _parent;
		_parent->highlight = -1;  // to topmost selection when back
		selected->pressed = true;
		if( _nvs )
			_nvs->commit();
		pressed = false;
		if( _action != 0 )
			(*_action)( this );
		if( _select_save != *_select )
			if( _restart ) {
				sleep( 2 );
				esp_restart();
			}
	}
	else
	{
		pressed = true;
	}
}
