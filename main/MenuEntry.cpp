/*
 * SetupMenu.cpp
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */
#include "MenuEntry.h"

#include "SetupMenu.h"
#include "IpsDisplay.h"
#include "ESPAudio.h"
#include "BMPVario.h"
#include "S2F.h"
#include "Version.h"
#include "Polars.h"
#include "sensor.h"
#include "Cipher.h"
#include "Units.h"
#include "Switch.h"
#include "Flap.h"
#include "logdef.h"

#include <inttypes.h>
#include <iterator>
#include <algorithm>

MenuEntry* MenuEntry::selected = nullptr;
bool MenuEntry::_restart = false;

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
	va_list argptr;
	va_start(argptr, format);
	MYUCG->setPrintPos(x,y);
	MYUCG->printf( format, argptr );
	va_end(argptr);
}

void MenuEntry::reBoot(){
	Audio::shutdown();
	clear();
	MYUCG->setPrintPos( 10, 50 );
	MYUCG->print("...rebooting now" );
	delay(2000);
	esp_restart();
}

void MenuEntry::uprint( int x, int y, const char* str ) {
	MYUCG->setPrintPos(x,y);
	MYUCG->print( str );
}

void MenuEntry::SavedDelay(bool showit)
{
	if ( showit ) {
		MYUCG->setColor( COLOR_BLACK );
		MYUCG->drawBox( 1,280,240,40 );
		MYUCG->setPrintPos( 1, 300 );
		MYUCG->setColor( COLOR_WHITE );
		MYUCG->print("Saved");
	}
	vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void MenuEntry::enter()
{
	attach();
	selected = this;
	display();
}

void MenuEntry::exit(int ups)
{
	if ( ups != 0 && _parent != 0 ) {
		detach();
		selected = _parent;
		selected->exit(--ups);
		return;
	}
	display();
}

void MenuEntry::regParent(SetupMenu *p)
{
	if ( _parent == nullptr ) {
		_parent = p;
	}
}

void MenuEntry::showhelp()
{
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
		MYUCG->setFont(ucg_font_ncenR14_hr);
		for( int p=0; p<w; p++ )
		{
			int len = MYUCG->getStrWidth( words[p] );
			// ESP_LOGI(FNAME,"showhelp pix len word #%d = %d, %s ", p, len, words[p]);
			if( x+len > 239 ) {   // does still fit on line
				y+=25;
				x=1;
			}
			MYUCG->setPrintPos(x, y);
			MYUCG->print( words[p] );
			x+=len+5;
		}
		free( buf );
	}
}

void MenuEntry::clear()
{
	MYUCG->setColor(COLOR_BLACK);
	MYUCG->drawBox( 0,0,240,320 );
	// MYUCG->begin(UCG_FONT_MODE_SOLID);
	MYUCG->setFont(ucg_font_ncenR14_hr);
	MYUCG->setPrintPos( 1, 30 );
	MYUCG->setColor(COLOR_WHITE);
}

