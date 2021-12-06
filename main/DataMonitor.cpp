#include "DataMonitor.h"
#include "sensor.h"
#include "logdef.h"


DataMonitor::DataMonitor(){
	mon_started = false;
	ucg = 0;
	scrollpos = 0;
	paused = true;
	_rotary = 0;
}

int DataMonitor::maxChar( const char *str, int pos ){
	int N=0;
	int i=0;
	char s[2] = { 0 };
	while( N <= 240 ){
		s[0] = str[i+pos];
		N += ucg->getStrWidth( s );
		if( N<240 )
			i++;
	}
	return i;
}

static bool first=true;

void DataMonitor::monitorString( int ch, e_dir_t dir, const char *str ){
	// ESP_LOGI(FNAME,"monitorString ch:%d dir:%d string:%s", ch, dir, str );
	if( !mon_started || paused ){
		// ESP_LOGI(FNAME,"not active, return started:%d paused:%d", mon_started, paused );
		return;
	}
	if( ch != data_monitor.get() ){
		return;
	}
	std::string s( str );
	int total = 0;
	const int ypos = 318;
	const int scroll = 20;
	std::string S;
	if( dir == DIR_RX )
		S = std::string(">");
	else
		S = std::string("<");
	S += s;
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	if( first ){
		first = false;
		ucg->setColor( COLOR_BLACK );
		ucg->drawBox( 0,0,240,320 );
	}
	ucg->setFont(ucg_font_fub11_tr);
	ucg->setColor( COLOR_WHITE );

	while( total <= S.length() ){
		int hunklen = maxChar( S.c_str(), total );
		std::string hunk = S.substr( total, hunklen );
		ucg->setColor( COLOR_WHITE );
		ucg->setPrintPos( 0, scrollpos );
		ucg->print( hunk.c_str() );
		total+=hunklen;
		// ESP_LOGI(FNAME,"hunklen: %d total: %d hunk %s", hunklen, total, hunk.c_str() );
		scrollpos+=scroll;
		if( scrollpos > 320 )
			scrollpos = 0;
		ucg->scrollLines( scrollpos );
		ucg->setColor( COLOR_BLACK );
		ucg->drawBox( 0,scrollpos-scroll,240,scroll );
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
}

void DataMonitor::longPress(){
	ESP_LOGI(FNAME,"longPress" );
	if( !mon_started ){
		ESP_LOGI(FNAME,"longPress, but not started, return" );
		return;
	}
	stop();
}

void DataMonitor::start(){
	ESP_LOGI(FNAME,"start");
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	_rotary->attach( this );
	SetupMenu::catchFocus( true );
	ucg->scrollSetMargins( 0, 0 );
	ucg->setColor( COLOR_BLACK );
	ucg->drawBox( 0,0,240,320 );
	ucg->setColor( COLOR_WHITE );
	ucg->setPrintPos( 10,20 );
	xSemaphoreGive(spiMutex);
	mon_started = true;
	paused = true; // will resume with press()
	ESP_LOGI(FNAME,"started");
}

void DataMonitor::stop(){
	ESP_LOGI(FNAME,"stop");
	// _rotary->detach( this );
	mon_started = false;
	delay(1000);
	ucg->scrollLines( 0 );
	SetupMenu::catchFocus( false );
}

