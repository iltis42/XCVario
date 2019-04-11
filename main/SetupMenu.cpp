/*
 * SetupMenu.cpp
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#include "SetupMenu.h"
#include "DotDisplay.h"
#include <inttypes.h>
#include <iterator>
#include "ESPAudio.h"
#include "BMPVario.h"
#include "PWMOut.h"
#include "S2F.h"
#include "Version.h"
#include "Polars.h"


DotDisplay* MenuEntry::_display = 0;
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

// Action Routines
int contrast( SetupMenuValFloat * p )
{
	printf("contrast( %f ) \n", *p->_value );
	pwm1.setContrast( *p->_value );
	return 0;
}

int qnh_adj( SetupMenuValFloat * p )
{
	printf("qnh_adj");
	char j[14];
	float alt = p->_bmp->readAltitude( *(p->_value) );
	sprintf( j,"%4d %s", (int)(alt+0.5), "m"  );
	//  printf( "%4d %s", (int)(alt+0.5), "m"  );
	printf("Setup BA alt=%f QNH=%f\n", alt, *(p->_value)  );
	u8g2_DrawStr( p->u8g2, 110-30,1, j );
	return 0;
}

// Battery Voltage Meter Calibration
int factv_adj( SetupMenuValFloat * p )
{
	printf("factv_adj");
	char j[14];
	float adj = 0.0;
	for( int i=0; i<10; i++ )
		adj += p->_adc->getBatVoltage(true);
		sleep( 0.01 );
	adj = adj/10.0;
	sprintf( j,"%0.2f %s", adj, "V"  );
	u8g2_DrawStr( p->u8g2, 110-30,1, j );
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
    u8g2_t *u8g2 = p->_display->getBuffer();
    float loadinc = (p->_setup->get()->_ballast +100.0)/100.0;
    float newwl = p->_setup->get()->_polar.wingload * loadinc;
    char val[14];
    sprintf( val,"%0.2f kg/m2", newwl );
    u8g2_DrawStr( u8g2, 110-30,1, val );
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
}


SetupMenu::SetupMenu( std::string title ) {
	printf("SetupMenu::SetupMenu( %s ) \n", title.c_str() );
	printf("this:%x\n", (int)this);
	_rotary->attach(this);
	_title = title;
	highlight = -1;
}

void SetupMenu::begin( DotDisplay* display, ESPRotary * rotary, Setup* my_setup, SetupVolt* my_setupv, BME280_ESP32_SPI * bmp, BatVoltage *adc ){
	printf("SetupMenu() begin\n");
	_rotary = rotary;
	_setup = my_setup;
	_setupv = my_setupv;
	_bmp = bmp;
	_display = display;
	_adc = adc;
	setup();
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

MenuEntry* MenuEntry::findMenu( std::string title, MenuEntry* start )
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
	u8g2 = _display->getBuffer();
	u8g2_ClearBuffer( u8g2 );
	u8g2_SetFont( u8g2,  u8g2_font_ncenR08_tf /* u8g2_font_helvB08_tf */ );
	std::string tit = "<"+selected->_title;
	u8g2_DrawStr( u8g2, 110,1, tit.c_str() );

	u8g2_SetDrawColor( u8g2, 2);
	y=0;
	for (int i=0; i<_childs.size(); i++ ) {
		MenuEntry * child = _childs[i];
		y+=10;
		u8g2_DrawStr( u8g2, 110-y,1, child->_title.c_str() );
	}
	y=0;
	for (int i=-1; i<(int)(_childs.size()); i++ ) {
		if (i == highlight)
			u8g2_DrawBox( u8g2, 110-y, 0 , 10, 64 );
		y+=10;
	}
	y+=14;
	showhelp( y );
	u8g2_SendBuffer(u8g2);
}

void MenuEntry::showhelp( int y ){
	if( helptext != 0 ){
		char buf[32];
		for( int i=0,j=0; i < strlen( helptext ); i++,j++ )
		{
			if( j == 0 && helptext[i] == ' ' ){
				j=-1;
				continue;
			}
			buf[j] = helptext[i];
			buf[j+1] = '\0';
			if( u8g2_GetStrWidth( u8g2, buf ) > 62 ) {
				buf[j] = '\0';
				u8g2_DrawStr( u8g2, 110-y,1, buf );
				j=-1;
				i--;
				y+=11;
			}
		}
		u8g2_DrawStr( u8g2, 110-y,1, buf );
	}
}

void SetupMenu::down(){
	if( (selected != this) || !_menu_enabled )
		return;
	printf("down %d %d\n", highlight, _childs.size() );
	if( highlight  > -1 ){
		highlight --;
	}
	else
		highlight = (int)(_childs.size() -1 );
	pressed = true;
	display();
}

void SetupMenu::up(){
	if( (selected != this) || !_menu_enabled )
		return;
	printf("up %d %d\n", highlight, _childs.size() );
	if( highlight < (int)(_childs.size()-1) ){
		highlight ++;
	}
	else
		highlight = -1;
	pressed = true;
	display();
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
			_display->doMenu();
			Audio.disable();
			_menu_enabled = true;
		}
		else
		{
			_display->setup();
			_display->doMenu(false);
			Audio.disable(false);
			Audio.setup();
			bmpVario.setup();
			_menu_enabled = false;
		}
	}
	display();
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
}

void SetupMenu::setup( )
{
	printf("SetupMenu setup()\n");
	SetupMenu * root = new SetupMenu( "Setup" );
	MenuEntry* mm = root->addMenu( root );

	SetupMenuValFloat * mc = new SetupMenuValFloat(
			"MC", &_setup->get()->_MC, "m/s",	0.01, 10, 0.1,  mc_adj );
	mc->setHelp("Mac Cready value for optimum cruise speed");
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
	SetupMenuValFloat * vda = new SetupMenuValFloat( 	"Damping",
			&_setup->get()->_vario_delay,
			"sec",
			0.5, 10.0,
			0.1 );
	vda->setHelp("Response time, time constant of vario low pass filter");
	vae->addMenu( vda );
	SetupMenuValFloat * vga = new SetupMenuValFloat( 	"Range",
			&_setup->get()->_range,
			"m/s",
			1.0, 30.0,
			1 );
	vga->setHelp("Upper and lower value for graphic display region");
	vae->addMenu( vga );
// Bluetooth
	SetupMenuSelect * bt = new SetupMenuSelect( 	"Bluetooth",
			&_setup->get()->_blue_enable, true );
	bt->addEntry( "OFF");
	bt->addEntry( "ON");
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
		ar->setHelp("Either fix audio or variable with current vario display range");
		ade->addMenu( ar );

	SetupMenu * db = new SetupMenu( "Deadband" );
	MenuEntry* dbe = ade->addMenu( db );
	dbe->setHelp("Audio deadband limits within audio remains silent");

	SetupMenuValFloat * dbminlv = new SetupMenuValFloat( 	"Lower Val",
		&_setup->get()->_deadband_neg,
					"m/s",
					-5.0, 5.0,
					0.1 );
	dbminlv->setHelp("Lower limit for audio mute function");
	dbe->addMenu( dbminlv );
	ade->addMenu( oc );
	SetupMenuValFloat * dbmaxlv = new SetupMenuValFloat( 	"Upper Val",
			&_setup->get()->_deadband,
			"m/s",
			-5.0, 5.0,
			0.1 );
	dbmaxlv->setHelp("Upper limit for audio mute function");
	dbe->addMenu( dbmaxlv );

// Altimeter
	SetupMenuSelect * al = new SetupMenuSelect( 	"Altimeter",
					&_setup->get()->_alt_select );
	mm->addMenu( al );
	al->addEntry( "TE Alt");
	al->addEntry( "Baro Alt");

	SetupMenu * po = new SetupMenu( "Polar" );
	MenuEntry* poe = mm->addMenu( po );
	poe->setHelp( "Polar setup to match performance of glider");

	SetupMenuSelect * glt = new SetupMenuSelect( 	"Glider-Type",
						&_setup->get()->_glider_type, false, polar_select );
	poe->addMenu( glt );

	printf( "Num Polars: %d", Polars::numPolars() );
	for( int x=0; x< Polars::numPolars(); x++ ){
		glt->addEntry( Polars::getPolar(x).type );
	}

	SetupMenu * pa = new SetupMenu( "PolarAdjust" );
	pa->setHelp( "Adjust selected polar");
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

	SetupMenuValFloat * con = new SetupMenuValFloat( 	"Contrast",
					&_setup->get()->_contrast_adj,
		            "%",
					20,100,1, contrast );
	con->setHelp("LCD display contrast in percent");
	sye->addMenu( con );

	Version V;
	static uint8_t select_dummy = 0;
	SetupMenuSelect * ver = new SetupMenuSelect( 	"Version",
					&select_dummy, false );
	ver->addEntry( V.version() );
	sye->addMenu( ver );

	static uint8_t select_dummy2 = 0;
	char * ids = _setup->getID();
	printf( "Setup id=%s\n", ids );
	SetupMenuSelect * idm = new SetupMenuSelect( 	"BT Name",
						&select_dummy2, false );
	idm->addEntry( ids );
	sye->addMenu( idm );

	SetupMenu::display();
}

SetupMenuValFloat::SetupMenuValFloat( std::string title, float *value, std::string unit, float min, float max, float step, int (*action)( SetupMenuValFloat *p ) ) {
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


void SetupMenuValFloat::display( int mode ){
	if( (selected != this) || !_menu_enabled )
		return;
	printf("SetupMenuValFloat display() %d %x\n", pressed, (int)this);
	u8g2 = _display->getBuffer();
	u8g2_ClearBuffer( u8g2 );
	u8g2_SetFont( u8g2,  u8g2_font_ncenR08_tf /* u8g2_font_helvB08_tf */ );
	u8g2_DrawStr( u8g2, 110,1, selected->_title.c_str() );
	int y=16;
    char val[14];
	sprintf( val,"%0.2f %s", *_value, _unit.c_str() );
	u8g2_DrawStr( u8g2, 110-y,1, val );

	if( _action != 0 )
		(*_action)( this );

	y+=28;
	showhelp( y );
	if(mode == 1){
		y+=24;
		u8g2_DrawStr( u8g2, 1,1, "Saved" );
	}
	u8g2_SetDrawColor( u8g2, 2);
	y=0;

	u8g2_SendBuffer(u8g2);
	if( mode == 1 )
	    vTaskDelay(1000 / portTICK_PERIOD_MS);
	printf("~SetupMenuValFloat display\n");
}

void SetupMenuValFloat::down(){
	if( (selected != this) || !_menu_enabled )
		return;
	printf("val down\n");
	if( *_value > _min )
		*_value-=_step;
	display();
}

void SetupMenuValFloat::up(){
	if( (selected != this) || !_menu_enabled )
		return;
	printf("val up\n" );
	if ( *_value < _max )
		*_value+=_step;
	display();
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
		display();
	}
}


SetupMenuSelect::SetupMenuSelect( std::string title, uint8_t *select, bool restart, int (*action)(SetupMenuSelect *p) ) {
	printf("SetupMenuSelect( %s ) \n", title.c_str() );
	_rotary->attach(this);
	_title = title;
	highlight = -1;
	_select = select;
	_select_save = *select;
	_numval = 0;
	_restart = restart;
	_action = action;
}

void SetupMenuSelect::display( int mode ){
	if( (selected != this) || !_menu_enabled )
		return;
	printf("SetupMenuSelect display() %d %x\n", pressed, (int)this);
	u8g2= _display->getBuffer();
	u8g2_ClearBuffer( u8g2 );
	u8g2_SetFont( u8g2,  u8g2_font_ncenR08_tf  );
	u8g2_DrawStr( u8g2, 110,1, selected->_title.c_str() );
	u8g2_SetDrawColor( u8g2, 2);

	printf("select=%d numval=%d\n", *_select, _numval );
	int y=10;
	int start=0;
	if( *_select > 10 )
		start = *_select-9;
	printf("start=%d \n", start );
	for( int i=start; i<_numval && i<(start+10); i++ )	{
		u8g2_DrawStr( u8g2, 110-y,1, _values[i].c_str() );
		if( i == *_select ){
			u8g2_DrawBox( u8g2, 110-y, 0 , 10, 64 );
		}
		y+=10;
	}
	y+=14;
	showhelp( y );
	if(mode == 1){
		u8g2_DrawStr( u8g2, 1,1, "Saved" );
		if( _select_save != *_select )
			if( _restart ) {
				u8g2_DrawStr( u8g2, 110-(y+30),1, "Now Restart" );
			}
	}
	u8g2_SendBuffer(u8g2);
	if( mode == 1 )
		vTaskDelay(1000 / portTICK_PERIOD_MS);

}

void SetupMenuSelect::down(){
	if( (selected != this) || !_menu_enabled )
		return;

	if( (*_select) >  0 )
		(*_select)--;
	printf("val down %d\n", *_select );
	display();
}

void SetupMenuSelect::up(){
	if( (selected != this) || !_menu_enabled )
		return;
	if ( (*_select) < _numval-1 )
		(*_select)++;
	printf("val up %d\n", *_select );
	display();
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
		display();
	}
}
