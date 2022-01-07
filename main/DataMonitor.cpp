#include "DataMonitor.h"
#include "sensor.h"
#include "logdef.h"
#include "Flarm.h"

#define SCROLL_TOP      20
#define SCROLL_BOTTOM  320

xSemaphoreHandle DataMonitor::mutex = 0;

DataMonitor::DataMonitor(){
	mon_started = false;
	ucg = 0;
	scrollpos = SCROLL_BOTTOM;
	paused = true;
	setup = 0;
	channel = MON_OFF;
	mutex = xSemaphoreCreateMutex();
	first=true;
	rx_total = 0;
	tx_total = 0;
}

int DataMonitor::maxChar( const char *str, int pos, bool binary ){
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
		N += ucg->getStrWidth( s );
		if( N<240 ){
			i++;
		}
	}
	return i;
}

void DataMonitor::header( int ch, bool binary ){
	const char * what;
	switch( ch ) {
		case MON_BLUETOOTH: what = "BT"; break;
		case MON_WIFI_8880: what = "W 8880"; break;
		case MON_WIFI_8881: what = "W 8882"; break;
		case MON_WIFI_8882: what = "W 8883"; break;
		case MON_S1:  what = "S1"; break;
		case MON_S2:  what = "S2"; break;
		case MON_CAN: what = "CAN"; break;
		default:      what = "OFF"; break;
	}
	const char * b;
	if( binary )
		b = "B-";
	else
		b = "";
	ucg->setPrintPos( 20, SCROLL_TOP );
	ucg->printf("%s%s: RX:%d TX:%d bytes   ", b, what, rx_total, tx_total );
}

void DataMonitor::monitorString( int ch, e_dir_t dir, const char *str, bool binary ){
	if( !mon_started || paused || (ch != channel) ){
		// ESP_LOGI(FNAME,"not active, return started:%d paused:%d", mon_started, paused );
		return;
	}
	xSemaphoreTake(mutex,portMAX_DELAY );
	if( !binary ){
		if( Flarm::bincom )
			binary = true;
	}
	printString( ch, dir, str, binary );
	xSemaphoreGive(mutex);
}

void DataMonitor::printString( int ch, e_dir_t dir, const char *str, bool binary ){
	ESP_LOGI(FNAME,"DM ch:%d dir:%d string:%s", ch, dir, str );
	std::string s( str );
	const int scroll = 20;
	int len = strlen( str );
	std::string S;
	if( dir == DIR_RX ){
		S = std::string(">");
		rx_total += len;
	}
	else{
		S = std::string("<");
		tx_total += len;
	}
	S += s;
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	if( first ){
		first = false;
		ucg->setColor( COLOR_BLACK );
		ucg->drawBox( 0,SCROLL_TOP,240,320 );
	}
	ucg->setColor( COLOR_WHITE );
    header( ch, binary );
    int rest = len;
	int pos = 0;
	while( rest > 1 ){  // ignore the \n
		// ESP_LOGI(FNAME,"DM 1 rest: %d pos: %d", rest, pos );
		int hunklen = maxChar( S.c_str(), pos, binary );
		std::string hunk = S.substr( pos, hunklen );
		// ESP_LOGI(FNAME,"DM 2 rest: %d hunklen: %d pos: %d  h:%s", rest, hunklen, pos, hunk.c_str()  );
		ucg->setColor( COLOR_BLACK );
		ucg->drawBox( 0, scrollpos, 240,scroll );
		ucg->setColor( COLOR_WHITE );
		ucg->setPrintPos( 0, scrollpos+scroll );
		ucg->setFont(ucg_font_fub11_tr, true );
		if( binary ){
			std::string binstr;
			for( int i=0; i<hunklen; i++ ){
				char bin[4];
				sprintf( bin, "%02x ", hunk[i] );
				binstr += std::string(bin);
			}
			ucg->print( binstr.c_str() );
		}
		else{
			ucg->print( hunk.c_str() );
		}
		pos+=hunklen;
		rest -= hunklen;
		// ESP_LOGI(FNAME,"DM 3 rest: %d pos: %d", rest, pos );
		scrollpos+=scroll;
		if( scrollpos >= SCROLL_BOTTOM )
			scrollpos = 20;
		ucg->scrollLines( scrollpos );  // set frame origin
	}
	xSemaphoreGive(spiMutex);
}

void DataMonitor::press(){
	ESP_LOGI(FNAME,"press paused: %d", paused );
	if( !Rotary.readSwitch() ){ // only process press here
	if( paused )
		paused = false;
	else
		paused = true;
	}
	delay( 100 );
}

void DataMonitor::longPress(){
	ESP_LOGI(FNAME,"longPress" );
	if( !mon_started ){
		ESP_LOGI(FNAME,"longPress, but not started, return" );
		return;
	}
	stop();
	delay( 100 );
}

void DataMonitor::start(SetupMenuSelect * p){
	ESP_LOGI(FNAME,"start");
	if( !setup )
		attach( this );
	setup = p;
	tx_total = 0;
	rx_total = 0;
	channel = p->getSelect();
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	SetupMenu::catchFocus( true );
	ucg->setColor( COLOR_BLACK );
	ucg->drawBox( 0,0,240,320 );
	ucg->setColor( COLOR_WHITE );
	ucg->setFont(ucg_font_fub11_tr, true );
	header( channel );
	if( display_orientation.get() == DISPLAY_TOPDOWN )
		ucg->scrollSetMargins( 0, SCROLL_TOP );
	else
		ucg->scrollSetMargins( SCROLL_TOP, 0 );
	mon_started = true;
	paused = true; // will resume with press()
	xSemaphoreGive(spiMutex);
	ESP_LOGI(FNAME,"started");
}

void DataMonitor::stop(){
	ESP_LOGI(FNAME,"stop");
	channel = MON_OFF;
	mon_started = false;
	paused = false;
	delay(1000);
	ucg->scrollLines( 0 );
	setup->setSelect( MON_OFF );
	SetupMenu::catchFocus( false );
}

