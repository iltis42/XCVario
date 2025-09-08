
#include "setup/SubMenuGlider.h"

#include "setup/SetupMenu.h"
#include "setup/SetupMenuSelect.h"
#include "setup/SetupMenuValFloat.h"
#include "setup/SetupMenu.h"
#include "setup/SetupNG.h"
#include "Flap.h"
#include "sensor.h"
#include "logdefnone.h"

static int polar_select(SetupMenuSelect *p) {
	ESP_LOGI(FNAME,"glider-index %d", p->getValue());
	glider_type_index.set(p->getValue());
	p->getParent()->setBuzzword(p->value());
	p->getParent()->setDirty();
	return 0;
}

int start_weight_adj(SetupMenuValFloat *p) {
	float wingload = (ballast_kg.get() + crew_weight.get() + empty_weight.get()) / polar_wingarea.get();
	ESP_LOGI(FNAME,"wingload:%.1f empty: %.1f cw:%.1f water:%.1f", wingload, empty_weight.get(), crew_weight.get(), ballast_kg.get() );
	MYUCG->setFont(ucg_font_fub25_hr, true);
	MYUCG->setPrintPos(1, 110);
	MYUCG->printf("%0.2f kg/m2  ", wingload);
	MYUCG->setFont(ucg_font_ncenR14_hr);
	return 0;
}

static void glider_menu_create_polarpoints(SetupMenu *top) {
	SetupMenuValFloat *wil = new SetupMenuValFloat("Ref Wingload", "kg/m2", nullptr, false, &polar_wingload);
	wil->setHelp(
			"Wingloading that corresponds to the 3 value pairs for speed/sink of polar");
	top->addEntry(wil);
	SetupMenuValFloat *pov1 = new SetupMenuValFloat("Speed 1", "km/h", nullptr, false, &polar_speed1);
	pov1->setHelp("Speed 1, near minimum sink from polar e.g. 80 km/h");
	top->addEntry(pov1);
	SetupMenuValFloat *pos1 = new SetupMenuValFloat("Sink  1", "m/s", nullptr, false, &polar_sink1);
	top->addEntry(pos1);
	SetupMenuValFloat *pov2 = new SetupMenuValFloat("Speed 2", "km/h", nullptr, false, &polar_speed2);
	pov2->setHelp("Speed 2 for a moderate cruise from polar e.g. 120 km/h");
	top->addEntry(pov2);
	SetupMenuValFloat *pos2 = new SetupMenuValFloat("Sink  2", "m/s", nullptr, false, &polar_sink2);
	top->addEntry(pos2);
	SetupMenuValFloat *pov3 = new SetupMenuValFloat("Speed 3", "km/h", nullptr, false, &polar_speed3);
	pov3->setHelp("Speed 3 for a fast cruise from polar e.g. 170 km/h");
	top->addEntry(pov3);
	SetupMenuValFloat *pos3 = new SetupMenuValFloat("Sink  3", "m/s", nullptr, false, &polar_sink3);
	top->addEntry(pos3);
}


static void position_labels_menu_create(SetupMenu* top){
	SetupMenuSelect *flab = new SetupMenuSelect( "Flap Label +3", RST_NONE, nullptr, &wk_label_plus_3 );
	flab->addEntryList( flap_labels ); // Initialize Flap Label Entries
	top->addEntry( flab );
	flab = new SetupMenuSelect( "Flap Label +2", RST_NONE, nullptr, &wk_label_plus_2 );
	flab->addEntryList( flap_labels );
	top->addEntry( flab );
	flab = new SetupMenuSelect( "Flap Label +1", RST_NONE, nullptr, &wk_label_plus_1 );
	flab->addEntryList( flap_labels );
	top->addEntry( flab );
	flab = new SetupMenuSelect( "Flap Label  0", RST_NONE, nullptr, &wk_label_null_0 );
	flab->addEntryList( flap_labels );
	top->addEntry( flab );
	flab = new SetupMenuSelect( "Flap Label -1", RST_NONE, nullptr, &wk_label_minus_1 );
	flab->addEntryList( flap_labels );
	top->addEntry( flab );
	flab = new SetupMenuSelect( "Flap Label -2", RST_NONE, nullptr, &wk_label_minus_2 );
	flab->addEntryList( flap_labels );
	top->addEntry( flab );
	flab = new SetupMenuSelect( "Flap Label -3", RST_NONE, nullptr, &wk_label_minus_3 );
	flab->addEntryList( flap_labels );
	top->addEntry( flab );
}

static void flap_speeds_menu_create(SetupMenu* top){
	SetupMenuValFloat *flgnd = new SetupMenuValFloat("Takeoff Flap","", nullptr, false, &flap_takeoff  );
	flgnd->setHelp("Flap position to be set on ground for takeoff, when there is no airspeed");
	top->addEntry( flgnd );

	SetupMenu *flapls = new SetupMenu("Flap Position Labels", position_labels_menu_create);
	top->addEntry( flapls );

	SetupMenuValFloat *plus3 = new SetupMenuValFloat("Speed +3 to +2", "", nullptr, false, &flap_plus_2  );
	top->addEntry( plus3 );

	SetupMenuValFloat *plus2 = new SetupMenuValFloat("Speed +2 to +1", "", nullptr, false, &flap_plus_1  );
	top->addEntry( plus2 );

	SetupMenuValFloat *plus1 = new SetupMenuValFloat("Speed +1 to  0", "", nullptr, false, &flap_0  );
	top->addEntry( plus1 );

	SetupMenuValFloat *min1 = new SetupMenuValFloat("Speed  0 to -1", "", nullptr, false, &flap_minus_1  );
	top->addEntry( min1 );

	SetupMenuValFloat *min2 = new SetupMenuValFloat("Speed -1 to -2", "", nullptr, false, &flap_minus_2  );
	top->addEntry( min2 );

	SetupMenuValFloat *min3 = new SetupMenuValFloat("Speed -2 to -3", "", nullptr, false, &flap_minus_3  );
	top->addEntry( min3 );
}

void glider_menu_create(SetupMenu *top) {
	ESP_LOGI(FNAME, "glider_menu_create");
	if ( top->getNrChilds() == 0 ) {
		SetupMenuSelect *glt = new SetupMenuSelect("Type", RST_NONE, polar_select, &glider_type_index);
		top->addEntry(glt);
		ESP_LOGI(FNAME, "#polars %d", Polars::numPolars());
		for (int x = 0; x < Polars::numPolars(); x++) {
			ESP_LOGI(FNAME, "P: %s - %d", Polars::getPolarName(x), Polars::getPolarIndex(x));
			glt->addEntry(Polars::getPolarName(x), Polars::getPolarIndex(x));
		}
		glt->setSelect(Polars::getGliderEnumPos()); // glider type nvs variable contains the magic index value
		ESP_LOGI(FNAME, "Number of Polars installed: %d", Polars::numPolars() );

		SetupMenu *pa = new SetupMenu("Polar Points", glider_menu_create_polarpoints);
		pa->setHelp("Adjust the polar at 3 points, in the commonly used metric system");
		top->addEntry(pa);

		SetupMenuValFloat *maxbal = new SetupMenuValFloat("Max Ballast", "liters", nullptr, false, &polar_max_ballast);
		top->addEntry(maxbal);

		SetupMenuValFloat *wingarea = new SetupMenuValFloat("Wing Area", "m2", nullptr, false, &polar_wingarea);
		top->addEntry(wingarea);

		SetupMenuValFloat *fixball = new SetupMenuValFloat("Empty Weight", "kg", start_weight_adj, false, &empty_weight);
		fixball->setPrecision(0);
		fixball->setHelp("Net rigged weight of the glider, according to the weight and balance plan");
		fixball->setNeverInline();
		top->addEntry(fixball);

		SetupMenuValFloat *vmax = new SetupMenuValFloat("Maximum Speed", "", nullptr, false, &v_max);
		vmax->setHelp("Configure maximum speed for corresponding aircraft type");
		top->addEntry(vmax);

		ESP_LOGI(FNAME,"glider-index %d", Polars::getGliderEnumPos());
		SetupMenu *flaps = new SetupMenu("Flap Speeds", flap_speeds_menu_create);
		flaps->setHelp("Transition speed for flap settings at ref wingload. Set to 0, if not aplicable");
		top->addEntry( flaps );
	}

	SetupMenu *tmp_menu = static_cast<SetupMenu*>(top->getEntry(6)); // flap speeds
	if( Polars::hasFlaps() ){
		tmp_menu->unlock();
		tmp_menu->setBuzzword();
	}else{
		tmp_menu->lock();
		tmp_menu->setBuzzword("No flaps");
	}


}

