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


DotDisplay* MenuEntry::_display = 0;
MenuEntry* MenuEntry::root = 0;
MenuEntry* MenuEntry::selected = 0;
ESPRotary* MenuEntry::_rotary = 0;
SetupCMD* MenuEntry::_setup = 0;
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
	printf( "%4d %s", (int)(alt+0.5), "m"  );
	u8g2_DrawStr( p->u8g2, 110-40,1, j );
	return 0;
}

int polar_adj( SetupMenuValFloat * p )
{
    s2f.change_polar();
    return 0;
}

int mc_bal_adj( SetupMenuValFloat * p )
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


void SetupMenu::begin( DotDisplay* display, ESPRotary * rotary, SetupCMD* my_setup, BME280_ESP32_SPI * bmp ){
	printf("SetupMenu() begin\n");
	_rotary = rotary;
	_setup = my_setup;
	_bmp = bmp;
	_display = display;
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
	u8g2_SendBuffer(u8g2);
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
			"MC", &_setup->get()->_MC, "MC",	0.0, 10, 0.1,  mc_bal_adj );
	mm->addMenu( mc );

	SetupMenuValFloat * qnh = new SetupMenuValFloat(
			"QNH", &_setup->get()->_QNH, "hPa",	900.0, 1100.0, 0.2, qnh_adj );
	mm->addMenu( qnh );

	SetupMenuValFloat * bal = new SetupMenuValFloat(
				"Ballast", &_setup->get()->_ballast, "%", 0.0, 100, 1, mc_bal_adj  );
	mm->addMenu( bal );


	SetupMenu * ad = new SetupMenu( "Audio" );
	MenuEntry* ade = mm->addMenu( ad );



	SetupMenu * va = new SetupMenu( "Vario" );
	MenuEntry* vae = mm->addMenu( va );
	SetupMenuValFloat * vda = new SetupMenuValFloat( 	"Damping",
			&_setup->get()->_vario_delay,
			"sec",
			0.5, 10.0,
			0.1 );
	vae->addMenu( vda );
	SetupMenuValFloat * vga = new SetupMenuValFloat( 	"Range",
			&_setup->get()->_range,
			"m/s",
			1.0, 30.0,
			1 );
	vae->addMenu( vga );

	SetupMenuSelect * bt = new SetupMenuSelect( 	"Bluetooth",
			&_setup->get()->_blue_enable, true );
	bt->addEntry( "OFF");
	bt->addEntry( "ON");
	mm->addMenu( bt );

	SetupMenuValFloat * cf = new SetupMenuValFloat( 	"CenterFreq",
			&_setup->get()->_center_freq,
			"Hz",
			200.0, 2000.0,
			10.0 );
	ade->addMenu( cf );

	SetupMenu * db = new SetupMenu( "Deadband" );
	MenuEntry* dbe = ade->addMenu( db );

	SetupMenuValFloat * dbminlv = new SetupMenuValFloat( 	"Lower Val",
			&_setup->get()->_deadband_neg,
			"m/s",
			-5.0, 5.0,
			0.1 );
	dbe->addMenu( dbminlv );

	SetupMenuValFloat * oc = new SetupMenuValFloat( 	"Octaves",
			&_setup->get()->_tone_var,
			"fold",
			1.5, 4,
			0.1 );

	ade->addMenu( oc );
	SetupMenuValFloat * dbmaxlv = new SetupMenuValFloat( 	"Upper Val",
			&_setup->get()->_deadband,
			"m/s",
			-5.0, 5.0,
			0.1 );
	dbe->addMenu( dbmaxlv );

	SetupMenuSelect * ar = new SetupMenuSelect( 	"Range",
					&_setup->get()->_audio_range );
	ar->addEntry( "Fix 5 m/s");
	ar->addEntry( "Variable");
	ade->addMenu( ar );


	SetupMenuSelect * al = new SetupMenuSelect( 	"Altimeter",
					&_setup->get()->_alt_select );
	mm->addMenu( al );
	al->addEntry( "TE Alt");
	al->addEntry( "Baro Alt");

	SetupMenu * po = new SetupMenu( "Polar" );
	MenuEntry* poe = mm->addMenu( po );

	SetupMenuValFloat * pov1 = new SetupMenuValFloat(
			"Speed 1", &_setup->get()->_v1, "km/h", 50.0, 120.0, 1, polar_adj );
	poe->addMenu( pov1 );
	SetupMenuValFloat * pos1 = new SetupMenuValFloat(
			"Sink  1", &_setup->get()->_w1, "m/s", -3.0, 0.0, 0.01, polar_adj );
	poe->addMenu( pos1 );
	SetupMenuValFloat * pov2 = new SetupMenuValFloat(
			"Speed 2", &_setup->get()->_v2, "km/h", 100.0, 180.0, 1, polar_adj );
	poe->addMenu( pov2 );
	SetupMenuValFloat * pos2 = new SetupMenuValFloat(
			"Sink  2", &_setup->get()->_w2, "m/s", -5.0, 0.0, 0.01, polar_adj );
	poe->addMenu( pos2 );
	SetupMenuValFloat * pov3 = new SetupMenuValFloat(
			"Speed 3", &_setup->get()->_v3, "km/h", 120.0, 250.0, 1, polar_adj );
	poe->addMenu( pov3 );
	SetupMenuValFloat * pos3 = new SetupMenuValFloat(
			"Sink  3", &_setup->get()->_w3, "m/s", -6.0, 0.0, 0.01, polar_adj );
	poe->addMenu( pos3 );

	SetupMenu * sy = new SetupMenu( "System" );
	MenuEntry* sye = mm->addMenu( sy );

	SetupMenuValFloat * voltadj = new SetupMenuValFloat( 	"BatV Adj",
				&_setup->get()->_voltmeter_adj,
				"Volt",
				-1.2, 1.2,
				0.1 );

	sye->addMenu( voltadj );

	SetupMenuSelect * fa = new SetupMenuSelect( 	"Factory",
				&_setup->get()->_factory_reset, true );
	fa->addEntry( "Cancel");
	fa->addEntry( "ResetAll");
	sye->addMenu( fa );

	SetupMenuValFloat * con = new SetupMenuValFloat( 	"Contrast",
					&_setup->get()->_contrast_adj,
		            "%",
					0,100,1, contrast );
	sye->addMenu( con );

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
	int y=20;
    char val[14];
	sprintf( val,"%0.2f %s", *_value, _unit.c_str() );
	u8g2_DrawStr( u8g2, 110-y,1, val );

	if( _action != 0 )
		(*_action)( this );
    y+=40;
	if(mode == 1){
		u8g2_DrawStr( u8g2, 110-y,1, "Saved" );
	}

	u8g2_SetDrawColor( u8g2, 2);
	y=10;
	if( pressed )
		y=0;
	u8g2_DrawBox( u8g2, 110-y, 0 , 10, 64 );
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
		pressed = false;
	}
	else
	{
		pressed = true;
		display();
	}
}


SetupMenuSelect::SetupMenuSelect( std::string title, uint8_t *select, bool restart ) {
	printf("SetupMenuSelect( %s ) \n", title.c_str() );
	_rotary->attach(this);
	_title = title;
	highlight = -1;
	_select = select;
	_select_save = *select;
	_numval = 0;
	_restart = restart;
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
	int y=10;
	for( int i=0; i<_numval; i++ )	{
		u8g2_DrawStr( u8g2, 110-y,1, _values[i].c_str() );
		if( i == *_select ){
			u8g2_DrawBox( u8g2, 110-y, 0 , 10, 64 );
		}
		y+=10;
	}
	if(mode == 1){
		u8g2_DrawStr( u8g2, 110-(y+10),1, "Saved" );
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
