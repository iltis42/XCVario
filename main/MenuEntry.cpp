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
#include "MenuEntry.h"

AdaptUGC *MenuEntry::ucg = 0;
IpsDisplay* MenuEntry::_display = 0;
MenuEntry* MenuEntry::root = 0;
MenuEntry* MenuEntry::selected = 0;
AnalogInput* MenuEntry::_adc = 0;
PressureSensor *MenuEntry::_bmp = 0;
bool MenuEntry::_restart = false;

MenuEntry::~MenuEntry()
{
    // ESP_LOGI(FNAME,"del menu %s",_title );
    detach(this);
}

const MenuEntry* MenuEntry::findMenu(const char *title) const
{
	ESP_LOGI(FNAME,"MenuEntry findMenu() %s %p", title, this );
	if( std::strcmp(_title, title) == 0 ) {
		ESP_LOGI(FNAME,"Menu entry found for start %s", title );
		return this;
	}
	return nullptr;
}

void MenuEntry::uprintf( int x, int y, const char* format, ...) {
	if( ucg == 0 ) {
		ESP_LOGE(FNAME,"Error UCG not initialized !");
		return;
	}
	va_list argptr;
	va_start(argptr, format);
	ucg->setPrintPos(x,y);
	ucg->printf( format, argptr );
	va_end(argptr);
}

void MenuEntry::restart(){
	Audio::shutdown();
	clear();
	ucg->setPrintPos( 10, 50 );
	ucg->print("...rebooting now" );
	delay(2000);
	esp_restart();
}

void MenuEntry::uprint( int x, int y, const char* str ) {
	if( ucg == 0 ) {
		ESP_LOGE(FNAME,"Error UCG not initialized !");
		return;
	}
	ucg->setPrintPos(x,y);
	ucg->print( str );
}

void MenuEntry::showhelp(){
	if( helptext != 0 ){
		int w=0;
		char *buf = (char *)malloc(512);
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
			ucg->setPrintPos(x, y);
			ucg->print( words[p] );
			x+=len+5;
		}
		free( buf );
	}
}

void MenuEntry::clear()
{
	// ESP_LOGI(FNAME,"MenuEntry::clear");
	ucg->setColor(COLOR_BLACK);
	ucg->drawBox( 0,0,240,320 );
	// ucg->begin(UCG_FONT_MODE_SOLID);
	ucg->setFont(ucg_font_ncenR14_hr);
	ucg->setPrintPos( 1, 30 );
	ucg->setColor(COLOR_WHITE);
}

