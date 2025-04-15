#include "DataMonitor.h"

#include "comm/DeviceMgr.h"
#include "SetupMenu.h"
#include "setup/SetupAction.h"
#include "sensor.h"
#include "SetupNG.h"
#include "Flarm.h"
#include "logdefnone.h"

#define SCROLL_TOP      20
#define SCROLL_BOTTOM  320

DataMonitor *DM = nullptr;

DataMonitor::DataMonitor()
{
	scrollpos = SCROLL_BOTTOM;
	DM = this;
}

int DataMonitor::maxChar( const char *str, int pos, int len ){
	int N=0;
	int i=0;
	char s[4] = { 0 };
	while( N <= 240 ){
		if( bin_mode ){
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

void DataMonitor::header( int len, e_dir_t dir )
{
	if( dir == DIR_RX ){
		rx_total += len;
	}
	else{
		tx_total += len;
	}
	// ESP_LOGI(FNAME,"header() %d %d %d ", len, rx_total, tx_total );
	char what[10];
	strncpy(what, DeviceManager::getItfName(channel.iid).data(), 10);
	what[3] = '\0'; // cut after first three chars
	if ( channel.port > 0 ) {
		sprintf( what+1, "%d", channel.port);
	}
	const char * b;
	if( bin_mode )
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

void DataMonitor::monitorString(e_dir_t dir, const char *str, int len)
{
	ESP_LOGI(FNAME,"dir %d, len %d", (int)dir, len );
	// bool binary = data_monitor_mode.get() == MON_MOD_BINARY;
	if( paused )
	{
		// ESP_LOGI(FNAME,"not active, return started:%d paused:%d", mon_started, paused );
		header( len, dir );
		return;
	}
	printString(dir, str, len);
}

void DataMonitor::printString(e_dir_t dir, const char *str, int len ){
	// ESP_LOGI(FNAME,"DM ch:%x dir:%d len:%d data:%s", (unsigned)ch.raw, dir, len, str );
	const int scroll_lines = 20;
	char dirsym = 0;
	if( dir == DIR_RX ){
		dirsym = '>';
	}
	else{
		dirsym = '<';
	}
	MYUCG->setColor( COLOR_WHITE );
    header(len, dir );

	int hunklen = 0;
	int pos=0;
	do {
		// ESP_LOGI(FNAME,"DM 1 len: %d pos: %d", len, pos );
		hunklen = maxChar( str, pos, len );
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
			if( bin_mode ){   // format data as readable text
				hpos += sprintf( txt, "%c ", dirsym );
				for( int i=0; i<hunklen && hpos<95 ; i++ ){
					hpos += sprintf( txt+hpos, "%02x ", hunk[i] );
				}
				txt[hpos] = 0; // zero terminate string
				MYUCG->print( txt );
				ESP_LOGI(FNAME,"DM binary ch:%d dir:%d string:%s", (int)channel.raw, dir, txt );
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
	if( paused ) {
		paused = false;
	}
	else {
		paused = true;
	}
}

void DataMonitor::longPress()
{
	ESP_LOGI(FNAME,"stop");
	DEVMAN->stopDM();
	channel = 0;
	paused = false;
	delay(1000);
	MYUCG->scrollLines( 0 );
	DM = nullptr;
	delete this;
	exit();
}

void DataMonitor::start(SetupAction *p, ItfTarget ch)
{
	ESP_LOGI(FNAME,"start %x", unsigned(ch.raw));
	attach();
	_parent = p->getParent();
	tx_total = 0;
	rx_total = 0;
	channel = ch;
	bin_mode = false;
	MYUCG->setColor( COLOR_BLACK );
	MYUCG->drawBox( 0,0,240,320 );
	MYUCG->setColor( COLOR_WHITE );
	MYUCG->setFont(ucg_font_fub11_tr, true );
	paused = false;
	header();
	if( display_orientation.get() == DISPLAY_TOPDOWN ) {
		MYUCG->scrollSetMargins( 0, SCROLL_TOP );
	}
	else {
		MYUCG->scrollSetMargins( SCROLL_TOP, 0 );
	}
	paused = false; // will resume with press()
	bin_mode = DEVMAN->startDM(channel);
	ESP_LOGI(FNAME,"started");
}

