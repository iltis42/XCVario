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
#include "S2F.h"
#include "Version.h"
#include "Polars.h"

IpsDisplay* MenuEntry::_display = 0;
MenuEntry* MenuEntry::root = 0;
MenuEntry* MenuEntry::selected = 0;
ESPRotary* MenuEntry::_rotary = 0;
BatVoltage* MenuEntry::_adc = 0;
BME280_ESP32_SPI *MenuEntry::_bmp = 0;
float MenuEntry::volume;
bool      MenuEntry::_menu_enabled = false;
extern S2F Speed2Fly;
extern xSemaphoreHandle spiMutex;
Ucglib_ILI9341_18x240x320_HWSPI *MenuEntry::ucg = 0;
static char rentry[25];
SetupMenuSelect * audio_range_sm = 0;


int update_rentry(SetupMenuValFloat * p)
{
	char rentry[25];
    sprintf( rentry, "Variable (%d m/s)", (int)(range.get()) );
    audio_range_sm->updateEntry( rentry, 1 );
    return 0;
}

// Action Routines
int contrast( SetupMenuValFloat * p )
{
	printf("contrast( %f ) \n", *p->_value );
	return 0;
}

int qnh_adj( SetupMenuValFloat * p )
{
	// printf("qnh_adj");
	float alt=0;
	for( int i=0; i<6; i++ ) {
		alt += p->_bmp->readAltitude( *(p->_value) );
		sleep(0.01);
	}
	alt = alt/6;

	printf("Setup BA alt=%f QNH=%f\n", alt, *(p->_value)  );
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
	// printf("elev_adj");
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
	printf("factv_adj");
	float bat = p->_adc->getBatVoltage(true);
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
	int cs = OpenVario::getCheckSum(&mc[1]);
	int i = strlen(mc);
	sprintf( &mc[i], "*%02X\n", cs );
    btsender.send(mc);
#endif
    return 0;
}

int vol_adj( SetupMenuValFloat * p ){
	Audio.setVolume( (int(p->volume*1.270)));
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
	// printf("SetupMenu::SetupMenu( %s ) \n", title.c_str() );
	_rotary->attach(this);
	_title = title;
	highlight = -1;
}

void SetupMenu::begin( IpsDisplay* display, ESPRotary * rotary, BME280_ESP32_SPI * bmp, BatVoltage *adc ){
	printf("SetupMenu() begin\n");
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
	// printf("MenuEntry addMenu() title %s\n", item->_title.c_str() );
	if( root == 0 ){
		printf("Init root menu\n");
		root = item;
		item->_parent = 0;
		selected = item;
		return item;
	}
	else{
		// printf("add to childs\n");
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
		int y=hypos;
		ucg->setFont(ucg_font_ncenR14_hr);
		for( int p=0; p<w; p++ )
		{
			int len = ucg->getStrWidth( words[p] );
			// printf("showhelp pix len word #%d = %d, %s \n", p, len, words[p]);
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
	Audio.decVolume(count);
}

void inc_volume( int count ) {
	Audio.incVolume(count);
}


void SetupMenu::down(int count){
	if( selected == this && !_menu_enabled ) {
		printf("root: down\n");
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
}

void SetupMenu::up(int count){
	if( selected == this && !_menu_enabled ) {
		printf("root: up\n");
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
}

void SetupMenu::longPress(){
	long_pressed = true;
	printf("SetupMenue: longPress\n");
	// Audio.disable();
	// delay(1000);
	// esp_restart();
}


void SetupMenu::press(){
	if( selected == 0 )
		selected = root;
	if( (selected != this) )
		return;
	printf("SetupMenu press p:%d h:%d parent:%x\n", pressed, highlight, (int)_parent );
	// main menue behavior, extra class maybe
	// for now here we catch this
	// this is just for the main menu,
	if( (_parent == 0) && (highlight == -1) )
	{
		if( !pressed )
		{
			printf("!pressed\n");
			Audio.disable();
			delay( 500 );
			_menu_enabled = true;
		}
		else
		{
			printf("pressed\n");
			_display->setup();
			_display->initDisplay();
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

	SetupMenuValFloat * mc = new SetupMenuValFloat( "MC", 0, "m/s",	0.0, 9.9, 0.1,  mc_adj, true, &MC );
	mc->setHelp(PROGMEM"Default Mac Cready value for optimum cruise speed, or average climb rate, MC is provided in usual metric system means");
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


// Vario
	SetupMenu * va = new SetupMenu( "Vario" );
	MenuEntry* vae = mm->addMenu( va );

	String vunit;
	if( vario_unit.get() == 0 )
		vunit = "m/s";
	else if( vario_unit.get() == 1 )
		vunit = "x 100ft/m";
	else if( vario_unit.get() == 2 )
		vunit = "kt";
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

    SetupMenuValFloat * vcch = new SetupMenuValFloat( "Mean Climb Period", 0,	"min", 1, 300, 1, 0, false, &core_climb_history );
    vcch->setHelp(PROGMEM"Number of minutes where samples for mean climb value are regarded, default is last 3 thermals a 15 min");
    vae->addMenu( vcch );


	SetupMenuSelect * sink = new SetupMenuSelect( 	"Polar Sink Display", 0, false, 0 , true, &ps_display );
	sink->setHelp(PROGMEM"Show polar sink rate together with TE in Vario bar");
	sink->addEntry( "DISABLE");
	sink->addEntry( "ENABLE");
	vae->addMenu( sink );



// Audio
	SetupMenu * ad = new SetupMenu( "Audio" );
	MenuEntry* ade = mm->addMenu( ad );

	SetupMenuValFloat * dv = new SetupMenuValFloat( "Default Volume", 0, "%", 0, 100, 1.0, 0, false, &default_volume );
	ade->addMenu( dv );
	dv->setHelp(PROGMEM"Default volume for Audio when device is switched on");

	SetupMenuSelect * dt = new SetupMenuSelect( 	"Tone Style", 0, false, 0 , true, &dual_tone );
	dt->setHelp(PROGMEM"Select dual tone aka ilec sound, (di/da/di) or single tone (di-di-di) mode");
	dt->addEntry( "Single Tone");      // 0
	dt->addEntry( "Dual Tone");        // 1
	ade->addMenu( dt );

	SetupMenuSelect * tch = new SetupMenuSelect( 	"Tone Chopping", 0, false, 0 , true, &chopping_mode );
	tch->setHelp(PROGMEM"Select tone chopping option for Vario and or S2F");
	tch->addEntry( "Disabled");             // 0
	tch->addEntry( "Vario only");           // 1
	tch->addEntry( "S2F only");             // 2
	tch->addEntry( "Vario and S2F");        // 3  default
	ade->addMenu( tch );

	SetupMenuValFloat * htv = new SetupMenuValFloat( 	"Dual Tone Pich", 0, "%", 0, 50, 1.0, 0, false, &high_tone_var );
	htv->setHelp(PROGMEM"Tone variation in Dual Tone mode, percent of frequency pitch up for second tone");
	ade->addMenu( htv );


	SetupMenuSelect * am = new SetupMenuSelect( 	"Audio Mode", 0, false, 0 , true, &audio_mode );
	am->setHelp( PROGMEM"Selects audio source. Audio either follows Vario, or S2F exclusively, controlled by external switch or automatically by speed" );
		am->addEntry( "Vario");
		am->addEntry( "S2F");
		am->addEntry( "Switch");
		am->addEntry( "AutoSpeed");
	ade->addMenu( am );

	SetupMenuValFloat * ts = new SetupMenuValFloat( 	"AutoSpeed", 0, "km/h", 30.0, 200.0, 1.0, 0, false, &s2f_speed );
	ade->addMenu( ts );
	ts->setHelp(PROGMEM"Transition speed when audio changes to S2F mode in AutoSpeed mode in metric system. 100 km/h equals rounfd 62 mph or 54 knots");

	SetupMenuValFloat * cf = new SetupMenuValFloat( 	"CenterFreq", 0,	"Hz", 200.0, 2000.0, 10.0, 0, false, &center_freq );
	cf->setHelp(PROGMEM"Center frequency for Audio at zero Vario or zero S2F delta");
	ade->addMenu( cf );
	SetupMenuValFloat * oc = new SetupMenuValFloat( 	"Octaves", 0, "fold", 1.5, 4, 0.1, 0, false, &tone_var );
	oc->setHelp(PROGMEM"Maximum tone frequency variation");
	ade->addMenu( oc );


	SetupMenuSelect * ar = new SetupMenuSelect( 	"Range", 0, false, 0 , true, &audio_range  );

	audio_range_sm = ar;
	sprintf( rentry, "Variable (%d m/s)", (int)(range.get()) );
	ar->addEntry( "Max eq. 5 m/s");
	ar->addEntry( "Max eq. 10 m/s");
	ar->addEntry( rentry );
	ar->setHelp(PROGMEM"Select either fix (5m/s) or variable Audio range according to current vario setting");
	ade->addMenu( ar );

	SetupMenu * db = new SetupMenu( "Deadband" );
	MenuEntry* dbe = ade->addMenu( db );
	dbe->setHelp(PROGMEM"Audio dead band limits within Audio remains silent in metric scale. 0,1 m/s equals roughtly 20 ft/min or 0.2 knots");

	SetupMenuValFloat * dbminlv = new SetupMenuValFloat( 	"Lower Value Vario", 0,	"m/s", -5.0, 0, 0.1, 0 , false, &deadband_neg  );
	dbminlv->setHelp(PROGMEM"Lower limit for Audio mute function");
	dbe->addMenu( dbminlv );

	SetupMenuValFloat * dbmaxlv = new SetupMenuValFloat( 	"Upper Value Vario", 0,	"m/s", 0, 5.0, 0.1, 0 , false, &deadband );
	dbmaxlv->setHelp(PROGMEM"Upper limit for Audio mute function");
	dbe->addMenu( dbmaxlv );

	SetupMenuValFloat * dbmaxls2f = new SetupMenuValFloat( 	"S2F Deadband", 	0, "+-km/h", 0, 25.0, 1, 0 , false, &s2f_deadband );
	dbmaxls2f->setHelp(PROGMEM"Positive and negative limit in speed deviation for S2F mute function");
	dbe->addMenu( dbmaxls2f );



// Polar Setup
	SetupMenu * po = new SetupMenu( "Polar" );
	po->setHelp( PROGMEM"Polar setup to match performance of glider");
	MenuEntry* poe = mm->addMenu( po );


	SetupMenuSelect * glt = new SetupMenuSelect( 	"Glider-Type",	0, false, polar_select, true, &glider_type );
	poe->addMenu( glt );

	printf( "Number of Polars installed: %d\n", Polars::numPolars() );
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
	SetupMenuValFloat * pov2 = new SetupMenuValFloat( "Speed 2", 0, "km/h", 100.0, 180.0, 1, polar_adj, false, &polar_speed2 );
	pov2->setHelp(PROGMEM"Speed 2 for a moderate cruise from polar e.g. 120 km/h");
	pa->addMenu( pov2 );
	SetupMenuValFloat * pos2 = new SetupMenuValFloat( "Sink  2", 0, "m/s", -5.0, 0.0, 0.01, polar_adj, false, &polar_sink2 );
	pos2->setHelp(PROGMEM"Sink indication at Speed 2 from polar");
	pa->addMenu( pos2 );
	SetupMenuValFloat * pov3 = new SetupMenuValFloat( "Speed 3", 0, "km/h", 120.0, 250.0, 1, polar_adj, false, &polar_speed3 );
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



	SetupMenu * wk = new SetupMenu( "Flap (WK) Indicator" );
	MenuEntry* wkm = mm->addMenu( wk );
	SetupMenuSelect * wke = new SetupMenuSelect( "Flap Indicator Option", 0, true, 0, true, &flap_enable );
	wke->addEntry( "Disable");
	wke->addEntry( "Enable");
	wke->setHelp(PROGMEM"Option to enable Flap (WK) Indicator to assist optimum flap setting depending on speed and ballast");
	wkm->addMenu( wke );

	SetupMenuValFloat * plus1 = new SetupMenuValFloat("Speed +2 to +1", 0, "km/h",  50, 150, 1, 0, false, &flap_plus_1  );
	plus1->setHelp(PROGMEM"Speed for transition from +2 to +1 flap setting");
	wkm->addMenu( plus1 );
	SetupMenuValFloat * zero = new SetupMenuValFloat("Speed +1 to 0", 0, "km/h",  50, 150, 1, 0, false, &flap_0  );
	zero->setHelp(PROGMEM"Speed for transition from +1 to 0 flap setting");
	wkm->addMenu( zero );
	SetupMenuValFloat * min1 = new SetupMenuValFloat("Speed 0 to -1", 0, "km/h",  80, 180, 1, 0, false, &flap_minus_1  );
	min1->setHelp(PROGMEM"Speed for transition from 0 to -1 flap setting");
	wkm->addMenu( min1 );
	SetupMenuValFloat * min2 = new SetupMenuValFloat("Speed -1 to -2", 0, "km/h",  100, 280, 1, 0, false, &flap_minus_2  );
	min2->setHelp(PROGMEM"Speed for transition from -1 to -2 flap setting");
	wkm->addMenu( min2 );

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

	// Bluetooth
	String btname="Bluetooth   ";
	btname += SetupCommon::getID();

	SetupMenuSelect * btm = new SetupMenuSelect(  btname, 0, true, 0, true, &blue_enable );
	btm->addEntry( "Sender OFF");
	btm->addEntry( "Sender ON");
	sye->addMenu( btm );

	float fva = factory_volt_adjust.get();
	if( abs(fva - 0.00815) < 0.00001 ) {
		printf("Add option to Factory adjust ADC; not yet done\n");
		SetupMenuValFloat * fvoltadj = new SetupMenuValFloat( 	"Factory Voltmeter Adj", 0, "%",	-25.0, 25.0, 0.01, factv_adj, false, &factory_volt_adjust );
		fvoltadj->setHelp(PROGMEM "Option to fine factory adjust voltmeter");
		sye->addMenu( fvoltadj );
	}

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

	SetupMenu * display = new SetupMenu( "DISPLAY Setup" );
	sye->addMenu( display );
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
    // Orientation   _display_orientation
	SetupMenuSelect * diso = new SetupMenuSelect( "Display Orientation", 0, true , 0, true, &display_orientation );
	display->addMenu( diso );
	diso->setHelp( PROGMEM "Display Orientation either NORMAL means control panel is right, or TOPDOWN means control panel is left");
	diso->addEntry( "NORMAL (Rotary left)");
	diso->addEntry( "TOPDOWN (Rotary right)");

	// Altimeter, IAS
	SetupMenu * aia = new SetupMenu( "Altimeter, Airspeed" );
	sye->addMenu( aia );
	SetupMenuSelect * als = new SetupMenuSelect( "Altimeter Source",	0, false, 0, true, &alt_select );
	aia->addMenu( als );
	als->setHelp( PROGMEM "Select Source of altimeter to either barometric or static pressure sensor (default), or TE sensor what results in an 'energy' altitude");
	als->addEntry( "TE   Sensor");
	als->addEntry( "Baro Sensor");

	SetupMenuSelect * atl = new SetupMenuSelect( "Automatic Transition",	0, false, 0, true, &fl_auto_transition );
	aia->addMenu( atl );
	atl->setHelp( PROGMEM "Option to enable automatic altitude transition to QNH Standard (1013.25) above 'Transition Altitude'");
	atl->addEntry( "Disable");
	atl->addEntry( "Enable");

	SetupMenuValFloat * tral = new SetupMenuValFloat( "Transition Altitude", 0, "FL", 0, 400, 10, 0, false, &transition_alt  );
	tral->setHelp(PROGMEM"Transition altitude (or transition height, when using QFE) is the altitude/height above which standard pressure (QNE) is set (1013.2 mb/hPa)");
	aia->addMenu( tral );

	SetupMenuSelect * atr = new SetupMenuSelect( "Transition Level",	0, false, 0, true, &alt_select );
	aia->addMenu( atr );
	atr->setHelp( PROGMEM "Option to enable automatic transition to QNH Standard (1013.25) above an altitude of 5000 ft");
	atr->addEntry( "Disable");
	atr->addEntry( "Enable");

	SetupMenuValFloat * spc = new SetupMenuValFloat( "IAS Calibration", 0, "%", -10, 10, 1, 0, false, &speedcal  );
	spc->setHelp(PROGMEM"Calibration of indicated airspeed (IAS). Normally not needed, hence pressure probes may have systematic error");
	aia->addMenu( spc );

	SetupMenuSelect * amode = new SetupMenuSelect( "Airspeed Mode",	0, false, 0, true, &airspeed_mode );
	aia->addMenu( amode );
	amode->setHelp( PROGMEM "Select mode of Airspeed indicator to display IAS (Indicated AirSpeed, default) or TAS (True AirSpeed) considering air density");
	amode->addEntry( "IAS");
	amode->addEntry( "TAS");


	// Units
	SetupMenu * un = new SetupMenu( "Units" );
	un->setHelp( PROGMEM "Setup altimeter, airspeed indicator and variometer with European Metric, American, British or Australian units");
	SetupMenuSelect * alu = new SetupMenuSelect( 	"Altimeter",	0, true,  0, true, &alt_unit );
	alu->addEntry( "Meter       (m)");
	alu->addEntry( "Foot        (ft)");
	alu->addEntry( "FlightLevel (FL)");
	un->addMenu( alu );
	SetupMenuSelect * iau = new SetupMenuSelect( "Indicated Airspeed", 0, true , 0, true, &ias_unit );
	iau->addEntry( "Km per hour     (Km/h)");
	iau->addEntry( "Miles  per hour (mph)");
	iau->addEntry( "Knots               (kt)");
	un->addMenu( iau );
	SetupMenuSelect * vau = new SetupMenuSelect( "Vario", 0, true , 0, true, &vario_unit );
	vau->addEntry( "Meters/sec   (m/s)");
	vau->addEntry( "Foot per min (ft/min)");
	vau->addEntry( "Knots        (knots)");
	un->addMenu( vau );
	sye->addMenu( un );

	// Rotary Default
	SetupMenuSelect * rd = new SetupMenuSelect( "Rotary Default", 0, false, 0, true, &rot_default );
	sye->addMenu( rd );
	rd->setHelp(PROGMEM "Select value to be altered at rotary movement outside of setup menu");
	rd->addEntry( "Volume");
	rd->addEntry( "MC Value");

	// _serial2_speed
	SetupMenu * rs232 = new SetupMenu( "RS232 Serial Interface" );
	sye->addMenu( rs232 );
	SetupMenuSelect * s2sp = new SetupMenuSelect( PROGMEM "Serial RS232 Speed",	0, false, 0, true, &serial2_speed );
	rs232->addMenu( s2sp );
	// s2sp->setHelp( "Serial RS232 (TTL) speed, pins RX:2, TX:3 on external RJ45 connector");
	s2sp->addEntry( "Serial OFF");
	s2sp->addEntry( "4800 baud");
	s2sp->addEntry( "9600 baud");
	s2sp->addEntry( "19200 baud");
	s2sp->addEntry( "38400 baud");
	s2sp->addEntry( "57600 baud");
	s2sp->addEntry( "115200 baud");
	SetupMenuSelect * s2lo = new SetupMenuSelect( PROGMEM "Serial BT Bridge", 0, false, 0, true, &serial2_rxloop );
	rs232->addMenu( s2lo );
	s2lo->setHelp( "Serial RS232 (TTL) option to retransmit data on serial RX (pin2 RJ45), to bluetooth sender to connect e.g. FLARM");
	s2lo->addEntry( "Disable");
	s2lo->addEntry( "Enable");

	SetupMenuSelect * sout = new SetupMenuSelect( PROGMEM "Serial TX", 0, false, 0, true, &serial2_tx );
	rs232->addMenu( sout );
	sout->setHelp( "Serial RS232 (TTL) option to transmit OpenVario NMEA and or data from BT on serial TX (pin3 RJ45) to connect serial devices");
	sout->addEntry( "Disable all");
	sout->addEntry( "Enable OpenVario");
	sout->addEntry( "Enable BT Bridge");
	sout->addEntry( "Enable all");

	SetupMenuSelect * stxi = new SetupMenuSelect( PROGMEM "Serial TX Inversion", 0, true , 0, true, &serial2_tx_inverted );
	rs232->addMenu( stxi );
	stxi->setHelp( "Serial RS232 (TTL) option for negative logic, means a '1' will be sent at zero level (RS232 standard and default) and vice versa");
	stxi->addEntry( "Normal");
	stxi->addEntry( "Inverted");

	SetupMenuSelect * srxi = new SetupMenuSelect( PROGMEM "Serial RX Inversion", 0, true, 0, true, &serial2_rx_inverted );
	rs232->addMenu( srxi );
	srxi->setHelp( "Serial RS232 (TTL) option for negative logic, means a '1' will be received at zero level (RS232 standard and default) and vice versa");
	srxi->addEntry( "Normal");
	srxi->addEntry( "Inverted");

	SetupMenuSelect * nmea = new SetupMenuSelect( PROGMEM "NMEA Protocol", 0, false , 0, true, &nmea_protocol );
	sye->addMenu( nmea );
	nmea->setHelp( "Setup protocol used for NMEA sending what corresponds to the driver used in OpenVario");
	nmea->addEntry( "OpenVario");
	nmea->addEntry( "Borgelt");

	SetupMenu::display();
}

SetupMenuValFloat::SetupMenuValFloat( String title, float *value, String unit, float min, float max, float step, int (*action)( SetupMenuValFloat *p ), bool end_menu, SetupNG<float> *anvs ) {
	// printf("SetupMenuValFloat( %s ) \n", title.c_str() );
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
	printf("MenuEntry::clear\n");
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
	printf("SetupMenuValFloat::showQnhMenu()\n");
	if( qnh_menu ) {
		printf("qnh_menu = true\n");
		_menu_enabled = true;
		selected = qnh_menu;
		Audio.disable();
		qnh_menu->clear();
		qnh_menu->display();
		qnh_menu->pressed = true;
	}
}

void SetupMenuValFloat::display( int mode ){
	if( (selected != this) || !_menu_enabled )
		return;
	// printf("SetupMenuValFloat display() %d %x\n", pressed, (int)this);
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
	printf("~SetupMenuValFloat display\n");
}

void SetupMenuValFloat::displayVal()
{
	ucg->setFont(ucg_font_fub25_hr);
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setPrintPos( 1, 70 );

	ucg->printf("%0.*f %s   ", _precision, *_value, _unit.c_str());
	xSemaphoreGive(spiMutex );
	ucg->setFont(ucg_font_ncenR14_hr);
}

void SetupMenuValFloat::down( int count ){
	if( (selected != this) || !_menu_enabled )
		return;
	printf("val down %d times \n", count );
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
	printf("val up %d times \n", count );
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
	printf("SetupMenuValFloat press\n");
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
	printf("SetupMenuSelect( %s ) \n", title.c_str() );
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
		printf("_nvs->key(): %s val: %d\n", _nvs->key(), (int)(_nvs->get()) );
		_select = _nvs->getPtr();
		_select_save = _nvs->get();
	}
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
		printf("val down %d\n", *_select );
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
		printf("val up %d\n", *_select );
		ucg->drawFrame( 1,(*_select+1)*25+3,238,25 );  // draw new frame
		xSemaphoreGive(spiMutex );
	}
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
