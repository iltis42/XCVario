#include "DataMonitor.h"
#include "SetupMenu.h"
#include "SetupMenuSelect.h"
#include "sensor.h"
#include "SetupNG.h"
#include "Flarm.h"
#include "logdefnone.h"

#define SCROLL_TOP      20
#define SCROLL_BOTTOM  320

DataMonitor::DataMonitor(){
	mon_started = false;
	MYUCG = 0;
	scrollpos = SCROLL_BOTTOM;
	paused = true;
	setup = 0;
	channel = 0;
	first=true;
	rx_total = 0;
	tx_total = 0;
}

int DataMonitor::maxChar( const char *str, int pos, int len, bool binary ){
	int N=0;
	int i=0;
	char s[4] = { 0 };
	while( N <= 240 ){
		if( binary ){
			sprintf( s, "%02x ", str[i+pos] );
		}
		else{
			s[0] = str[i+pos];
		}
		N += MYUCG->getStrWidth( s );
		if( N<220 && (i+pos)<len ){
			i++;
		}else{
			break;
		}
	}
	return i;
}

void DataMonitor::header( ItfTarget ch, bool binary, int len, e_dir_t dir ){
	if( dir == DIR_RX ){
		rx_total += len;
	}
	else{
		tx_total += len;
	}
	// ESP_LOGI(FNAME,"header() %d %d %d ", len, rx_total, tx_total );
	const char * what;
	switch( ch.raw ) {
		case ItfTarget(BT_SPP).raw: what = "BT"; break;
		case ItfTarget(WIFI,8880).raw: what = "W8880"; break;
		case ItfTarget(WIFI,8881).raw: what = "W8881"; break;
		case ItfTarget(WIFI,8882).raw: what = "W8882"; break;
		case ItfTarget(S1_RS232).raw:  what = "S1"; break;
		case ItfTarget(S2_RS232).raw:  what = "S2"; break;
		case ItfTarget(CAN_BUS).raw: what = "CAN"; break;
		default:      what = "OFF"; break;
	}
	const char * b;
	if( binary )
		b = "B-";
	else
		b = "";
	MYUCG->setColor( COLOR_WHITE );
	MYUCG->setFont(ucg_font_fub11_tr, true );
	MYUCG->setPrintPos( 0, SCROLL_TOP );

	if( paused )
		MYUCG->printf( "%s%s: RX:%d TX:%d hold  ", b, what, rx_total, tx_total );
	else
		MYUCG->printf( "%s%s: RX:%d TX:%d bytes   ", b, what, rx_total, tx_total );
}

void DataMonitor::monitorString( ItfTarget ch, e_dir_t dir, const char *str, int len )
{
	if( !mon_started ) { return; }

	// ESP_LOGI(FNAME,"ch %x, channel %x", (unsigned)ch.raw, (unsigned)channel.raw );
	bool binary = data_monitor_mode.get() == MON_MOD_BINARY;
	if( paused || (ch != channel) ){
		// ESP_LOGI(FNAME,"not active, return started:%d paused:%d", mon_started, paused );
		if( ch == channel )
			header( ch, binary, len, dir );
		return;
	}
	printString( ch, dir, str, binary, len );
}

void DataMonitor::printString( ItfTarget ch, e_dir_t dir, const char *str, bool binary, int len ){
	// if (! binary)
	// ESP_LOGI(FNAME,"DM ch:%x dir:%d len:%d data:%s", (unsigned)ch.raw, dir, len, str );
	const int scroll_lines = 20;
	char dirsym = 0;
	if( dir == DIR_RX ){
		dirsym = '>';
	}
	else{
		dirsym = '<';
	}
	if( first ){
		first = false;
		MYUCG->setColor( COLOR_BLACK );
		MYUCG->drawBox( 0,SCROLL_TOP,240,320 );
	}
	MYUCG->setColor( COLOR_WHITE );
    header( ch, binary, len, dir );
	//if( !binary )
	// 	len = len-1;  // ignore the \n in ASCII mode
	int hunklen = 0;
	int pos=0;
	do {
		// ESP_LOGI(FNAME,"DM 1 len: %d pos: %d", len, pos );
		hunklen = maxChar( str, pos, len, binary );
		if( hunklen ){
			char hunk[64] = { 0 };
			memcpy( (void*)hunk, (void*)(str+pos), hunklen );
			// ESP_LOGI(FNAME,"DM 2 hunklen: %d pos: %d  h:%s", hunklen, pos, hunk );
			MYUCG->setColor( COLOR_BLACK );
			MYUCG->drawBox( 0, scrollpos, 240,scroll_lines );
			MYUCG->setColor( COLOR_WHITE );
			MYUCG->setPrintPos( 0, scrollpos+scroll_lines );
			MYUCG->setFont(ucg_font_fub11_tr, true );
			char txt[256];
			int hpos = 0;
			if( binary ){   // format data as readable text
				hpos += sprintf( txt, "%c ", dirsym );
				for( int i=0; i<hunklen && hpos<95 ; i++ ){
					hpos += sprintf( txt+hpos, "%02x ", hunk[i] );
				}
				txt[hpos] = 0; // zero terminate string
				MYUCG->print( txt );
				ESP_LOGI(FNAME,"DM binary ch:%d dir:%d string:%s", (int)ch.raw, dir, txt );
			}
			else{
				hpos += sprintf( txt, "%c ", dirsym );
				hpos += sprintf( txt+hpos, "%s", hunk );
				txt[hpos] = 0;
				MYUCG->print( txt );
				//ESP_LOGI(FNAME,"DM ascii ch:%d dir:%d data:%s", ch, dir, txt );
			}
			pos+=hunklen;
			// ESP_LOGI(FNAME,"DM 3 pos: %d", pos );
			scroll(scroll_lines);
		}
	}while( hunklen );
}

void DataMonitor::scroll(int scroll){
	scrollpos+=scroll;
	if( scrollpos >= SCROLL_BOTTOM )
		scrollpos = scroll;
	MYUCG->scrollLines( scrollpos );  // set frame origin
}

void DataMonitor::press(){
	ESP_LOGI(FNAME,"press paused: %d", paused );
	if( paused )
		paused = false;
	else
		paused = true;
	// }
}

void DataMonitor::longPress(){
	ESP_LOGI(FNAME,"longPress" );
	if( !mon_started ){
		ESP_LOGI(FNAME,"longPress, but not started, return" );
		return;
	}
	exit();
}

void DataMonitor::start(SetupMenuSelect *p, ItfTarget ch)
{
	ESP_LOGI(FNAME,"start");
	// forced replacement of the Select Menu on the observer stack
	p->detach();
	attach();
	_parent = p->getParent();
	mon_started = false; // important to avoid context race
	tx_total = 0;
	rx_total = 0;
	channel = ch;
	MYUCG->setColor( COLOR_BLACK );
	MYUCG->drawBox( 0,0,240,320 );
	MYUCG->setColor( COLOR_WHITE );
	MYUCG->setFont(ucg_font_fub11_tr, true );
	paused = false;
	header( channel, data_monitor_mode.get() == MON_MOD_BINARY );
	if( display_orientation.get() == DISPLAY_TOPDOWN )
		MYUCG->scrollSetMargins( 0, SCROLL_TOP );
	else
		MYUCG->scrollSetMargins( SCROLL_TOP, 0 );
	mon_started = true;
	paused = false; // will resume with press()
	ESP_LOGI(FNAME,"started");
}

void DataMonitor::exit(int ups)
{
	ESP_LOGI(FNAME,"stop");
	channel = 0;
	mon_started = false;
	paused = false;
	delay(1000);
	MYUCG->scrollLines( 0 );
	_parent->menuSetTop();
	MenuEntry::exit(1);
}
