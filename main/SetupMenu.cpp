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
#include "ESPAudio.h"
#include "BMPVario.h"
#include "PWMOut.h"
#include "S2F.h"
#include "Version.h"
#include "Polars.h"


IpsDisplay* MenuEntry::_display = 0;
MenuEntry* MenuEntry::root = 0;
MenuEntry* MenuEntry::selected = 0;
ESPRotary* MenuEntry::_rotary = 0;
Setup* MenuEntry::_setup = 0;
SetupVolt* MenuEntry::_setupv = 0;
BatVoltage* MenuEntry::_adc = 0;
BME280_ESP32_SPI *MenuEntry::_bmp = 0;
bool      MenuEntry::_menu_enabled = false;
extern PWMOut pwm1;
extern S2F s2f;
extern xSemaphoreHandle spiMutex;
Ucglib_ILI9341_18x240x320_HWSPI *MenuEntry::ucg = 0;


// Action Routines
int contrast( SetupMenuValFloat * p )
{
	printf("contrast( %f ) \n", *p->_value );
	pwm1.setContrast( *p->_value );
	return 0;
}

int qnh_adj( SetupMenuValFloat * p )
{
	// printf("qnh_adj");
	float alt = p->_bmp->readAltitude( *(p->_value) );
	// printf("Setup BA alt=%f QNH=%f\n", alt, *(p->_value)  );
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	p->ucg->setPrintPos(1,100);
	p->ucg->printf("%4d m", (int)(alt+0.5) );
	xSemaphoreGive(spiMutex );
	return 0;
}

// Battery Voltage Meter Calibration
int factv_adj( SetupMenuValFloat * p )
{
	printf("factv_adj");
	float adj = 0.0;
	for( int i=0; i<10; i++ ) {
           adj += p->_adc->getBatVoltage(true);
	   sleep( 0.02 );
	}
	adj = adj/10.0;
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	p->ucg->setPrintPos(1,100);
	p->ucg->printf("%0.2f Volt", adj);
	xSemaphoreGive(spiMutex );
	return 0;
}

int polar_adj( SetupMenuValFloat * p )
{
    s2f.change_polar();
    return 0;
}

int polar_select( SetupMenuSelect * p )
{
    s2f.select_polar();
    return 0;
}

int bal_adj( SetupMenuValFloat * p )
{
    s2f.change_mc_bal();
    float loadinc = (p->_setup->get()->_ballast +100.0)/100.0;
    float newwl = p->_setup->get()->_polar.wingload * loadinc;
    xSemaphoreTake(spiMutex,portMAX_DELAY );
    p->ucg->setPrintPos(1,100);
    p->ucg->printf("%0.2f kg/m2", newwl);
	xSemaphoreGive(spiMutex );
    return 0;
}
int mc_adj( SetupMenuValFloat * p )
{
    s2f.change_mc_bal();
    return 0;
}

SetupMenu::SetupMenu(){

	// _rotary->attach(this);
	highlight = -1;
	_parent = 0;
	y = 0;
	helptext = 0;
}

SetupMenu::SetupMenu( String title ) {
	printf("SetupMenu::SetupMenu( %s ) \n", title.c_str() );
	printf("this:%x\n", (int)this);
	_rotary->attach(this);
	_title = title;
	highlight = -1;
}

void SetupMenu::begin( IpsDisplay* display, ESPRotary * rotary, Setup* my_setup, SetupVolt* my_setupv, BME280_ESP32_SPI * bmp, BatVoltage *adc ){
	printf("SetupMenu() begin\n");
	_rotary = rotary;
	_setup = my_setup;
	_setupv = my_setupv;
	_bmp = bmp;
	_display = display;
	ucg = display->getDisplay();
	_adc = adc;
	setup();
}

void MenuEntry::uprintf( int x, int y, const char* format, ...) {
	if( ucg == 0 ) {
		printf("Error UCG not initialized !");
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
		printf("Error UCG not initialized !");
		return;
	}
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setPrintPos(x,y);
	ucg->print( str );
	xSemaphoreGive(spiMutex );
}


MenuEntry* MenuEntry::addMenu( MenuEntry * item ) {
	printf("MenuEntry addMenu() title %s\n", item->_title.c_str() );
	if( root == 0 ){
		printf("Init root menu\n");
		root = item;
		item->_parent = 0;
		// selected = item;
		return item;
	}
	else{
		printf("add to childs\n");
		item->_parent = this;
		_childs.push_back( item );
		return item;
	}
}

MenuEntry* MenuEntry::findMenu( String title, MenuEntry* start )
{
	printf("MenuEntry findMenu() %s %x\n", title.c_str(), (uint32_t)start );
	if( start->_title == title ) {
		printf("Menu entry found for start %s\n", title.c_str() );
		return start;
	}
	for(MenuEntry* child : start->_childs) {
		if( child->_title == title )
			return child;
		MenuEntry* m = child->findMenu( title, child );
		if( m != 0 ) {
			printf("Menu entry found for %s\n", title.c_str() );
			return m;
		}
	};
	printf("Menu entry not found for %s\n", title.c_str() );
	return 0;
}


void SetupMenu::display( int mode ){
	if( (selected != this) || !_menu_enabled )
		return;
	printf("SetupMenu display( %s)\n", _title.c_str() );
	clear();
	y=25;
	printf("Title: %s y=%d\n", selected->_title.c_str(),y );
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setPrintPos(1,y);
	ucg->printf("<< %s",selected->_title.c_str());
	ucg->drawFrame( 1,3,238,25 );

	for (int i=0; i<_childs.size(); i++ ) {
		MenuEntry * child = _childs[i];
		ucg->setPrintPos(1,(i+1)*25+25);
		ucg->printf("%s",child->_title.c_str());
		printf("Child: %s y=%d\n",child->_title.c_str() ,y );
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
		// printf("showhelp number of words: %d\n", w);
		int x=1;
		for( int p=0; p<w; p++ )
		{
			int len = ucg->getStrWidth( words[p] );
			// printf("showhelp pix len word #%d = %d, %s \n", p, len, words[p]);
			if( x+len > 239 ) {   // does still fit on line
				y+=25;
				x=1;
			}
			xSemaphoreTake(spiMutex,portMAX_DELAY );
			ucg->setPrintPos(x, 50+y);
			ucg->print( words[p] );
			xSemaphoreGive(spiMutex );
			x+=len+5;
		}
	}
}

void SetupMenu::down(){
	if( (selected != this) || !_menu_enabled )
		return;
	printf("down %d %d\n", highlight, _childs.size() );
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
	// display();
}

void SetupMenu::up(){
	if( (selected != this) || !_menu_enabled )
		return;
	printf("SetupMenu::up %d %d\n", highlight, _childs.size() );
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
	// display();
}

void SetupMenu::press(){
	if( selected == 0 )
		selected = root;
	if( (selected != this) )
		return;
	printf("SetupMenu press p:%d h:%d\n", pressed, highlight );
	// main menue behavior, extra class maybe
	// for now here we catch this
	// this is just for the main menu,
	if( (_parent == 0) && (highlight == -1) )
	{
		if( !pressed )
		{
			printf("!pressed\n");
			// _display->doMenu();
			Audio.disable();
			delay( 500 );
			_menu_enabled = true;
			// clear();

		}
		else
		{
			printf("pressed\n");
			_display->setup();
			_display->initDisplay();
			// _display->doMenu(false);
			Audio.setup();
			bmpVario.setup();
			_menu_enabled = false;
			Audio.disable(false);
		}
	}
	// default is not pressed, so just display, but we toogle pressed state at the end
	// so next time we either step up to parent,
	if( pressed )
	{
		if( highlight == -1 ) {
			printf("SetupMenu to parent\n");
			if( _parent != 0 ){
				selected = _parent;
				selected->highlight = -1;
				selected->pressed = true;
			}
		}
		else {
			printf("SetupMenu to child\n");
			if( (highlight >=0) && (highlight < (int)(_childs.size()) ) ){
				selected = _childs[highlight];
				// selected->highlight = -1;
				selected->pressed = false;
			}
		}
		pressed = false;
	}
	else
	{
		pressed = true;
	}
	printf("~SetupMenu press()\n");
}

void SetupMenu::setup( )
{
	printf("SetupMenu setup()\n");
	SetupMenu * root = new SetupMenu( "Setup" );
	MenuEntry* mm = root->addMenu( root );

	SetupMenuValFloat * mc = new SetupMenuValFloat(
			"MC", &_setup->get()->_MC, "m/s",	0.01, 10, 0.1,  mc_adj );
	mc->setHelp("Mac Cready value for optimum cruise speed, or average climb rate");
	mm->addMenu( mc );

	SetupMenuValFloat * qnh = new SetupMenuValFloat(
			"QNH", &_setup->get()->_QNH, "hPa",	900.0, 1100.0, 0.2, qnh_adj );
	qnh->setHelp("QNH pressure value from next ATC");
	mm->addMenu( qnh );

	SetupMenuValFloat * bal = new SetupMenuValFloat(
				"Ballast", &_setup->get()->_ballast, "%", 0.0, 100, 1, bal_adj  );
	bal->setHelp("Percent wing load increase by ballast");
	mm->addMenu( bal );

// Vario
	SetupMenu * va = new SetupMenu( "Vario" );
	MenuEntry* vae = mm->addMenu( va );

	SetupMenuValFloat * vga = new SetupMenuValFloat( 	"Range",
			&_setup->get()->_range,
			"m/s",
			1.0, 30.0,
			1 );
	vga->setHelp("Upper and lower value for Vario graphic display region");
	vae->addMenu( vga );

	SetupMenuValFloat * vda = new SetupMenuValFloat( 	"Damping",
			&_setup->get()->_vario_delay,
			"sec",
			0.5, 10.0,
			0.1 );
	vda->setHelp("Response time, time constant of Vario low pass filter");
	vae->addMenu( vda );

// Bluetooth
	SetupMenu * bt = new SetupMenu( 	"Bluetooth"  );

	SetupMenuSelect * btm = new SetupMenuSelect( 	"BT Sender",
				&_setup->get()->_blue_enable, true );
	btm->addEntry( "OFF");
	btm->addEntry( "ON");
	bt->addMenu( btm );

	static uint8_t select_dummy2 = 0;
	char * ids = _setup->getID();
	printf( "Setup id=%s\n", ids );
	SetupMenuSelect * idm = new SetupMenuSelect( 	"BT Name",
								&select_dummy2, false, 0, false );
	idm->addEntry( ids );
	bt->addMenu( idm );
	mm->addMenu( bt );


// Audio
	SetupMenu * ad = new SetupMenu( "Audio" );
	MenuEntry* ade = mm->addMenu( ad );
	SetupMenuSelect * am = new SetupMenuSelect( 	"Audio Mode",
				&_setup->get()->_audio_mode, false );
	am->setHelp( "Controls audio source selection" );
		am->addEntry( "Vario");
		am->addEntry( "S2F");
		am->addEntry( "Switch");
		am->addEntry( "AutoSpeed");
		ad->addMenu( am );

	SetupMenuValFloat * ts = new SetupMenuValFloat( 	"AutoSpeed",
					&_setup->get()->_s2f_speed,
					"km/h",
					50.0, 200.0,
					1.0 );
		ade->addMenu( ts );
	ts->setHelp("Transition speed when audio changes to S2F mode in AutoSpeed mode");

	SetupMenuValFloat * cf = new SetupMenuValFloat( 	"CenterFreq",
			&_setup->get()->_center_freq,
			"Hz",
			200.0, 2000.0,
			10.0 );
	cf->setHelp("Center frequency for tone indication at zero lift or S2F delta");
	ade->addMenu( cf );
	SetupMenuValFloat * oc = new SetupMenuValFloat( 	"Octaves",
			&_setup->get()->_tone_var,
			"fold",
			1.5, 4,
			0.1 );
	oc->setHelp("Maximum tone frequency variation");
	SetupMenuSelect * ar = new SetupMenuSelect( 	"Range",
						&_setup->get()->_audio_range );
		ar->addEntry( "Fix 5 m/s");
		ar->addEntry( "Variable");
		ar->setHelp("Select either fix (5m/s) or variable Audio range according to current vario setting");
		ade->addMenu( ar );

	SetupMenu * db = new SetupMenu( "Deadband" );
	MenuEntry* dbe = ade->addMenu( db );
	dbe->setHelp("Audio dead band limits within Audio remains silent");

	SetupMenuValFloat * dbminlv = new SetupMenuValFloat( 	"Lower Val",
		&_setup->get()->_deadband_neg,
					"m/s",
					-5.0, 5.0,
					0.1 );
	dbminlv->setHelp("Lower limit for Audio mute function");
	dbe->addMenu( dbminlv );
	ade->addMenu( oc );
	SetupMenuValFloat * dbmaxlv = new SetupMenuValFloat( 	"Upper Val",
			&_setup->get()->_deadband,
			"m/s",
			-5.0, 5.0,
			0.1 );
	dbmaxlv->setHelp("Upper limit for Audio mute function");
	dbe->addMenu( dbmaxlv );

// Altimeter
	SetupMenuSelect * al = new SetupMenuSelect( 	"Altimeter",
					&_setup->get()->_alt_select );
	mm->addMenu( al );
	al->addEntry( "TE Alt");
	al->addEntry( "Baro Alt");

// Polar Setup
	SetupMenu * po = new SetupMenu( "Polar" );
	po->setHelp( "Polar setup to match performance of glider");
	MenuEntry* poe = mm->addMenu( po );


	SetupMenuSelect * glt = new SetupMenuSelect( 	"Glider-Type",
						&_setup->get()->_glider_type, false, polar_select );
	poe->addMenu( glt );

	printf( "Num Polars: %d", Polars::numPolars() );
	for( int x=0; x< Polars::numPolars(); x++ ){
		glt->addEntry( Polars::getPolar(x).type );
	}

	SetupMenu * pa = new SetupMenu( "PolarAdjust" );
	pa->setHelp( "Adjust speed/sink at representative points of selected polar");
	poe->addMenu( pa );

	SetupMenuValFloat * wil = new SetupMenuValFloat(
				"Wingload", &_setup->get()->_polar.wingload, "kg/m2", 10.0, 100.0, 1, polar_adj );
	pa->addMenu( wil );
	SetupMenuValFloat * pov1 = new SetupMenuValFloat(
			"Speed 1", &_setup->get()->_polar.speed1, "km/h", 50.0, 120.0, 1, polar_adj );
	pov1->setHelp("Speed 1, near to minimum sink from polar e.g. 80 km/h");
	pa->addMenu( pov1 );
	SetupMenuValFloat * pos1 = new SetupMenuValFloat(
			"Sink  1", &_setup->get()->_polar.sink1, "m/s", -3.0, 0.0, 0.01, polar_adj );
	pos1->setHelp("Sink indication at Speed 1 from polar");
	pa->addMenu( pos1 );
	SetupMenuValFloat * pov2 = new SetupMenuValFloat(
			"Speed 2", &_setup->get()->_polar.speed2, "km/h", 100.0, 180.0, 1, polar_adj );
	pov2->setHelp("Speed 2 for a moderate cruise from polar e.g. 120 km/h");
	pa->addMenu( pov2 );
	SetupMenuValFloat * pos2 = new SetupMenuValFloat(
			"Sink  2", &_setup->get()->_polar.sink2, "m/s", -5.0, 0.0, 0.01, polar_adj );
	pos2->setHelp("Sink indication at Speed 2 from polar");
	pa->addMenu( pos2 );
	SetupMenuValFloat * pov3 = new SetupMenuValFloat(
			"Speed 3", &_setup->get()->_polar.speed3, "km/h", 120.0, 250.0, 1, polar_adj );
	pov3->setHelp("Speed 3 for a fast cruise from polar e.g. 170 km/h");
	pa->addMenu( pov3 );
	SetupMenuValFloat * pos3 = new SetupMenuValFloat(
			"Sink  3", &_setup->get()->_polar.sink3, "m/s", -6.0, 0.0, 0.01, polar_adj );
	pos3->setHelp("Sink indication at Speed 3 from polar");
	pa->addMenu( pos3 );

	SetupMenu * sy = new SetupMenu( "System" );
	MenuEntry* sye = mm->addMenu( sy );

	/*  Since its acurate after Factory adj, can be removed.
	SetupMenuValFloat * voltadj = new SetupMenuValFloat( 	"BatV Adj",
				&_setup->get()->_voltmeter_adj,
				"Volt",
				-1.2, 1.2,
				0.1 );
	voltadj->setHelp("Option to adjust voltmeter to compensate drop on line");
	sye->addMenu( voltadj );
	*/
	printf("Factory adjust: %0.5f\n", _setupv->get()->_factory_volt_adjust );
	float fva = _setupv->get()->_factory_volt_adjust;
	if( abs(fva - 0.00815) < 0.00001 ) {
	printf("Now Factory adjust ADC\n");
	SetupMenuValFloat * fvoltadj = new SetupMenuValFloat( 	"Factory VAdj",
					&_setupv->get()->_factory_volt_adjust,
					"%",
					-25.0, 25.0,
					0.01,
					factv_adj);
	fvoltadj->setHelp("Option to fine factory adjust voltmeter");
	sye->addMenu( fvoltadj );
	}

	SetupMenuSelect * fa = new SetupMenuSelect( 	"Factory",
				&_setup->get()->_factory_reset, true );
	fa->addEntry( "Cancel");
	fa->addEntry( "ResetAll");
	sye->addMenu( fa );

	Version V;
	static uint8_t select_dummy = 0;
	SetupMenuSelect * ver = new SetupMenuSelect( 	"Version",
					&select_dummy, false, 0, false );
	ver->addEntry( V.version() );
	sye->addMenu( ver );

	SetupMenu::display();
}

SetupMenuValFloat::SetupMenuValFloat( String title, float *value, String unit, float min, float max, float step, int (*action)( SetupMenuValFloat *p ) ) {
	printf("SetupMenuValFloat( %s ) \n", title.c_str() );
	_rotary->attach(this);
	_title = title;
	highlight = -1;
	_value = value;
	_unit = unit;
	_min = min;
	_max = max;
	_step = step;
	_action = action;
}

void MenuEntry::clear()
{
	printf("MenuEntry::clear\n");
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setColor(COLOR_BLACK);
	ucg->drawBox( 0,0,240,320 );
	ucg->setFont(ucg_font_ncenR14_hr);
	ucg->setPrintPos( 1, 30 );
	ucg->setColor(COLOR_WHITE);
	xSemaphoreGive(spiMutex );
}

void SetupMenuValFloat::display( int mode ){
	if( (selected != this) || !_menu_enabled )
		return;
	printf("SetupMenuValFloat display() %d %x\n", pressed, (int)this);
	uprintf( 5,25, selected->_title.c_str() );
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setPrintPos( 1, 75 );
	ucg->printf("%0.2f %s", *_value, _unit.c_str());
	xSemaphoreGive(spiMutex );
	y= 75;

	if( _action != 0 )
		(*_action)( this );

	showhelp( y );
	if(mode == 1){
		y+=24;
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		ucg->setPrintPos( 1, 225 );
		ucg->print("Saved");
		xSemaphoreGive(spiMutex );
	}
	y=0;

	if( mode == 1 )
	    vTaskDelay(1000 / portTICK_PERIOD_MS);
	printf("~SetupMenuValFloat display\n");
}

void SetupMenuValFloat::displayVal()
{
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setPrintPos( 1, 75 );
	ucg->printf("%0.2f %s", *_value, _unit.c_str());
	xSemaphoreGive(spiMutex );
}

void SetupMenuValFloat::down(){
	if( (selected != this) || !_menu_enabled )
		return;
	// printf("val down\n");
	if( *_value > _min )
		*_value-=_step;
	displayVal();
	if( _action != 0 )
		(*_action)( this );
}

void SetupMenuValFloat::up(){
	if( (selected != this) || !_menu_enabled )
		return;
	// printf("val up\n" );
	if ( *_value < _max )
		*_value+=_step;
    displayVal();
	if( _action != 0 )
		(*_action)( this );
}

void SetupMenuValFloat::press(){
	if( selected != this )
		return;
	printf("SetupMenuValFloat press\n");
	if ( pressed ){
		display( 1 );
		if( _parent != 0)
			selected = _parent;
		_parent->highlight = -1;  // to topmost selection when back
		selected->pressed = true;
		_setup->commit();
		_setupv->commit();
		pressed = false;
	}
	else
	{
		pressed = true;
		clear();
		display();
	}
}


SetupMenuSelect::SetupMenuSelect( String title, uint8_t *select, bool restart, int (*action)(SetupMenuSelect *p), bool save ) {
	printf("SetupMenuSelect( %s ) \n", title.c_str() );
	_rotary->attach(this);
	_title = title;
	highlight = -1;
	_select = select;
	_select_save = *select;
	_numval = 0;
	_restart = restart;
	_action = action;
	_save = save;
}



void SetupMenuSelect::display( int mode ){
	if( (selected != this) || !_menu_enabled )
		return;
	printf("SetupMenuSelect display() %d %x\n", pressed, (int)this);
	clear();
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setPrintPos(1,25);
	printf("Title: %s y=%d\n", _title.c_str(),y );
	ucg->printf("<< %s",_title.c_str());
	xSemaphoreGive(spiMutex );
	printf("select=%d numval=%d\n", *_select, _numval );
	int start=0;
	if( *_select > 10 )
		start = *_select-9;
	printf("start=%d \n", start );
	for( int i=start; i<_numval && i<(start+10); i++ )	{
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		ucg->setPrintPos( 1, 50+25*i );
		ucg->print( _values[i].c_str() );
		xSemaphoreGive(spiMutex );
	}
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->drawFrame( 1,(*_select+1)*25+3,238,25 );
	xSemaphoreGive(spiMutex );
	y=_numval*25+50;
	showhelp( y );
	if(mode == 1 && _save == true ){
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		ucg->setPrintPos( 1, 275 );
		ucg->print("Saved !" );
		if( _select_save != *_select )
			if( _restart ) {
				ucg->setPrintPos( 1, 250  );
				ucg->print("Now Restart" );
			}
		xSemaphoreGive(spiMutex );
	}
	if( mode == 1 )
		delay(1000);
}

void SetupMenuSelect::down(){
	if( (selected != this) || !_menu_enabled )
		return;
	ucg->setColor(COLOR_BLACK);
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->drawFrame( 1,(*_select+1)*25+3,238,25 );  // blank old frame
	ucg->setColor(COLOR_WHITE);
	if( (*_select) >  0 )
		(*_select)--;
	printf("val down %d\n", *_select );
	ucg->drawFrame( 1,(*_select+1)*25+3,238,25 );  // draw new frame
	xSemaphoreGive(spiMutex );
}

void SetupMenuSelect::up(){
	if( (selected != this) || !_menu_enabled )
		return;
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setColor(COLOR_BLACK);
	ucg->drawFrame( 1,(*_select+1)*25+3,238,25 );  // blank old frame
	ucg->setColor(COLOR_WHITE);
	if ( (*_select) < _numval-1 )
		(*_select)++;
	printf("val up %d\n", *_select );
	ucg->drawFrame( 1,(*_select+1)*25+3,238,25 );  // draw new frame
	xSemaphoreGive(spiMutex );
}

void SetupMenuSelect::press(){
	if( selected != this )
		return;
	printf("SetupMenuSelect press\n");
	if ( pressed ){
		display( 1 );
		if( _parent != 0)
			selected = _parent;
		_parent->highlight = -1;  // to topmost selection when back
		selected->pressed = true;
		_setup->commit();
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
