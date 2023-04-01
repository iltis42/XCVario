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
    for ( MenuEntry* c : _childs ) {
        delete c;
        c = nullptr;
    }
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
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setPrintPos(x,y);
	ucg->print( str );
	xSemaphoreGive(spiMutex );
}

MenuEntry* MenuEntry::getFirst() const {
	// ESP_LOGI(FNAME,"MenuEntry::getFirst()");
	return _childs.front();
}

MenuEntry* MenuEntry::addEntry( MenuEntry * item ) {
	// ESP_LOGI(FNAME,"MenuEntry addMenu() title %s", item->_title );
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

MenuEntry* MenuEntry::addEntry( MenuEntry * item, const MenuEntry* after ) {
	// ESP_LOGI(FNAME,"AddMenuEntry title %s after %s", item->_title, after->_title );
	if( root == 0   ){
		return addEntry(item);
	}
	else{
        std::vector<MenuEntry *>::iterator position = std::find(_childs.begin(), _childs.end(), after );
        if (position != _childs.end()) {
            item->_parent = this;
            _childs.insert( ++position, item );
            return item;
        }
        else { return addEntry(item); }
	}
}


void MenuEntry::delEntry( MenuEntry * item ) {
	ESP_LOGI(FNAME,"MenuEntry delMenu() title %s", item->_title );
	std::vector<MenuEntry *>::iterator position = std::find(_childs.begin(), _childs.end(), item );
	if (position != _childs.end()) { // == myVector.end() means the element was not found
		ESP_LOGI(FNAME,"found entry, now erase" );
		_childs.erase(position);
        delete *position;
	}
}

MenuEntry* MenuEntry::findMenu( std::string title, MenuEntry* start )
{
	ESP_LOGI(FNAME,"MenuEntry findMenu() %s %x", title.c_str(), (uint32_t)start );
	if( std::string(start->_title) == title ) {
		ESP_LOGI(FNAME,"Menu entry found for start %s", title.c_str() );
		return start;
	}
	for(MenuEntry* child : start->_childs) {
		if( std::string(start->_title) == title )
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

void MenuEntry::showhelp( int y ){
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
			xSemaphoreTake(spiMutex,portMAX_DELAY );
			ucg->setPrintPos(x, y);
			ucg->print( words[p] );
			xSemaphoreGive(spiMutex );
			x+=len+5;
		}
		free( buf );
	}
}

void MenuEntry::clear()
{
	// ESP_LOGI(FNAME,"MenuEntry::clear");
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setColor(COLOR_BLACK);
	ucg->drawBox( 0,0,240,320 );
	// ucg->begin(UCG_FONT_MODE_SOLID);
	ucg->setFont(ucg_font_ncenR14_hr);
	ucg->setPrintPos( 1, 30 );
	ucg->setColor(COLOR_WHITE);
	xSemaphoreGive(spiMutex );
}

void MenuEntry::semaphoreTake()
{
  xSemaphoreTake( spiMutex, portMAX_DELAY );
}

void MenuEntry::semaphoreGive()
{
  xSemaphoreGive( spiMutex );
}
