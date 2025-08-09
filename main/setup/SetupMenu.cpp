/*
 * SetupMenu.cpp
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#include "setup/SetupMenu.h"
#include "setup/SubMenuDevices.h"
#include "setup/SubMenuCompassWind.h"
#include "setup/SubMenuGlider.h"
#include "setup/ShowBootMsg.h"
#include "screen/SetupRoot.h"
#include "IpsDisplay.h"
#include "ESPAudio.h"
#include "BMPVario.h"
#include "S2F.h"
#include "Version.h"
#include "glider/Polars.h"
#include "Cipher.h"
#include "Units.h"
#include "S2fSwitch.h"
#include "Flap.h"
#include "setup/SetupMenuSelect.h"
#include "setup/SetupMenuValFloat.h"
#include "setup/SetupMenuChar.h"
#include "setup/SetupAction.h"
#include "esp_wifi.h"
#include "Flarm.h"
#include "protocol/FlarmSim.h"
#include "KalmanMPU6050.h"
#include "sensor.h"
#include "setup/SetupNG.h"

#include "comm/DeviceMgr.h"
#include "protocol/NMEA.h"
#include "comm/SerialLine.h"
#include "coredump_to_server.h"
#include "protocol/nmea/JumboCmdMsg.h"
#include "logdefnone.h"

#include <inttypes.h>
#include <iterator>
#include <algorithm>
#include <string>

static void setup_create_root(SetupMenu *top);

static void wiper_menu_create(SetupMenu *top);
static void bugs_item_create(SetupMenu *top);
static void vario_menu_create(SetupMenu *top);
static void vario_menu_create_damping(SetupMenu *top);
static void vario_menu_create_meanclimb(SetupMenu *top);
static void vario_menu_create_s2f(SetupMenu *top);
static void vario_menu_create_ec(SetupMenu *top);

static void audio_menu_create(SetupMenu *top);
static void audio_menu_create_volume(SetupMenu *top);
static void audio_menu_create_tonestyles(SetupMenu *top);
static void audio_menu_create_deadbands(SetupMenu *top);
static void audio_menu_create_equalizer(SetupMenu *top);
static void audio_menu_create_mute(SetupMenu *top);

static void options_menu_create(SetupMenu *top);
static void options_menu_create_units(SetupMenu *top);
static void options_menu_create_flarm(SetupMenu *top);
static void screens_menu_create_gload(SetupMenu *top);

static void system_menu_create(SetupMenu *top);
static void system_menu_create_software(SetupMenu *top);
static void system_menu_create_battery(SetupMenu *top);
static void system_menu_create_hardware(SetupMenu *top);
static void system_menu_create_altimeter_airspeed(SetupMenu *top);
static void system_menu_create_altimeter_airspeed_stallwa(SetupMenu *top);
static void system_menu_create_hardware_type(SetupMenu *top);
static void system_menu_create_hardware_rotary(SetupMenu *top);
static void system_menu_create_hardware_rotary_screens(SetupMenu *top);
static void system_menu_create_hardware_ahrs(SetupMenu *top);
static void system_menu_create_ahrs_calib(SetupMenu *top);
static void system_menu_create_hardware_ahrs_lc(SetupMenu *top);
static void system_menu_create_hardware_ahrs_parameter(SetupMenu *top);


// Menu for flap setup
float elev_step = 1;

bool SetupMenu::focus = false;

int gload_reset(SetupMenuSelect *p) {
	if ( p->getSelect() == 0 ) {
		gload_pos_max.set(1);
		gload_neg_max.set(0);
		airspeed_max.set(0);
	}
	return 0;
}

int compass_ena(SetupMenuSelect *p) {
	return 0;
}

int vario_setup(SetupMenuValFloat *p) {
	bmpVario.configChange();
	return 0;
}

static int set_rotary_direction(SetupMenuSelect *p) {
	Rotary->updateRotDir();
	return 0;
}

static int set_rotary_increment(SetupMenuSelect *p) {
	Rotary->updateIncrement(p->getSelect());
	return 0;
}

int audio_setup_s(SetupMenuSelect *p) {
	AUDIO->setup();
	return 0;
}

int audio_setup_f(SetupMenuValFloat *p) {
	AUDIO->setup();
	return 0;
}

int centeraid_action(SetupMenuSelect *p)
{
    if ( p->getSelect() ) {
        // create the center aid
        CenterAid::create();
    }
    else {
        CenterAid::remove();
    }
    return 0;
}

int speedcal_change(SetupMenuValFloat *p) {
	if (asSensor)
		asSensor->changeConfig();
	return 0;
}

int set_ahrs_defaults(SetupMenuSelect *p) {
	if (p->getSelect() == 1) {
		ahrs_gyro_factor.setDefault();
		ahrs_min_gyro_factor.setDefault();
		ahrs_dynamic_factor.setDefault();
		gyro_gating.setDefault();
	}
	p->setSelect(0);
	return 0;
}

gpio_num_t SetupMenu::getGearWarningIO() {
	gpio_num_t io = GPIO_NUM_0;
	if (gear_warning.get() == GW_FLAP_SENSOR
			|| gear_warning.get() == GW_FLAP_SENSOR_INV) {
		io = GPIO_NUM_34;
	} else if (gear_warning.get() == GW_S2_RS232_RX
			|| gear_warning.get() == GW_S2_RS232_RX_INV) {
		io = GPIO_NUM_18;
	}
	return io;
}

void SetupMenu::initGearWarning() {
	gpio_num_t io = SetupMenu::getGearWarningIO();
	if (io != GPIO_NUM_0) {
		gpio_reset_pin(io);
		gpio_set_direction(io, GPIO_MODE_INPUT);
		gpio_set_pull_mode(io, GPIO_PULLUP_ONLY);
		gpio_pullup_en(io);
	}
	ESP_LOGI(FNAME,"initGearWarning: IO: %d", io );
}

int config_gear_warning(SetupMenuSelect *p) {
	SetupMenu::initGearWarning();
	return 0;
}

int upd_screens(SetupMenuSelect *p)
{
	Menu->initScreens();
	return 0;
}


int do_display_test(SetupMenuSelect *p) {
	if (display_test.get()) {
		MYUCG->setColor(0, 0, 0);
		MYUCG->drawBox(0, 0, 240, 320);
		while (!Rotary->readSwitch()) {
			delay(100);
			ESP_LOGI(FNAME,"Wait for key press");
		}
		MYUCG->setColor(255, 255, 255);
		MYUCG->drawBox(0, 0, 240, 320);
		while (!Rotary->readSwitch()) {
			delay(100);
			ESP_LOGI(FNAME,"Wait for key press");
		}
		esp_restart();
	}
	return 0;
}

static char rentry0[32];
static char rentry1[32];
static char rentry2[32];

int update_rentry(SetupMenuValFloat *p) {
	// ESP_LOGI(FNAME,"update_rentry() vu:%s", Units::VarioUnit() );
	sprintf(rentry0, "Fixed (5  %s)", Units::VarioUnit());
	sprintf(rentry1, "Fixed (10 %s)", Units::VarioUnit());
	sprintf(rentry2, "Variable (%d %s)", (int) (scale_range.get()), Units::VarioUnit());
	return 0;
}

int update_rentrys(SetupMenuSelect *p) {
	update_rentry(0);
	return 0;
}

static const char *velocity_mode[3] = {"IAS", "TAS", "CAS"};
static int update_velocity_buzz(SetupMenuSelect *p) {
	SetupMenu *velocity = static_cast<SetupMenu*>(p->getParent()->getParent()->getEntry(3));
	velocity->setBuzzword(velocity_mode[airspeed_mode.get()]);
	return 0;
}

static const char *alti_mode[2] = {"QNH", "QFE"};
static int update_alti_buzz(SetupMenuSelect *p) {
	SetupMenu *alti = static_cast<SetupMenu*>(p->getParent()->getParent()->getEntry(4));
	alti->setBuzzword(alti_mode[alt_display_mode.get()]);
	return 0;
}

static void setAHRSBuzz(SetupMenu *p)
{
	static char ahrs_buzzword_buf[5];
	ahrs_buzzword_buf[0] = char(ahrs_licence_dig1.get()+'0');
	ahrs_buzzword_buf[1] = char(ahrs_licence_dig2.get()+'0');
	ahrs_buzzword_buf[2] = char(ahrs_licence_dig3.get()+'0');
	ahrs_buzzword_buf[3] = char(ahrs_licence_dig4.get()+'0');
	ahrs_buzzword_buf[4] = '\0';
	p->setBuzzword(ahrs_buzzword_buf);
}
static int add_key(SetupMenuSelect *p) {
	ESP_LOGI(FNAME,"add_key( %d ) ", p->getSelect() );
	if ( ahrs_licence_dig1.get() == ('@'-'0') ) {
		// hidden short-cut to delete the license key
		ahrs_licence_dig1.set(0);
		ahrs_licence_dig2.set(0);
		ahrs_licence_dig3.set(0);
		ahrs_licence_dig4.set(0);
		p->setSelect(0);
		p->setTerminateMenu();
	}
	else {
		Cipher crypt;
		gflags.ahrsKeyValid = crypt.checkKeyAHRS();
	}
	setAHRSBuzz(p->getParent());
	return 0;
}

static int imu_gaa(SetupMenuValFloat *f) {
	if (!(imu_reference.get() == Quaternion())) {
		IMU::applyImuReference(f->_value, imu_reference.get());
	}
	return 0;
}

static int imu_calib(SetupMenuSelect *p) {
	ESP_LOGI(FNAME,"Collect AHRS data (%d)", p->getSelect() );
	int sel = p->getSelect();
	switch (sel) {
	case 0:
		break; // cancel
	case 1:
		// collect samples
		IMU::doImuCalibration(p);
		break;
	case 2:
		// reset to default
		IMU::defaultImuReference();
		break;
	default:
		break;
	}
	p->setSelect(0);
	return 0;
}

int qnh_adj(SetupMenuValFloat *p) {
	ESP_LOGI(FNAME,"qnh_adj %f", QNH.get() );
	float alt = 0;
	if (Flarm::validExtAlt() && alt_select.get() == AS_EXTERNAL) {
		alt = alt_external + (QNH.get() - 1013.25) * 8.2296; // correct altitude according to ISA model = 27ft / hPa
	} else {
		int samples = 0;
		for (int i = 0; i < 6; i++) {
			bool ok;
			float a = baroSensor->readAltitude(QNH.get(), ok);
			if (ok) {  // only consider correct readouts
				alt += a;
				samples++;
			}
			delay(10);
		}
		alt = alt / (float) samples;
	}
	ESP_LOGI(FNAME,"Setup BA alt=%f QNH=%f hPa", alt, QNH.get() );
	MYUCG->setFont(ucg_font_fub25_hr, true);
	float altp;
	const char *u = "m";
	if (alt_unit.get() == 0) { // m
		altp = alt;
	} else {
		u = "ft";
		altp = Units::meters2feet(alt);
	}
	MYUCG->setPrintPos(1, 120);
	MYUCG->setColor( COLOR_WHITE );
	MYUCG->printf("%5d %s  ", (int) (altp + 0.5), u);

	MYUCG->setFont(ucg_font_ncenR14_hr);
	return 0;
}

// Battery Voltage Meter Calibration
int factv_adj(SetupMenuValFloat *p) {
	ESP_LOGI(FNAME,"factv_adj");
	getBattery()->redoAdjust();
	float bat = getBattery()->get(true);
	MYUCG->setPrintPos(1, 100);
	MYUCG->printf("%0.2f Volt", bat);
	return 0;
}

int water_adj(SetupMenuValFloat *p) {
	if ( ballast_kg.get() > polar_max_ballast.get() ) {
		ballast_kg.set(polar_max_ballast.get());
	}
	else if ( ballast_kg.get() < 0 ) {
		ballast_kg.set(0);
	}
	p->setMax(polar_max_ballast.get());
	start_weight_adj(p);
	return 0;
}

int wiper_button(SetupAction *p) {
	NmeaPrtcl *jumbo = static_cast<NmeaPrtcl*>(DEVMAN->getProtocol(DeviceId::JUMBO_DEV, ProtocolType::JUMBOCMD_P));

	ESP_LOGI(FNAME, "wipe %d", p->getCode());
	jumbo->sendJPShortPress(p->getCode());
	return 0;
}

int bug_adj(SetupMenuValFloat *p) {
	return 0;
}

int cur_vol_dflt(SetupMenuSelect *p) {
	if (p->getSelect() != 0)  // "set"
		default_volume.set(audio_volume.get());
	p->setSelect(0);   // return to "cancel"
	return 0;
}

static int startFlarmSimulation(SetupMenuSelect *p) {
	FlarmSim::StartSim();
	return 0;
}

static int eval_chop(SetupMenuSelect *p) {
	AUDIO->evaluateChopping();
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// SetupMenu
///////////////////////////////////////////////////////////////////////////////////////////////////////
SetupMenu::SetupMenu(const char *title, SetupMenuCreator_t menu_create, int cont_id) :
	MenuEntry(),
	populateMenu(menu_create),
	content_id(cont_id)
{
	// ESP_LOGI(FNAME,"SetupMenu::SetupMenu( %s ) ", title );
	_title.assign(title);
	setRotDynamic(1.f);
}

SetupMenu::~SetupMenu() {
	ESP_LOGI(FNAME,"del SetupMenu( %s ) ", _title.c_str() );
	for (auto *c : _childs) {
		delete c;
	}
	_childs.clear();
}

// fixme
void SetupMenu::catchFocus(bool activate) {
	focus = activate;
}

void SetupMenu::enter()
{
	ESP_LOGI(FNAME,"enter inSet %d, mptr: %p", gflags.inSetup, populateMenu );
	if ((_childs.empty() || dyn_content) && populateMenu) {
		dirty = true; // force to create child list
		ESP_LOGI(FNAME,"create_childs %d", _childs.size());
	}
	MenuEntry::enter();
}

void SetupMenu::display(int mode)
{
	if ( dirty && populateMenu) {
		// Cope with changes in menu item presence
		ESP_LOGI(FNAME,"SetupMenu display() dirty %d", dirty );
		dirty = false;
		(populateMenu)(this);
		ESP_LOGI(FNAME,"create_childs %d", _childs.size());
	}
	xSemaphoreTake(display_mutex, portMAX_DELAY);
	ESP_LOGI(FNAME,"SetupMenu display(%s-%d)", _title.c_str(), highlight );
	if ( highlight >= (int)(_childs.size()) ) {
		highlight = _childs.size()-1;
	}
	ESP_LOGI(FNAME,"SetupMenu display %d", highlight );
	clear();
	ESP_LOGI(FNAME,"Title: %s child size:%d", _title.c_str(), _childs.size());
	MYUCG->setFont(ucg_font_ncenR14_hr);
	MYUCG->setFontPosBottom();
	menuPrintLn("<<", 0);
	menuPrintLn(_title.c_str(), 0, 30);
	doHighlight(highlight);
	for (int i = 0; i < _childs.size(); i++) {
		MenuEntry *child = _childs[i];
		child->refresh(); // cope with potential external change to the e.g. nvs representation of values
		if (!child->isLeaf() || child->value()) {
			MYUCG->setColor( COLOR_HEADER_LIGHT);
		}
		menuPrintLn(child->getTitle(), i+1);
		// ESP_LOGI(FNAME,"Child Title: %s - %p", child->getTitle(), child->value() );
		if (child->value() && *child->value() != '\0') {
			int fl = MYUCG->getStrWidth(child->getTitle());
			menuPrintLn(": ", i+1, 1+fl);
			MYUCG->setColor( COLOR_WHITE);
			menuPrintLn(child->value(), i+1, 1 + fl + MYUCG->getStrWidth(": "));
		}
		MYUCG->setColor( COLOR_WHITE);
		// ESP_LOGI(FNAME,"Child: %s y=%d",child->getTitle() ,y );
	}
	showhelp(true);
	xSemaphoreGive(display_mutex);
}

int SetupMenu::freeBottomLines() const
{
    return dheight/25 - getNrChilds() - 1;
}

// void SetupMenu::setHighlight(MenuEntry *value)
// {
// 	int found = -1;
// 	for (int i = 0; i < _childs.size(); ++i) {
// 		if (_childs[i] == value) {
// 			found = i;
// 			break;
// 		}
// 	}
// 	if ( found >= 0 ) {
// 		highlight = found;
// 	}
// }
void SetupMenu::setHighlight(int chnr)
{
	if ( chnr >= 0 && chnr < (int)_childs.size() ) {
		highlight = chnr;
	}
}

MenuEntry* SetupMenu::addEntry( MenuEntry * item )
{
	ESP_LOGI(FNAME,"add to childs");
	item->regParent(this);
	_childs.push_back( item );
	return item;
}

void SetupMenu::delEntry( MenuEntry * item ) {
	ESP_LOGI(FNAME,"SetupMenu delMenu() title %s", item->getTitle() );
	std::vector<MenuEntry *>::iterator position = std::find(_childs.begin(), _childs.end(), item );
	if (position != _childs.end()) {
		ESP_LOGI(FNAME,"found entry, now erase" );
		delete *position;
		_childs.erase(position);
	}
}

// not yet used
// const MenuEntry* SetupMenu::findMenu(const char *title) const
// {
// 	ESP_LOGI(FNAME,"MenuEntry findMenu() %s %p", title, this );
// 	if( _title == title ) {
// 		ESP_LOGI(FNAME,"Menu entry found for start %s", title );
// 		return this;
// 	}
// 	for (const MenuEntry* child : static_cast<const SetupMenu*>(this)->_childs) {
// 		const MenuEntry* m = child->findMenu(title);
// 		if( m != nullptr ) {
// 			ESP_LOGI(FNAME,"Menu entry found for %s", title);
// 			return m;
// 		}
// 	}
// 	ESP_LOGW(FNAME,"Menu entry not found for %s", title);
// 	return nullptr;
// }
//
// int SetupMenu::findMenuIdx(int contId) const
// {
// 	int idx = 0;
// 	for (const MenuEntry* child : _childs) {
// 		if( !child->isLeaf() && static_cast<const SetupMenu*>(child)->getContId() == contId ) {
// 			ESP_LOGI(FNAME,"Menu entry found for %s", title);
// 			return idx;
// 		}
// 		idx++;
// 	}

// 	return -1; // not found
// }


MenuEntry *SetupMenu::getEntry(int n) const
{
	if ( n < _childs.size() ) {
		return _childs[n];
	}
	return nullptr;
}

static int modulo(int a, int b) {
	return (a % b + b) % b;
}

void SetupMenu::rot(int count)
{
	// ESP_LOGI(FNAME,"select %d: %d/%d", count, highlight, _childs.size() );
	unHighlight(highlight);
	highlight = modulo(highlight+1+count, _childs.size()+1) - 1;
	doHighlight(highlight);
}

void SetupMenu::press()
{
	ESP_LOGI(FNAME,"press() inSet %d highl: %d", gflags.inSetup, highlight );
	if (highlight == -1) {
		_parent->highlightTop();
		exit();
	} else {
		ESP_LOGI(FNAME,"SetupMenu to child");
		if ((highlight >= 0) && (highlight < _childs.size())) {
			_childs[highlight]->enter();
		}
	}
}

void SetupMenu::longPress()
{
	if (highlight == -1) {
		exit(-1); // fast exit
	} else {
		press();
	}
}

int deviceDumpAction(SetupAction *p)
{
	// dump devices
	DEVMAN->dumpMap();
	return 0;
}

int varioAvChange(SetupMenuValFloat *p) {
	// ESP_LOGI(FNAME,"varioAvChange() %f", vario_av_delay.get() );
	if (vario_av_delay.get() > 0.1) {
		bmpVario.setAveragerTime(vario_av_delay.get());
	}
	return 0;
}

static int s2fModeChange(SetupMenuSelect *p) {
	if ( S2FSWITCH ) {
		S2FSWITCH->updateSwitchSetup();
	}
	return 0;
}
static int s2fModeChangeF(SetupMenuValFloat *p) {
	if ( S2FSWITCH ) {
		S2FSWITCH->updateSwitchSetup();
	}
	return 0;
}

void vario_menu_create_damping(SetupMenu *top) {
	SetupMenuValFloat *vda = new SetupMenuValFloat("Damping", "sec", vario_setup, false, &vario_delay);
	vda->setHelp("Response time, time constant of Vario low pass filter");
	top->addEntry(vda);

	SetupMenuValFloat *vdav = new SetupMenuValFloat("Averager", "sec", varioAvChange, false, &vario_av_delay);
	vdav->setHelp("Response time, time constant of digital Average Vario Display");
	top->addEntry(vdav);
}

void vario_menu_create_meanclimb(SetupMenu *top) {
	SetupMenuValFloat *vccm = new SetupMenuValFloat("Minimum climb", "", nullptr, false, &core_climb_min);
	vccm->setHelp("Minimum climb rate that counts for arithmetic mean climb value");
	top->addEntry(vccm);

	SetupMenuValFloat *vcch = new SetupMenuValFloat("Duration", "min", nullptr, false, &core_climb_history);
	vcch->setHelp(
			"Duration in minutes over which mean climb rate is computed, default is last 3 thermals or 45 min");
	top->addEntry(vcch);

	SetupMenuValFloat *vcp = new SetupMenuValFloat("Cycle", "sec", nullptr, false, &core_climb_period);
	vcp->setHelp(
			"Cycle: number of seconds when mean climb value is recalculated, default is every 60 seconds");
	top->addEntry(vcp);

	SetupMenuValFloat *vcmc = new SetupMenuValFloat("Major Change", "m/s", nullptr, false, &mean_climb_major_change);
	vcmc->setHelp(
			"Change in mean climb during last cycle (minute), that results in a major change indication (with arrow symbol)");
	top->addEntry(vcmc);
}

void vario_menu_create_s2f(SetupMenu *top) {
	SetupMenuValFloat *vds2 = new SetupMenuValFloat("Damping", "sec", nullptr, false, &s2f_delay);
	vds2->setHelp("Time constant of S2F low pass filter");
	top->addEntry(vds2);

	SetupMenuSelect *blck = new SetupMenuSelect("Blockspeed", RST_NONE, nullptr, &s2f_blockspeed);
	blck->setHelp(
			"With Blockspeed enabled, vertical movement of airmass or G-load is not considered for speed to fly calculation");
	blck->mkEnable();
	top->addEntry(blck);

	SetupMenuSelect *s2fmod = new SetupMenuSelect("S2F Mode", RST_NONE, s2fModeChange, &s2f_switch_mode);
	s2fmod->setHelp("Select data source for switching between S2F and Vario modes", 230);
	s2fmod->addEntry("Switch", AM_SWITCH);
	s2fmod->addEntry("AutoSpeed", AM_AUTOSPEED);
	if ( flap_enable.get() ) {
		s2fmod->addEntry("AutoFlap", AM_FLAP); // not dynamic, exit setup to change
	}
	s2fmod->addEntry("AutoTurn", AM_AHRS);
	s2fmod->addEntry("Vario fix", AM_VARIO);
	s2fmod->addEntry("Cruise fix", AM_S2F);
	s2fmod->addEntry("Master/Second", AM_EXTERNAL);
	top->addEntry(s2fmod);

	SetupMenuSelect *s2fsw = new SetupMenuSelect("S2F Switch", RST_NONE, s2fModeChange, &s2f_switch_type);
	top->addEntry(s2fsw);
	s2fsw->setHelp("Select S2F switch type: normal switch, push button (toggling S2F mode on each press), or disabled");
	s2fsw->addEntry("Disable", S2F_SWITCH_DISABLE );
	s2fsw->addEntry("Switch", S2F_HW_SWITCH);
	s2fsw->addEntry("Switch Invert", S2F_HW_SWITCH_INVERTED);
	s2fsw->addEntry("Push Button", S2F_HW_PUSH_BUTTON);

	SetupMenuValFloat *autospeed = new SetupMenuValFloat("AutoSpeed Thresh.", "", nullptr, false, &s2f_threshold);
	top->addEntry(autospeed);
	autospeed->setHelp("Transition speed for the AutoSpeed S2F switch");

	if ( flap_enable.get() ) {
		SetupMenuValFloat *s2f_flap = new SetupMenuValFloat("AutoFlap Position", "<", nullptr, false, &s2f_flap_pos);
		top->addEntry(s2f_flap);
		s2f_flap->setHelp("Precise flap position for the AutoFlap S2F switch");
	}

	SetupMenuValFloat *s2f_gyro = new SetupMenuValFloat("AutoTurn Rate", "°/s", nullptr, false, &s2f_gyro_deg);
	top->addEntry(s2f_gyro);
	s2f_gyro->setHelp("Turnrate for the AutoTurnrate switch");

	SetupMenuValFloat *s2flag = new SetupMenuValFloat("Switch Lag", "sec", s2fModeChangeF, false, &s2f_auto_lag);
	s2flag->setHelp("Lag to delay the auto switch event (2-20sec)");
	top->addEntry(s2flag);

	SetupMenuSelect *s2fnc = new SetupMenuSelect("Arrow Color", RST_NONE, nullptr, &s2f_arrow_color);
	s2fnc->setHelp("Select color of the S2F arrow when painted in Up/Down position");
	s2fnc->addEntry("White/White");
	s2fnc->addEntry("Blue/Blue");
	s2fnc->addEntry("Green/Red");
	top->addEntry(s2fnc);
}

void vario_menu_create_ec(SetupMenu *top) {
	SetupMenuSelect *enac = new SetupMenuSelect("eCompensation", RST_NONE, nullptr, &te_comp_enable);
	enac->setHelp(
			"Enable/Disable electronic TE compensation option; Enable only when TE port is connected to ST (static) pressure");
	enac->addEntry("TEK Probe");
	enac->addEntry("EPOT");
	enac->addEntry("PRESSURE");
	top->addEntry(enac);

	SetupMenuValFloat *elca = new SetupMenuValFloat("Adjustment", "%", nullptr, false, &te_comp_adjust);
	elca->setHelp(
			"Adjustment option for electronic TE compensation in %. This affects the energy altitude calculated from airspeed");
	top->addEntry(elca);
}

void wiper_menu_create(SetupMenu *top) {
	SetupAction *wiperL = new SetupAction("Wipe left         ", wiper_button, 1);
	JumboCmdMsg::LeftAction = wiperL;
	SetupAction *wiperR = new SetupAction("Wipe       right", wiper_button, 0);
	JumboCmdMsg::RightAction = wiperR;
	top->addEntry(wiperL);
	top->addEntry(wiperR);

	bugs_item_create(top);
}

void bugs_item_create(SetupMenu *top) {
	SetupMenuValFloat *bgs = new SetupMenuValFloat("Bugs", "%", bug_adj, true, &bugs);
	bgs->setHelp("Percent degradation of gliding performance due to bugs contamination");
	top->addEntry(bgs);
}

void vario_menu_create(SetupMenu *vae) {
	ESP_LOGI(FNAME,"SetupMenu::vario_menu_create( %p )", vae );

	SetupMenuValFloat *vga = new SetupMenuValFloat("Range", "", audio_setup_f, true, &scale_range);
	vga->setHelp("Upper and lower value for Vario graphic display region");
	vga->setPrecision(0);
	vae->addEntry(vga);

	SetupMenuSelect *vlogscale = new SetupMenuSelect("Log. Scale", RST_NONE, nullptr, &log_scale);
	vlogscale->mkEnable();
	vae->addEntry(vlogscale);

	SetupMenuSelect *vamod = new SetupMenuSelect("Mode", RST_NONE, nullptr, &vario_mode);
	vamod->setHelp(
			"Controls if vario considers polar sink (=Netto), or not (=Brutto), or if Netto vario applies only in Cruise Mode");
	vamod->addEntry("Brutto");
	vamod->addEntry("Netto");
	vamod->addEntry("Cruise-Netto");
	vae->addEntry(vamod);

	SetupMenuSelect *nemod = new SetupMenuSelect("Netto Mode", RST_NONE, nullptr, &netto_mode);
	nemod->setHelp(
			"In 'Relative' mode (also called 'Super-Netto') circling sink is considered,  to show climb rate as if you were circling there");
	nemod->addEntry("Normal");
	nemod->addEntry("Relative");
	vae->addEntry(nemod);

	SetupMenu *vdamp = new SetupMenu("Vario Damping", vario_menu_create_damping);
	vae->addEntry(vdamp);

	SetupMenu *meanclimb = new SetupMenu("Mean Climb", vario_menu_create_meanclimb);
	meanclimb->setHelp(
			"Options for calculation of Mean Climb (MC recommendation) displayed by green/red rhombus");
	vae->addEntry(meanclimb);

	SetupMenu *s2fs = new SetupMenu("S2F Settings", vario_menu_create_s2f);
	vae->addEntry(s2fs);

	SetupMenu *elco = new SetupMenu("Electronic Compensation", vario_menu_create_ec);
	vae->addEntry(elco);
}

void audio_menu_create_tonestyles(SetupMenu *top) {
	SetupMenuValFloat *cf = new SetupMenuValFloat("CenterFreq", "Hz", audio_setup_f, false, &center_freq);
	cf->setHelp("Center frequency for Audio at zero Vario or zero S2F delta");
	top->addEntry(cf);

	SetupMenuValFloat *oc = new SetupMenuValFloat("Octaves", "fold", audio_setup_f, false, &tone_var);
	oc->setHelp("Maximum tone frequency variation");
	top->addEntry(oc);

	SetupMenuSelect *dt = new SetupMenuSelect("Dual Tone", RST_NONE, audio_setup_s, &dual_tone);
	dt->setHelp(
			"Select dual tone modue aka ilec sound (di/da/di), or single tone (di/di/di) mode");
	dt->mkEnable();
	top->addEntry(dt);

	SetupMenuValFloat *htv = new SetupMenuValFloat("Dual Tone Pitch", "%", audio_setup_f, false, &high_tone_var);
	htv->setHelp(
			"Tone variation in Dual Tone mode, percent of frequency pitch up for second tone");
	top->addEntry(htv);

	SetupMenuSelect *tch = new SetupMenuSelect("Chopping", RST_NONE, eval_chop, &chopping_mode);
	tch->setHelp(
			"Select tone chopping option on positive values for Vario and or S2F");
	tch->addEntry("Disabled");             // 0
	tch->addEntry("Vario only");           // 1
	tch->addEntry("S2F only");             // 2
	tch->addEntry("Vario and S2F");        // 3  default
	top->addEntry(tch);

	SetupMenuSelect *tchs = new SetupMenuSelect("Chopping Style", RST_NONE, nullptr, &chopping_style);
	tchs->setHelp(
			"Select style of tone chopping either hard, or soft with fadein/fadeout");
	tchs->addEntry("Soft");              // 0  default
	tchs->addEntry("Hard");              // 1
	top->addEntry(tchs);

	SetupMenuSelect *advarto = new SetupMenuSelect("Variable Tone", RST_NONE, nullptr, &audio_variable_frequency);
	advarto->setHelp(
			"Enable audio frequency updates within climbing tone intervals, disable keeps frequency constant");
	advarto->mkEnable();
	top->addEntry(advarto);
}

void audio_menu_create_deadbands(SetupMenu *top) {
	SetupMenuValFloat *dbminlv = new SetupMenuValFloat("Lower Vario", "", nullptr, false, &deadband_neg);
	top->addEntry(dbminlv);

	SetupMenuValFloat *dbmaxlv = new SetupMenuValFloat("Upper Vario", "", nullptr, false, &deadband);
	top->addEntry(dbmaxlv);

	SetupMenuValFloat *dbmaxls2fn = new SetupMenuValFloat("Lower S2F", "", nullptr, false, &s2f_deadband_neg);
	top->addEntry(dbmaxls2fn);

	SetupMenuValFloat *dbmaxls2f = new SetupMenuValFloat("Upper S2F", "", nullptr, false, &s2f_deadband);
	top->addEntry(dbmaxls2f);
}

void audio_menu_create_equalizer(SetupMenu *top) {
	SetupMenuSelect *audeqt = new SetupMenuSelect("Equalizer", RST_ON_EXIT, nullptr, &audio_equalizer);
	audeqt->setHelp(
			"Select the equalizer according to the type of loudspeaker used");
	audeqt->addEntry("Disable");
	audeqt->addEntry("Speaker 8 Ohms");
	audeqt->addEntry("Speaker 4 Ohms");
	audeqt->addEntry("Speaker External");
	top->addEntry(audeqt);

	SetupMenuValFloat *frqr = new SetupMenuValFloat("Frequency Response", "%", 0, false, &frequency_response);
	frqr->setHelp(
			"Setup frequency response, double frequency will be attenuated by the factor given, half frequency will be amplified");
	top->addEntry(frqr);
}

void audio_menu_create_volume(SetupMenu *top) {

	SetupMenuValFloat *vol = new SetupMenuValFloat("Current Volume", "%", nullptr, false, &audio_volume);
	vol->lock();
	top->addEntry(vol);

	SetupMenuValFloat *dv = new SetupMenuValFloat("Default Volume", "%", nullptr, false, &default_volume);
	top->addEntry(dv);
	dv->setHelp("Default volume for Audio when device is switched on");

	SetupMenuValFloat *mv = new SetupMenuValFloat("Max Volume", "%", nullptr, false, &max_volume);
	top->addEntry(mv);
	mv->setHelp("Maximum audio volume setting allowed");

	SetupMenu *audeq = new SetupMenu("Equalizer", audio_menu_create_equalizer);
	top->addEntry(audeq);
	audeq->setHelp("Equalization parameters for a constant perceived volume over a wide frequency range", 220);

	SetupMenuSelect *amspvol = new SetupMenuSelect("S2F Volume", RST_NONE, nullptr, &audio_split_vol);
	amspvol->setHelp(
			"Enable independent audio volume in SpeedToFly and Vario modes, disable for one volume for both");
	amspvol->mkEnable();
	top->addEntry(amspvol);
}

void audio_menu_create_mute(SetupMenu *top) {
	SetupMenuSelect *asida = new SetupMenuSelect("In Sink", RST_NONE, nullptr, &audio_mute_sink);
	asida->setHelp("Select whether vario audio will be muted while in sink");
	asida->addEntry("Stay On");  // 0
	asida->addEntry("Mute");     // 1
	top->addEntry(asida);

	SetupMenuSelect *ameda = new SetupMenuSelect("In Setup", RST_NONE, nullptr, &audio_mute_menu);
	ameda->setHelp(
			"Select whether vario audio will be muted while Setup Menu is open");
	ameda->addEntry("Stay On");  // 0
	ameda->addEntry("Mute");     // 1
	top->addEntry(ameda);

	SetupMenuSelect *ageda = new SetupMenuSelect("Generally", RST_NONE, nullptr, &audio_mute_gen);
	ageda->setHelp(
			"Select audio on, or vario audio muted, or all audio muted including alarms");
	ageda->addEntry("Audio On");      // 0 = AUDIO_ON
	ageda->addEntry("Alarms On");     // 1 = AUDIO_ALARMS
	ageda->addEntry("Audio Off");     // 2 = AUDIO_OFF
	top->addEntry(ageda);

	SetupMenuSelect *amps = new SetupMenuSelect("Amplifier", RST_NONE, nullptr, &amplifier_shutdown);
	amps->setHelp(
			"Select whether amplifier is shutdown during silences, or always stays on");
	amps->addEntry("Stay On");   // 0 = AMP_STAY_ON
	amps->addEntry("Shutdown");  // 1 = AMP_SHUTDOWN
	top->addEntry(amps);
}

void audio_menu_create(SetupMenu *audio) {

	SetupMenu *volumes = new SetupMenu("Volume options", audio_menu_create_volume);
	volumes->setHelp("Audio volume for variometer tone on internal and external speaker");
	audio->addEntry(volumes);

	SetupMenu *mutes = new SetupMenu("Mute Audio", audio_menu_create_mute);
	audio->addEntry(mutes);
	mutes->setHelp("Configure audio muting options", 240);

	SetupMenuSelect *abnm = new SetupMenuSelect("Cruise Audio", RST_NONE, nullptr, &cruise_audio_mode);
	abnm->setHelp(
			"Select either S2F command or Variometer (Netto/Brutto as selected) as audio source while cruising");
	abnm->addEntry("Speed2Fly");       // 0
	abnm->addEntry("Vario");           // 1
	audio->addEntry(abnm);

	SetupMenu *audios = new SetupMenu("Tone Styles", audio_menu_create_tonestyles);
	audio->addEntry(audios);
	audios->setHelp("Configure audio style in terms of center frequency, octaves, single/dual tone, pitch and chopping", 220);

	update_rentry(0);
	SetupMenuSelect *audio_range_sm = new SetupMenuSelect("Range", RST_NONE, audio_setup_s, &audio_range);
	audio_range_sm->addEntry(rentry0);
	audio_range_sm->addEntry(rentry1);
	audio_range_sm->addEntry(rentry2);
	audio_range_sm->setHelp(
			"Audio range: fixed, or variable according to current Vario display range setting");
	audio->addEntry(audio_range_sm);

	SetupMenu *db = new SetupMenu("Deadbands", audio_menu_create_deadbands);
	audio->addEntry(db);
	db->setHelp("Dead band limits within which audio remains silent.  1 m/s equals roughly 200 fpm or 2 knots");

	SetupMenuValFloat *afac = new SetupMenuValFloat("Audio Exponent", "", nullptr, false, &audio_factor);
	afac->setHelp(
			"How the audio frequency responds to the climb rate: < 1 for logarithmic, and > 1 for exponential, response");
	audio->addEntry(afac);
}

void options_menu_create_units(SetupMenu *top) {
	SetupMenuSelect *alu = new SetupMenuSelect("Altimeter", RST_NONE, nullptr, &alt_unit);
	alu->addEntry("Meter (m)");
	alu->addEntry("Feet (ft)");
	alu->addEntry("FL (FL)");
	top->addEntry(alu);
	SetupMenuSelect *iau = new SetupMenuSelect("Airspeed", RST_NONE, nullptr, &ias_unit);
	iau->addEntry("Kilom./hour (Km/h)");
	iau->addEntry("Miles/hour (mph)");
	iau->addEntry("Knots (kt)");
	top->addEntry(iau);
	SetupMenuSelect *vau = new SetupMenuSelect("Vario", RST_NONE, update_rentrys, &vario_unit);
	vau->addEntry("Meters/sec (m/s)");
	vau->addEntry("Feet/min x 100 (fpm)");
	vau->addEntry("Knots (kt)");
	top->addEntry(vau);
	SetupMenuSelect *teu = new SetupMenuSelect("Temperature", RST_NONE, nullptr, &temperature_unit);
	teu->addEntry("Celcius");
	teu->addEntry("Fahrenheit");
	teu->addEntry("Kelvin");
	top->addEntry(teu);
	SetupMenuSelect *qnhi = new SetupMenuSelect("QNH", RST_NONE, nullptr, &qnh_unit);
	qnhi->addEntry("Hectopascal");
	qnhi->addEntry("InchMercury");
	top->addEntry(qnhi);
	SetupMenuSelect *dst = new SetupMenuSelect("Distance", RST_NONE, nullptr, &dst_unit);
	dst->addEntry("Meter (m)");
	dst->addEntry("Feet (ft)");
	top->addEntry(dst);
}

static void system_menu_create_airspeed(SetupMenu *top) {
	SetupMenuSelect *amode = new SetupMenuSelect("Airspeed Mode", RST_NONE, update_velocity_buzz, &airspeed_mode);
	amode->setHelp("Select mode of Airspeed indicator to display IAS (Indicated AirSpeed), TAS (True AirSpeed) or CAS (calibrated airspeed)", 180);
	amode->addEntry(velocity_mode[0]);
	amode->addEntry(velocity_mode[1]);
	amode->addEntry(velocity_mode[2]);
	// amode->addEntry("Slip Angle");
	top->addEntry(amode);

	SetupMenuValFloat *spc = new SetupMenuValFloat("AS Calibration", "%", speedcal_change, false, &speedcal);
	spc->setHelp("Calibration of airspeed sensor (AS). Normally not needed, unless pressure the probe has a systematic error");
	top->addEntry(spc);

	SetupMenuSelect *auze = new SetupMenuSelect("Set AS Zero", RST_IMMEDIATE, nullptr, &autozero);
	top->addEntry(auze);
	auze->setHelp("Recalculate zero point for airspeed sensor on next power on");
	auze->addEntry("Cancel");
	auze->addEntry("Start");

	SetupMenuSelect *stawaen = new SetupMenuSelect("Stall Warning", RST_NONE, nullptr, &stall_warning);
	stawaen->setHelp("Enable alarm sound when speed goes below configured stall speed (until 30% less)");
	stawaen->mkEnable();
	top->addEntry(stawaen);

	SetupMenuValFloat *staspe = new SetupMenuValFloat("Stall Speed", "", nullptr, true, &stall_speed);
	top->addEntry(staspe);
}

static void options_menu_create_altimeter(SetupMenu *top) {
	SetupMenuSelect *altDisplayMode = new SetupMenuSelect("Altitude Mode", RST_NONE, update_alti_buzz, &alt_display_mode);
	top->addEntry(altDisplayMode);
	altDisplayMode->setHelp("Select altitude display mode");
	altDisplayMode->addEntry(alti_mode[0]);
	altDisplayMode->addEntry(alti_mode[1]);

	SetupMenuSelect *atrans = new SetupMenuSelect("Auto Transition", RST_NONE, nullptr, &fl_auto_transition);
	top->addEntry(atrans);
	atrans->setHelp("Option to enable automatic altitude transition to QNH Standard (1013.25) above 'Transition Altitude'");
	atrans->mkEnable();

	SetupMenuValFloat *tral = new SetupMenuValFloat("Transition Altitude", "FL", nullptr, false, &transition_alt);
	tral->setHelp("Transition altitude (or transition height, when using QFE) is the altitude/height above which standard pressure (QNE) is set (1013.2 mb/hPa)", 100);
	top->addEntry(tral);

	SetupMenuSelect *als = new SetupMenuSelect("Atl. Source", RST_NONE, nullptr, &alt_select);
	top->addEntry(als);
	als->setHelp("Select source for barometric altitude, either TE sensor or Baro sensor (recommended) or an external source e.g. FLARM (if avail)");
	als->addEntry("TE Sensor");
	als->addEntry("Baro Sensor");
	als->addEntry("External");

	SetupMenuSelect *alq = new SetupMenuSelect("Alt. Quantization", RST_NONE, nullptr, &alt_quantization);
	alq->setHelp("Set altimeter mode with discrete steps and rolling last digits");
	alq->addEntry("Disable");
	alq->addEntry("2");
	alq->addEntry("5");
	alq->addEntry("10");
	alq->addEntry("20");
	top->addEntry(alq);
}

void options_menu_create_flarm(SetupMenu *top) {
	SetupMenuSelect *flarml = new SetupMenuSelect("Alarm Level", RST_NONE, nullptr, &flarm_warning);
	flarml->setHelp(
			"Level of FLARM alarm to enable: 1 is lowest (13-18 sec), 2 medium (9-12 sec), 3 highest (0-8 sec) until impact");
	flarml->addEntry("Disable");
	flarml->addEntry("Level 1");
	flarml->addEntry("Level 2");
	flarml->addEntry("Level 3");
	top->addEntry(flarml);

	SetupMenuValFloat *flarmv = new SetupMenuValFloat("Alarm Volume", "%", nullptr, false, &flarm_volume);
	flarmv->setHelp("Maximum audio volume of FLARM alarm warning");
	top->addEntry(flarmv);

	SetupMenuValFloat *flarmt = new SetupMenuValFloat("Alarm Timeout", "sec", nullptr, false, &flarm_alarm_time);
	flarmt->setHelp(
			"The time FLARM alarm warning keeps displayed after alarm went off");
	top->addEntry(flarmt);

	SetupMenuSelect *flarms = new SetupMenuSelect("Alarm Simulation", RST_NONE, startFlarmSimulation, nullptr, false, true);
	flarms->setHelp(
			"Simulate an airplane crossing from left to right with different alarm levels and vertical distance 5 seconds after pressed (exits setup!)");
	flarms->addEntry("Start Simulation");
	top->addEntry(flarms);
}

void screens_menu_create_gload(SetupMenu *top) {
	SetupMenuSelect *glmod = new SetupMenuSelect("Activation Mode", RST_NONE, nullptr, &gload_mode);
	glmod->setHelp(
			"Switch off G-Force screen, or activate by threshold G-Force 'Dynamic', or static by 'Always-On'");
	glmod->addEntry("Off");
	glmod->addEntry("Dynamic");
	glmod->addEntry("Always-On");
	top->addEntry(glmod);

	SetupMenuValFloat *gtpos = new SetupMenuValFloat("Positive Threshold", "", nullptr, false, &gload_pos_thresh);
	top->addEntry(gtpos);
	gtpos->setPrecision(1);
	gtpos->setHelp("Positive threshold to launch G-Load display");

	SetupMenuValFloat *gtneg = new SetupMenuValFloat("Negative Threshold", "", nullptr, false, &gload_neg_thresh);
	top->addEntry(gtneg);
	gtneg->setPrecision(1);
	gtneg->setHelp("Negative threshold to launch G-Load display");

	SetupMenuValFloat *glpos = new SetupMenuValFloat("Red positive limit", "", nullptr, false, &gload_pos_limit);
	top->addEntry(glpos);
	glpos->setPrecision(1);
	glpos->setHelp(
			"Positive g load factor limit the aircraft is able to handle below maneuvering speed, see manual");

	SetupMenuValFloat *glposl = new SetupMenuValFloat("Yellow pos. Limit", "", nullptr, false, &gload_pos_limit_low);
	top->addEntry(glposl);
	glposl->setPrecision(1);
	glposl->setHelp(
			"Positive g load factor limit the aircraft is able to handle above maneuvering speed, see manual");

	SetupMenuValFloat *glneg = new SetupMenuValFloat("Red negative limit", "", nullptr, false, &gload_neg_limit);
	top->addEntry(glneg);
	glneg->setPrecision(1);
	glneg->setHelp(
			"Negative g load factor limit the aircraft is able to handle below maneuvering speed, see manual");

	SetupMenuValFloat *glnegl = new SetupMenuValFloat("Yellow neg. Limit", "", nullptr, false, &gload_neg_limit_low);
	top->addEntry(glnegl);
	glnegl->setPrecision(1);
	glnegl->setHelp(
			"Negative g load factor limit the aircraft is able to handle above maneuvering speed, see manual");

	SetupMenuValFloat *gmpos = new SetupMenuValFloat("Peak Positive", "", nullptr, false, &gload_pos_max);
	top->addEntry(gmpos);
	gmpos->lock();

	SetupMenuValFloat *gmneg = new SetupMenuValFloat("Peak Negative", "", nullptr, false, &gload_neg_max);
	top->addEntry(gmneg);
	gmneg->lock();

	SetupMenuSelect *gloadres = new SetupMenuSelect("Reset peak-hold", RST_NONE, gload_reset);
	gloadres->addEntry("Reset");
	gloadres->addEntry("Cancel");
	top->addEntry(gloadres);

	SetupMenuValFloat *gloadalvo = new SetupMenuValFloat("Alarm Volume", "%", nullptr, false, &gload_alarm_volume);
	gloadalvo->setHelp("Maximum volume of G-Load alarm audio warning");
	top->addEntry(gloadalvo);
}

static void screens_menu_create_vario(SetupMenu *top) {

	SetupMenuSelect *sink = new SetupMenuSelect("Polar Sink", RST_NONE, nullptr, &ps_display);
	sink->setHelp("Display polar sink rate together with climb rate when Vario is in Brutto Mode (else disabled)");
	sink->mkEnable();
	top->addEntry(sink);

	SetupMenuSelect *ncolor = new SetupMenuSelect("Needle Color", RST_NONE, nullptr, &needle_color);
	ncolor->addEntry("White");
	ncolor->addEntry("Orange");
	ncolor->addEntry("Red");
	top->addEntry(ncolor);

	SetupMenuSelect *scrcaid = new SetupMenuSelect("Center-Assistent", RST_NONE, centeraid_action, &screen_centeraid);
	scrcaid->setHelp("Enable/disable display of the thermal assistent");
	scrcaid->mkEnable();
	top->addEntry(scrcaid);

	SetupMenuSelect *tgauge = new SetupMenuSelect("Upper Gauge", RST_NONE, nullptr, &screen_gauge_top);
	tgauge->addEntry("Disable");
	tgauge->addEntry("Airspeed", GAUGE_SPEED);
	tgauge->addEntry("Speed2Fly", GAUGE_S2F);
	tgauge->addEntry("Heading", GAUGE_HEADING);
	tgauge->addEntry("Slip Angle", GAUGE_SLIP);
	top->addEntry(tgauge);

	SetupMenuSelect *bgauge = new SetupMenuSelect("Lower Gauge", RST_NONE, nullptr, &screen_gauge_bottom);
	bgauge->addEntry("Disable");
	bgauge->addEntry("Altimeter", GAUGE_ALT);
	bgauge->addEntry("Life Wind", GAUGE_NONE);
	top->addEntry(bgauge);
}

static void options_menu_create_screens(SetupMenu *top) { // dynamic!
	if ( top->getNrChilds() == 0 ) {
		SetupMenu *gload = new SetupMenu("G-Load Screen", screens_menu_create_gload);
		top->addEntry(gload);

		SetupMenu *vario = new SetupMenu("Vario Screen", screens_menu_create_vario);
		top->addEntry(vario);

		SetupMenuSelect *scrgmet = new SetupMenuSelect("G-Meter", RST_NONE, upd_screens, &screen_gmeter);
		scrgmet->mkEnable();
		top->addEntry(scrgmet);
	}
	if ( top->getNrChilds() == 4 ) {
		top->delEntry(top->getEntry(3));
	}
	if (gflags.ahrsKeyValid) {
		SetupMenuSelect *horizon = new SetupMenuSelect("Horizon", RST_NONE, upd_screens, &screen_horizon);
		horizon->mkEnable();
		top->addEntry(horizon);
	}
}

void options_menu_create(SetupMenu *opt) { // dynamic!
	if ( opt->getNrChilds() == 0 ) {
		if (student_mode.get() == 0) {
			SetupMenuSelect *stumo = new SetupMenuSelect("Student Mode", RST_NONE, nullptr, &student_mode);
			opt->addEntry(stumo);
			stumo->setHelp(
					"Student mode, disables all sophisticated setup to just basic pre-flight related items like MC, ballast or bugs");
			stumo->mkEnable();
		}
		Flap::setupMenue(opt);
		// Units
		SetupMenu *un = new SetupMenu("Units", options_menu_create_units);
		opt->addEntry(un);
		un->setHelp("Setup altimeter, airspeed indicator and variometer with European Metric, American, British or Australian units", 205);

		// Airspeed
		SetupMenu *velocity = new SetupMenu("Airspeed", system_menu_create_airspeed);
		velocity->setBuzzword(velocity_mode[airspeed_mode.get()]);
		opt->addEntry(velocity);

		// Altimeter
		SetupMenu *alti = new SetupMenu("Altimeter", options_menu_create_altimeter);
		alti->setBuzzword(alti_mode[alt_display_mode.get()]);
		opt->addEntry(alti);

		SetupMenu *flarm = new SetupMenu("FLARM", options_menu_create_flarm);
		opt->addEntry(flarm);
		flarm->setHelp("Option to display FLARM Warnings depending on FLARM alarm level");

		SetupMenu *compassWindMenu = new SetupMenu("Compass/Wind", options_menu_create_compasswind);
		compassWindMenu->setDynContent();
		opt->addEntry(compassWindMenu);
		compassWindMenu->setHelp("Setup Compass and Wind", 280);

		SetupMenu *screens = new SetupMenu("Screens & Gauges", options_menu_create_screens);
		screens->setDynContent();
		opt->addEntry(screens);
	}
	SetupMenu *flarm = static_cast<SetupMenu*>(opt->getEntry(5));
	if ( DEVMAN->getDevice(FLARM_DEV) != nullptr ) {
		flarm->unlock();
		flarm->setBuzzword();
	}
	else {
		flarm->lock();
		flarm->setBuzzword("n/a");
	}
}

void system_menu_create_software(SetupMenu *top) {
	Version V;
	SetupMenuSelect *ver = new SetupMenuSelect("Software Vers.", RST_NONE, nullptr);
	ver->addEntry(V.version());
	ver->lock();
	top->addEntry(ver);

	SetupMenuSelect *upd = new SetupMenuSelect("Software Update", RST_IMMEDIATE, nullptr, &software_update);
	upd->setHelp(
			"Software Update over the air (OTA). Starts Wifi AP 'ESP32 OTA' - connect and open http://192.168.4.1 in browser");
	upd->addEntry("Cancel");
	upd->addEntry("Start");
	top->addEntry(upd);

	if ( logged_tests.size() > 0 ) {
		SetupMenuDisplay *dis = new ShowBootMsg("Show Boot Messages");
		top->addEntry(dis);
	}
}

void system_menu_create_battery(SetupMenu *top) {
	SetupMenuValFloat *blow = new SetupMenuValFloat("Battery Low", "Volt ", nullptr, false, &bat_low_volt);
	SetupMenuValFloat *bred = new SetupMenuValFloat("Battery Red", "Volt ", nullptr, false, &bat_red_volt);
	SetupMenuValFloat *byellow = new SetupMenuValFloat("Battery Yellow", "Volt ", nullptr, false, &bat_yellow_volt);
	SetupMenuValFloat *bfull = new SetupMenuValFloat("Battery Full", "Volt ", nullptr, false, &bat_full_volt);

	SetupMenuSelect *batv = new SetupMenuSelect("Battery Display", RST_NONE, nullptr, &battery_display);
	batv->setHelp(
			"Option to display battery charge state either in Percentage e.g. 75% or Voltage e.g. 12.5V");
	batv->addEntry("Percentage");
	batv->addEntry("Voltage");
	batv->addEntry("Voltage Big");

	top->addEntry(blow);
	top->addEntry(bred);
	top->addEntry(byellow);
	top->addEntry(bfull);
	top->addEntry(batv);
}

void system_menu_create_hardware_type(SetupMenu *top) {
	// UNIVERSAL, RAYSTAR_RFJ240L_40P, ST7789_2INCH_12P, ILI9341_TFT_18P
	SetupMenuSelect *dtype = new SetupMenuSelect("HW Type", RST_NONE, nullptr, &display_type);
	dtype->setHelp("Factory setup for corresponding display type used");
	dtype->addEntry("UNIVERSAL");
	dtype->addEntry("RAYSTAR");
	dtype->addEntry("ST7789");
	dtype->addEntry("ILI9341");
	top->addEntry(dtype);

	SetupMenuSelect *disty = new SetupMenuSelect("Style", RST_NONE, nullptr, &display_style);
	top->addEntry(disty);
	disty->setHelp(
			"Display style in more digital airliner stype or retro mode with classic vario meter needle");
	disty->addEntry("Airliner");
	disty->addEntry("Retro");
	disty->addEntry("UL");

	SetupMenuSelect *disva = new SetupMenuSelect("Color Variant", RST_NONE, nullptr, &display_variant);
	top->addEntry(disva);
	disva->setHelp(
			"Display variant white on black (W/B) or black on white (B/W)");
	disva->addEntry("W/B");
	disva->addEntry("B/W");

	// Orientation   _display_orientation
	SetupMenuSelect * diso = new SetupMenuSelect( "Orientation", RST_ON_EXIT, nullptr, &display_orientation );
	top->addEntry( diso );
	diso->setHelp( "Display Orientation.  NORMAL means Rotary on left, TOPDOWN means Rotary on right  (reboots). A change will reset the AHRS reference calibration.");
	diso->addEntry( "NORMAL");
	diso->addEntry( "TOPDOWN");
	diso->addEntry( "NINETY");

	SetupMenuSelect *dtest = new SetupMenuSelect("Display Test", RST_NONE, do_display_test, &display_test);
	top->addEntry(dtest);
	dtest->setHelp("Start display test screens, press rotary to cancel");
	dtest->addEntry("Cancel");
	dtest->addEntry("Start Test");

	SetupMenuValFloat *dcadj = new SetupMenuValFloat("Display Clk Adj", "%", nullptr, true, &display_clock_adj, RST_IMMEDIATE);
	dcadj->setHelp(
			"Modify display clock by given percentage (restarts on exit)", 100);
	top->addEntry(dcadj);

}

void system_menu_create_hardware_rotary(SetupMenu *top) {
	SetupMenuSelect *rotype;
	rotype = new SetupMenuSelect("Direction", RST_NONE, set_rotary_direction, &rotary_dir);
	top->addEntry(rotype);
	rotype->setHelp("Choose the direction of the rotary knob");
	rotype->addEntry("Clockwise");
	rotype->addEntry("Counterclockwise");

	SetupMenuSelect *roinc = new SetupMenuSelect("Sensitivity", RST_NONE, set_rotary_increment, &rotary_inc);
	top->addEntry(roinc);
	roinc->setHelp(
			"Select rotary sensitivity in number of tick's for one increment, to your personal preference, 1 tick is most sensitive");
	roinc->addEntry("1 tick");
	roinc->addEntry("2 tick");
	roinc->addEntry("3 tick");

	// Rotary Default
	SetupMenuSelect *rd = new SetupMenuSelect("Rotation", RST_ON_EXIT, nullptr, &rot_default);
	top->addEntry(rd);
	rd->setHelp(
			"Select value to be altered at rotary movement outside of setup menu (reboots)");
	rd->addEntry("Volume");
	rd->addEntry("MC Value");

	SetupMenuSelect *sact = new SetupMenuSelect("Enter Setup by", RST_NONE, nullptr, &menu_long_press);
	top->addEntry(sact);
	sact->setHelp("Activate setup menu either by short or long button press");
	sact->addEntry("Short Press");
	sact->addEntry("Long Press");
}

void system_menu_create_ahrs_calib(SetupMenu *top) {
	SetupMenuSelect *ahrs_calib_collect = new SetupMenuSelect("Axis calibration", RST_NONE, imu_calib);
	ahrs_calib_collect->setHelp(
			"Calibrate IMU axis on flat leveled ground ground with no inclination. Run the procedure by selecting Start.");
	ahrs_calib_collect->addEntry("Cancel");
	ahrs_calib_collect->addEntry("Start");
	ahrs_calib_collect->addEntry("Reset");

	SetupMenuValFloat *ahrs_ground_aa = new SetupMenuValFloat("Ground angle of attack", "°", imu_gaa, false, &glider_ground_aa);
	ahrs_ground_aa->setHelp(
			"Angle of attack with tail skid on the ground to adjust the AHRS reference. Change this any time to correct the AHRS horizon level.");
	ahrs_ground_aa->setPrecision(0);
	top->addEntry(ahrs_calib_collect);
	top->addEntry(ahrs_ground_aa);
}

static const char  lkeys[][4] { "0", "1", "2", "3", "4", "5", "6", "7",
		"8", "9", ":", ";", "<", "=", ">", "?", "@", "A", "B", "C", "D", "E",
		"F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S",
		"T", "U", "V", "W", "X", "Y", "Z" };

void system_menu_create_hardware_ahrs_lc(SetupMenu *top) {
	SetupMenuSelect *ahrslc1 = new SetupMenuSelect("First    Letter", RST_NONE, add_key, &ahrs_licence_dig1);
	SetupMenuSelect *ahrslc2 = new SetupMenuSelect("Second Letter", RST_NONE, add_key, &ahrs_licence_dig2);
	SetupMenuSelect *ahrslc3 = new SetupMenuSelect("Third   Letter", RST_NONE, add_key, &ahrs_licence_dig3);
	SetupMenuSelect *ahrslc4 = new SetupMenuSelect("Last     Letter", RST_NONE, add_key, &ahrs_licence_dig4);
	top->addEntry(ahrslc1);
	top->addEntry(ahrslc2);
	top->addEntry(ahrslc3);
	top->addEntry(ahrslc4);
	ahrslc1->addEntryList(lkeys, sizeof(lkeys) / 4);
	ahrslc2->addEntryList(lkeys, sizeof(lkeys) / 4);
	ahrslc3->addEntryList(lkeys, sizeof(lkeys) / 4);
	ahrslc4->addEntryList(lkeys, sizeof(lkeys) / 4);
}

void system_menu_create_hardware_ahrs_parameter(SetupMenu *top) {
	SetupMenuValFloat *ahrsgf = new SetupMenuValFloat("Gyro Max Trust", "x", nullptr, false, &ahrs_gyro_factor);
	ahrsgf->setPrecision(0);
	ahrsgf->setHelp("Maximum Gyro trust factor in artifical horizon");
	top->addEntry(ahrsgf);

	SetupMenuValFloat *ahrsgfm = new SetupMenuValFloat("Gyro Min Trust", "x", nullptr, false, &ahrs_min_gyro_factor);
	ahrsgfm->setPrecision(0);
	ahrsgfm->setHelp("Minimum Gyro trust factor in artifical horizon");
	top->addEntry(ahrsgfm);

	SetupMenuValFloat *ahrsdgf = new SetupMenuValFloat("Gyro Dynamics", "", nullptr, false, &ahrs_dynamic_factor);
	ahrsdgf->setHelp(
			"Gyro dynamics factor, higher value trusts gyro more when load factor is different from one");
	top->addEntry(ahrsdgf);

	SetupMenuSelect *ahrsrollcheck = new SetupMenuSelect("Gyro Roll Check", RST_NONE, nullptr, &ahrs_roll_check);
	ahrsrollcheck->setHelp("Switch to test the gyro roll check code.");
	ahrsrollcheck->addEntry("Disable");
	ahrsrollcheck->addEntry("Enable");
	top->addEntry(ahrsrollcheck);

	SetupMenuValFloat *gyrog = new SetupMenuValFloat("Gyro Gating", "°", nullptr, false, &gyro_gating);
	gyrog->setHelp("Minimum accepted gyro rate in degree per second");
	top->addEntry(gyrog);

	SetupMenuValFloat *tcontrol = new SetupMenuValFloat("AHRS Temp Control", "", nullptr, false, &mpu_temperature);
	tcontrol->setPrecision(0);
	tcontrol->setHelp(
			"Regulated target temperature of AHRS silicon chip, if supported in hardware (model > 2023), -1 means OFF");
	top->addEntry(tcontrol);

	SetupMenuSelect *ahrsdef = new SetupMenuSelect("Reset to Defaults", RST_NONE, set_ahrs_defaults);
	top->addEntry(ahrsdef);
	ahrsdef->setHelp(
			"Set optimum default values for all AHRS Parameters as determined to the best practice");
	ahrsdef->addEntry("Cancel");
	ahrsdef->addEntry("Set Defaults");

}

void system_menu_create_hardware_ahrs(SetupMenu *top) {
	SetupMenuSelect *ahrsid = new SetupMenuSelect("XCV unique Id", RST_NONE);
	ahrsid->addEntry(SetupCommon::getDefaultID());
	ahrsid->lock();
	top->addEntry(ahrsid);

	SetupMenu *ahrscalib = new SetupMenu("Calibration", system_menu_create_ahrs_calib);
	ahrscalib->setHelp(
			 "Bias & Reference of the AHRS Sensor: Place glider on horizontal underground, first the right wing down, then the left wing.");
	top->addEntry(ahrscalib);

	SetupMenu *ahrslc = new SetupMenu("License Key", system_menu_create_hardware_ahrs_lc);
	ahrslc->setHelp("Enter valid AHRS License Key, then AHRS feature can be enabled under 'AHRS Option'");
	setAHRSBuzz(ahrslc);
	top->addEntry(ahrslc);

	SetupMenu *ahrspa = new SetupMenu("Parameters", system_menu_create_hardware_ahrs_parameter);
	ahrspa->setHelp("AHRS constants such as gyro trust and filtering", 275);
	top->addEntry(ahrspa);

	SetupMenuSelect *rpyl = new SetupMenuSelect("AHRS RPYL", RST_NONE, nullptr, &ahrs_rpyl_dataset);
	top->addEntry(rpyl);
	rpyl->setHelp("Send LEVIL AHRS like $RPYL sentence for artifical horizon");
	rpyl->mkEnable();
}

void system_menu_create_hardware(SetupMenu *top) {
	SetupMenu *display = new SetupMenu("DISPLAY Setup", system_menu_create_hardware_type);
	top->addEntry(display);

	SetupMenu *rotary = new SetupMenu("Rotary Setup", system_menu_create_hardware_rotary);
	top->addEntry(rotary);

	SetupMenuSelect *gear = new SetupMenuSelect("Gear Warn", RST_NONE, config_gear_warning, &gear_warning);
	top->addEntry(gear);
	gear->setHelp(
			"Enable gear warning on S2 flap sensor or serial RS232 pin (pos. or neg. signal) or by external command",
			220);
	gear->addEntry("Disable");
	gear->addEntry("S2 Flap positive"); // A positive signal, high signal or > 2V will start alarm
	gear->addEntry("S2 RS232 positive");
	gear->addEntry("S2 Flap negative"); // A negative signal, low signal or < 1V will start alarm
	gear->addEntry("S2 RS232 negative");
	gear->addEntry("External");  // A $g,w<n>*CS command from an external device

	if (hardwareRevision.get() >= XCVARIO_21) {
		SetupMenu *ahrs = new SetupMenu("AHRS Setup", system_menu_create_hardware_ahrs);
		top->addEntry(ahrs);
	}

	SetupMenuSelect * pstype = new SetupMenuSelect( "AS Sensor type", RST_ON_EXIT, nullptr, &airspeed_sensor_type );
	top->addEntry( pstype );
	pstype->setHelp( "Factory default for type of pressure sensor, will not erase on factory reset (reboots)");
	pstype->addEntry( "ABPMRR");
	pstype->addEntry( "TE4525");
	pstype->addEntry( "MP5004");
	pstype->addEntry( "MCPH21");
	pstype->addEntry( "Autodetect");

	SetupMenuValFloat *met_adj = SetupMenu::createVoltmeterAdjustMenu();
	top->addEntry(met_adj);
}

void system_menu_create(SetupMenu *sye) {
	SetupMenu *soft = new SetupMenu("Software", system_menu_create_software);
	sye->addEntry(soft);

	SetupMenuSelect *fa = new SetupMenuSelect("Factory Reset", RST_IMMEDIATE, nullptr, &factory_reset);
	fa->setHelp("Option to reset all settings to factory defaults, means metric system, 5 m/s vario range and more");
	fa->addEntry("Cancel");
	fa->addEntry("ResetAll");
	sye->addEntry(fa);

	SetupMenu *bat = new SetupMenu("Battery Setup", system_menu_create_battery);
	bat->setHelp(
			"Adjust corresponding voltage for battery symbol display low,red,yellow and full");
	sye->addEntry(bat);

	SetupMenu *hardware = new SetupMenu("Hardware Setup", system_menu_create_hardware);
	hardware->setHelp("Setup variometer hardware e.g. display, rotary, AS and AHRS sensor, voltmeter, etc", 240);
	sye->addEntry(hardware);

	// Audio
	SetupMenu *ad = new SetupMenu("Audio", audio_menu_create);
	sye->addEntry(ad);

	// XCV role
	SetupMenuSelect *role = new SetupMenuSelect("XCV device role", RST_IMMEDIATE, nullptr, &xcv_role);
	role->setHelp("Set the intended role of this device first (needs a reboot)");
	// role->addEntry("None", NO_ROLE); hidden, because there is no use case currently
	role->addEntry("Master", MASTER_ROLE);
	role->addEntry("Second", SECOND_ROLE);
	sye->addEntry(role);

	// Devices menu
	SetupMenu *devices = new SetupMenu("Connected Devices", system_menu_connected_devices);
	devices->setHelp("Devices, Interf.s, Protocols");
	devices->setDynContent();
	sye->addEntry(devices);

	SetupMenuSelect *logg = new SetupMenuSelect("Logging", RST_NONE, nullptr, &logging);
	logg->setHelp("R&D option, do not use!\n Collect raw sensor data with NMEA logger in XCSoar");
	logg->mkEnable("Sensor RAW Data");
	sye->addEntry(logg);

	// SetupAction *devdump = new SetupAction("Device Setup Dump", deviceDumpAction, 0);
	// sye->addEntry(devdump);
}

void setup_create_root(SetupMenu *top) {
	ESP_LOGI(FNAME,"setup_create_root()");
	if (rot_default.get() == 0) {
		SetupMenuValFloat *mc = new SetupMenuValFloat("MC", "", nullptr, true, &MC);
		mc->setHelp(
				"Mac Cready value for optimum cruise speed or average climb rate, in same unit as the variometer");
		mc->setPrecision(1);
		top->addEntry(mc);
	} else {
		SetupMenuValFloat *vol = new SetupMenuValFloat("Audio Volume", "%", nullptr, true, &audio_volume);
		vol->setHelp("Audio volume level for variometer tone on internal and external speaker");
		vol->setMax(max_volume.get());
		top->addEntry(vol);
	}

	ESP_LOGI(FNAME, "Create bugs menu");
	Device *jumbo = DEVMAN->getDevice(DeviceId::JUMBO_DEV);
	if (jumbo) {
		sprintf(rentry0, "Bugs at %d%%", int(bugs.get()));
		SetupMenu *wiper = new SetupMenu(rentry0, wiper_menu_create);
		top->addEntry(wiper);
	} else {
		bugs_item_create(top);
	}

	SetupMenuValFloat *bal = new SetupMenuValFloat("Ballast", "litre", water_adj, true, &ballast_kg);
	bal->setHelp("Amount of water ballast added to the over all weight");
	bal->setPrecision(0);
	bal->setNeverInline();
	top->addEntry(bal);

	SetupMenuValFloat *crewball = new SetupMenuValFloat("Crew Weight", "kg", start_weight_adj, false, &crew_weight);
	crewball->setPrecision(0);
	crewball->setHelp(
			"Weight of the pilot(s) including parachute (everything on top of the empty weight apart from ballast)");
	crewball->setNeverInline();
	top->addEntry(crewball);

	SetupMenuValFloat *qnh_menu = SetupMenu::createQNHMenu();
	top->addEntry(qnh_menu);

	SetupMenuValFloat *afe = new SetupMenuValFloat("Airfield Elevation", "", nullptr, true, &elevation);
	afe->setHelp(
			"Airfield elevation in meters for QNH auto adjust on ground according to this elevation");
	afe->setRotDynamic(3.0);
	top->addEntry(afe);

	// Clear student mode, password correct
	if ((int) (password.get()) == 271) {
		student_mode.set(0);
		password.set(0);
	}
	// Student mode: Query password
	if (student_mode.get()) {
		SetupMenuValFloat *passw = new SetupMenuValFloat("Expert Password", "", nullptr, false, &password);
		passw->setPrecision(0);
		passw->setHelp(
				"To exit from student mode enter expert password and restart device after expert password has been set correctly");
		top->addEntry(passw);
	} else {
		// Vario
		SetupMenu *va = new SetupMenu("Vario and Speed 2 Fly", vario_menu_create);
		top->addEntry(va);

		// Glider Setup
		SetupMenu *po = new SetupMenu("Glider Details", glider_menu_create);
		po->setBuzzword(Polars::getGliderType());
		top->addEntry(po);

		// Options Setup
		SetupMenu *opt = new SetupMenu("Options", options_menu_create);
		opt->setDynContent();
		top->addEntry(opt);

		// System Setup
		SetupMenu *sy = new SetupMenu("System", system_menu_create);
		top->addEntry(sy);
	}
}

SetupMenu* SetupMenu::createTopSetup() {
	SetupMenu *setup = new  SetupMenu("Setup", setup_create_root);
	return setup;
}

SetupMenuValFloat* SetupMenu::createQNHMenu() {
	SetupMenuValFloat *qnh = new SetupMenuValFloat("QNH", "", qnh_adj, true, &QNH);
	qnh->setHelp("QNH pressure value from ATC. On ground you may adjust to airfield altitude above MSL", 180);
	return qnh;
}

SetupMenuValFloat* SetupMenu::createVoltmeterAdjustMenu() {
	SetupMenuValFloat *met_adj = new SetupMenuValFloat("Voltmeter Adjust", "%", factv_adj, false, &factory_volt_adjust, RST_NONE, false, true);
	met_adj->setHelp("Option to factory fine-adjust voltmeter");
	met_adj->setNeverInline();
	return met_adj;
}
