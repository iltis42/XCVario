
#include "SubMenuCompassWind.h"

#include "comm/Devices.h"
#include "setup/SetupMenu.h"
#include "setup/SetupMenuSelect.h"

#include "DisplayDeviations.h"
#include "ShowCompassSettings.h"
#include "ShowCirclingWind.h"
#include "ShowStraightWind.h"
#include "Compass.h"
#include "CompassMenu.h"
#include "comm/DeviceMgr.h"
#include "logdef.h"
#include "wind/WindCalcTask.h"

#include <string_view>

// compass menu handlers.
static int compassDeviationAction(SetupMenuSelect *p) {
	if (p->getSelect() == 0) {
		CompassMenu::deviationAction(p);
	}
	return 0;
}

static int compassResetDeviationAction(SetupMenuSelect *p) {
	return CompassMenu::resetDeviationAction(p);
}

static int compassDeclinationAction(SetupMenuValFloat *p) {
	return CompassMenu::declinationAction(p);
}

static int compassSensorCalibrateAction(SetupMenuSelect *p) {
	ESP_LOGI(FNAME,"compassSensorCalibrateAction()");
	if (p->getSelect() != 0) { // Start, Show
		CompassMenu::sensorCalibrationAction(p);
	}
	p->setSelect(0);
	return 0;
}

static int windSettingsAction(SetupMenuSelect *p) {
	WindCalcTask::createWindResources();
	return 0;
}

static void options_menu_create_compasswind_compass_dev(SetupMenu *top) {
	const char *skydirs[8] = { "0°", "45°", "90°", "135°", "180°", "225°",
			"270°", "315°" };
	for (int i = 0; i < 8; i++) {
		SetupMenuSelect *sms = new SetupMenuSelect("Direction", RST_NONE, compassDeviationAction);
		sms->setHelp("Push button to start deviation action");
		sms->addEntry(skydirs[i]);
		top->addEntry(sms);
	}
}

static void options_menu_create_compasswind_compass_nmea(SetupMenu *top) {
	SetupMenuSelect *nmeaHdm = new SetupMenuSelect("Magnetic Heading", RST_NONE, nullptr, &compass_nmea_hdm);
	nmeaHdm->addEntry("Disable");
	nmeaHdm->addEntry("Enable");
	nmeaHdm->setHelp(
			"Enable/disable NMEA '$HCHDM' sentence generation for magnetic heading");
	top->addEntry(nmeaHdm);

	SetupMenuSelect *nmeaHdt = new SetupMenuSelect("True Heading", RST_NONE, nullptr, &compass_nmea_hdt);
	nmeaHdt->addEntry("Disable");
	nmeaHdt->addEntry("Enable");
	nmeaHdt->setHelp(
			"Enable/disable NMEA '$HCHDT' sentence generation for true heading");
	top->addEntry(nmeaHdt);
}

static void options_menu_create_compasswind_compass(SetupMenu *top) {
	SetupMenuSelect *compSensorCal = new SetupMenuSelect("Sensor Calibration", RST_NONE, compassSensorCalibrateAction);
	compSensorCal->addEntry("Cancel");
	compSensorCal->addEntry("Start");
	compSensorCal->addEntry("Show");
	compSensorCal->addEntry("Show Raw Data");
	compSensorCal->setHelp("Calibrate Magnetic Sensor, mandatory for operation");
	top->addEntry(compSensorCal);

	// Fixme replace by WMM
	SetupMenuValFloat *cd = new SetupMenuValFloat("Setup Declination", "°", compassDeclinationAction, false, &compass_declination);
	cd->setHelp("Set compass declination in degrees");
	top->addEntry(cd);

	SetupMenuSelect *devMenuA = new SetupMenuSelect("AutoDeviation", RST_NONE, nullptr, &compass_dev_auto);
	devMenuA->setHelp("Automatic adaptive deviation and precise airspeed evaluation method using data from circling wind");
	devMenuA->addEntry("Disable");
	devMenuA->addEntry("Enable");
	top->addEntry(devMenuA);

	SetupMenu *devMenu = new SetupMenu("Setup Deviations", options_menu_create_compasswind_compass_dev);
	devMenu->setHelp("Compass Deviations", 280);
	top->addEntry(devMenu);

	// Show comapss deviations
	DisplayDeviations *smd = new DisplayDeviations("Show Deviations");
	top->addEntry(smd);

	SetupMenuSelect *sms = new SetupMenuSelect("Reset Deviations ", RST_NONE, compassResetDeviationAction);
	sms->setHelp("Reset all deviation data to zero");
	sms->addEntry("Cancel");
	sms->addEntry("Reset");
	top->addEntry(sms);

	SetupMenu *nmeaMenu = new SetupMenu("Setup NMEA", options_menu_create_compasswind_compass_nmea);
	top->addEntry(nmeaMenu);

	SetupMenuValFloat *compdamp = new SetupMenuValFloat("Damping", "sec", nullptr, false, &compass_damping);
	compdamp->setPrecision(1);
	top->addEntry(compdamp);
	compdamp->setHelp("Compass or magnetic heading damping factor in seconds");

	// Show compass settings
	ShowCompassSettings *scs = new ShowCompassSettings("Show Settings");
	top->addEntry(scs);
}

static void options_menu_create_compasswind_straightwind_filters(SetupMenu *top) {
	SetupMenuValFloat *smgsm = new SetupMenuValFloat("Airspeed Lowpass", "", nullptr, false, &wind_as_filter);
	smgsm->setPrecision(3);
	top->addEntry(smgsm);
	smgsm->setHelp(
			"Lowpass filter factor (per sec) for airspeed estimation from AS/Compass and GPS tracks");

	SetupMenuValFloat *devlp = new SetupMenuValFloat("Deviation Lowpass", "", nullptr, false, &wind_dev_filter);
	devlp->setPrecision(3);
	top->addEntry(devlp);
	devlp->setHelp(
			"Lowpass filter factor (per sec) for deviation table correction from AS/Compass and GPS tracks");

	SetupMenuValFloat *smgps = new SetupMenuValFloat("GPS Lowpass", "sec", nullptr, false, &wind_gps_lowpass);
	smgps->setPrecision(1);
	top->addEntry(smgps);
	smgps->setHelp(
			"Lowpass filter factor for GPS track and speed, to correlate with Compass latency");

	SetupMenuValFloat *wlpf = new SetupMenuValFloat("Averager", "", nullptr, false, &wind_filter_lowpass);
	wlpf->setPrecision(0);
	top->addEntry(wlpf);
	wlpf->setHelp("Number of measurements (seconds) averaged in straight flight live wind estimation");
}

static void options_menu_create_compasswind_straightwind_limits(SetupMenu *top) {
	SetupMenuValFloat *smdev = new SetupMenuValFloat("Deviation Limit", "°", nullptr, false, &wind_max_deviation);
	smdev->setHelp(
			"Maximum deviation change accepted when derived from AS/Compass and GPS tracks");
	top->addEntry(smdev);

	SetupMenuValFloat *smslip = new SetupMenuValFloat("Sideslip Limit", "°", nullptr, false, &swind_sideslip_lim);
	smslip->setPrecision(1);
	top->addEntry(smslip);
	smslip->setHelp(
			"Maximum side slip estimation in ° accepted in straight wind calculation");

	SetupMenuValFloat *smcourse = new SetupMenuValFloat("Course Limit", "°", nullptr, false, &wind_straight_course_tolerance);
	smcourse->setPrecision(1);
	top->addEntry(smcourse);
	smcourse->setHelp(
			"Maximum delta angle in ° per second accepted for straight wind calculation");

	SetupMenuValFloat *aslim = new SetupMenuValFloat("AS Delta Limit", "km/h", nullptr, false, &wind_straight_speed_tolerance);
	aslim->setPrecision(0);
	top->addEntry(aslim);
	aslim->setHelp(
			"Maximum delta in airspeed estimation from wind and GPS during straight flight accepted for straight wind calculation");
}

static void options_menu_create_compasswind_straightwind(SetupMenu *top) {
	SetupMenu *strWindFM = new SetupMenu("Filters", options_menu_create_compasswind_straightwind_filters);
	top->addEntry(strWindFM);
	SetupMenu *strWindLM = new SetupMenu("Limits", options_menu_create_compasswind_straightwind_limits);
	top->addEntry(strWindLM);
	ShowStraightWind *ssw = new ShowStraightWind("Straight Wind Status");
	top->addEntry(ssw);
}

void options_menu_create_compasswind_circlingwind(SetupMenu *top) {
	SetupMenuValFloat *cirwd = new SetupMenuValFloat("Max Delta", "°", nullptr, false, &max_circle_wind_diff);
	top->addEntry(cirwd);
	cirwd->setHelp(
			"Maximum accepted value for heading error in circling wind calculation");

	SetupMenuValFloat *cirlp = new SetupMenuValFloat("Averager", "", nullptr, false, &circle_wind_lowpass);
	cirlp->setPrecision(0);
	top->addEntry(cirlp);
	cirlp->setHelp(
			"Number of circles used for circling wind averager. A value of 1 means no average");

	SetupMenuValFloat *cirwsd = new SetupMenuValFloat("Max Speed Delta", "km/h", nullptr, false, &max_circle_wind_delta_speed);
	top->addEntry(cirwsd);
	cirwsd->setPrecision(1);
	cirwsd->setHelp(
			"Maximum wind speed delta from last measurement accepted for circling wind calculation");

	SetupMenuValFloat *cirwdd = new SetupMenuValFloat("Max Dir Delta", "°", nullptr, false, &max_circle_wind_delta_deg);
	top->addEntry(cirwdd);
	cirwdd->setPrecision(1);
	cirwdd->setHelp(
			"Maximum wind direction delta from last measurement accepted for circling wind calculation");

	// Show Circling Wind Status
	ShowCirclingWind *scw = new ShowCirclingWind("Circling Wind Status");
	top->addEntry(scw);
}

void options_menu_create_compasswind(SetupMenu *top) { // dynamic!
	if ( top->getNrChilds() == 0 ) {
		SetupMenu *compassMenu = new SetupMenu("Compass", options_menu_create_compasswind_compass);
		top->addEntry(compassMenu);

		// Wind speed observation window
		SetupMenuSelect *windcal = new SetupMenuSelect("Wind Calculation", RST_NONE, windSettingsAction, &wind_enable);
		windcal->addEntry("Disable", WA_OFF);
		windcal->addEntry("Straight", WA_STRAIGHT);
		windcal->addEntry("Circling", WA_CIRCLING);
		windcal->addEntry("Both", WA_BOTH);
		windcal->setHelp("Enable Wind calculation for straight flight (needs compass), circling, both or external source");
		top->addEntry(windcal);

		// Display option
		SetupMenuSelect *winddis = new SetupMenuSelect("Display", RST_NONE, nullptr, &wind_display);
		winddis->addEntry("Disable");
		winddis->addEntry("Wind Digits");
		winddis->addEntry("Wind Arrow");
		winddis->addEntry("Wind Both");
		winddis->addEntry("Compass");
		winddis->setHelp(
				"What is to be displayed, as digits or arrow or both, on retro style screen. If no wind available, compass is shown");
		top->addEntry(winddis);

		// Wind speed observation window
		SetupMenuSelect *windref = new SetupMenuSelect("Arrow Ref", RST_NONE, nullptr, &wind_reference);
		windref->addEntry("North");
		windref->addEntry("Mag Heading");
		windref->addEntry("GPS Course");
		windref->setHelp(
				"Choose wind arrow relative to geographic north or to true aircraft heading");
		top->addEntry(windref);

		SetupMenu *strWindM = new SetupMenu("Straight Wind", options_menu_create_compasswind_straightwind);
		top->addEntry(strWindM);
		strWindM->setHelp("Straight flight wind calculation needs compass module active",250);

		SetupMenu *cirWindM = new SetupMenu("Circling Wind", options_menu_create_compasswind_circlingwind);
		top->addEntry(cirWindM);

		SetupMenuSelect *windlog = new SetupMenuSelect("Wind Logging", RST_NONE, nullptr, &wind_logging);
		windlog->addEntry("Disable");
		windlog->addEntry("Wind");
		windlog->addEntry("GYRO/MAG");
		windlog->addEntry("Both");
		windlog->setHelp("Enable Wind logging NMEA output, to Navi port");
		top->addEntry(windlog);
	}
	// compass menu only accessible with a connected compass
	SetupMenu *cmenu = static_cast<SetupMenu*>(top->getEntry(0));
	if ( DEVMAN->getDevice(MAGSENS_DEV) != nullptr ||
			DEVMAN->getDevice(MAGLEG_DEV) != nullptr ) {
		cmenu->unlock();
		cmenu->setBuzzword();
	}
	else {
		cmenu->lock();
		cmenu->setBuzzword("n/a");
	}
}
