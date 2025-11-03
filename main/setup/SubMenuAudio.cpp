/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "setup/SubMenuAudio.h"

#include "ESPAudio.h"
#include "setup/SetupMenu.h"
#include "setup/SetupMenuSelect.h"
#include "setup/SetupMenuValFloat.h"
#include "setup/SetupMenu.h"
#include "setup/SetupNG.h"
#include "math/CompareFloat.h"
#include "Units.h"
#include "logdef.h"

#include <cinttypes>
#include <string>
#include <string_view>

static void audio_menu_create_tonestyle(SetupMenu *top);
static void audio_menu_create_deadband(SetupMenu *top);

static std::string range0;
static std::string range1;
static std::string range2;

static int8_t selected_profile;
static std::string prfl_name;

// audio profiles
struct AUDIO_PROFILE
{
    bool isEqual(const AUDIO_PROFILE &ap) const;
    const char* getName() const { return name.data(); }
    std::string_view name;
    // profile attributes
    float center;
    float maxvariation;
    bool  dual_tone;
    int   chopping;
    int   harmonics;
    float exponent;
};
bool AUDIO_PROFILE::isEqual(const AUDIO_PROFILE &ap) const
{
    if (!floatEqualFast(center, ap.center)) return false;
    if (!floatEqualFast(maxvariation, ap.maxvariation)) return false;
    if (dual_tone != ap.dual_tone) return false;
    if (chopping != ap.chopping) return false;
    if (harmonics != ap.harmonics) return false;
    if (!floatEqualFast(exponent, ap.exponent)) return false;
    return true;
}
constexpr int NUM_AUDIO_PRFLS = 7;
const std::array<AUDIO_PROFILE, NUM_AUDIO_PRFLS> audio_prfls = {{ 
    {"XCVario",    500., 2.,  false, BOTH_CHOP, AUD_HARM_HIGH, 1. }, 
    {"eXtended",   500., 3.,  false, BOTH_CHOP, AUD_HARM_HIGH, 1.2 },
    {"Ilec",       500., 3.,  true,  BOTH_CHOP, AUD_HARM_MED,  1.2 },
    {"Westerboer", 500., 1.2, false, BOTH_CHOP, AUD_HARM_MED,  1. },
    {"Zander",     600., 2.,  false, BOTH_CHOP, AUD_HARM_HIGH, 1. },
    {"Borgelt",    500., 1.4, false, BOTH_CHOP, AUD_HARM_LOW, 0.8 },
    {"Custom",     500., 2.,  false, BOTH_CHOP, AUD_HARM_OFF, 1. } }};

static int getAudioProfile()
{
    AUDIO_PROFILE ap = { "", center_freq.get(), tone_var.get(), dual_tone.get() != 0, chopping_mode.get(), audio_harmonics.get(), audio_factor.get() };
    int i = 0;
    for (; i < NUM_AUDIO_PRFLS-1; i++) {
        if (audio_prfls[i].isEqual(ap)) {
            break;
        }
    }
    selected_profile = i;
    prfl_name.assign(audio_prfls[i].getName());
    ESP_LOGI(FNAME,"audio profile %d: %s", selected_profile, prfl_name.c_str() );
    return i;
}
static int applyAudioProfile(SetupMenuSelect *p)
{
    if ( p->getSelect() < NUM_AUDIO_PRFLS-1 ) {
        selected_profile = p->getSelect();
        // apply profile
        const AUDIO_PROFILE &ap = audio_prfls[selected_profile];
        center_freq.set(ap.center);
        tone_var.set(ap.maxvariation);
        dual_tone.set(ap.dual_tone ? 1 : 0);
        chopping_mode.set(ap.chopping);
        audio_harmonics.set(ap.harmonics);
        audio_factor.set(ap.exponent);
        p->getParent()->setDirty();
    }
    // custom profile - do nothing
    return 0;
}

static int update_range_entry(SetupMenuValFloat *p)
{
    // ESP_LOGI(FNAME,"update_rentry vu:%s", Units::VarioUnit() );
    range0.assign("Fixed (5  ");
    range0 += Units::VarioUnit();
    range0 += ")";
    range1.assign("Fixed (10 ");
    range1 += Units::VarioUnit();
    range1 += ")";
    range2.assign("Variable (");
    range2 += std::to_string((int)(scale_range.get()));
    range2 += " ";
    range2 += Units::VarioUnit();
    range2 += ")";
    return 0;
}
int update_range_entry_s(SetupMenuSelect *p)
{
    return update_range_entry(nullptr);
}

static int audio_setup_s(SetupMenuSelect *p) {
    AUDIO->applySetup();
    p->getParent()->setDirty();
    return 0;
}
int audio_setup_f(SetupMenuValFloat *p) {
    AUDIO->applySetup();
    p->getParent()->setDirty();
    return 0;
}

static int show_vol_dflt(SetupMenuValFloat *p) {
    AUDIO->setVolume(p->getFloat());
    return 0;
}

void audio_menu_create_tonestyle(SetupMenu *top) { // dynamic!

    getAudioProfile();
    // update buzzword
    top->setBuzzword(prfl_name.c_str());

    if (top->getNrChilds() == 0)
    {
        top->setDynContent();

        SetupMenuSelect *prfl = new SetupMenuSelect("Preset scheme", RST_NONE, applyAudioProfile);
        prfl->setHelp("Choose one of the predefined audio profiles to preset audio parameters");
        for (const auto &p : audio_prfls) {
            prfl->addEntry(p.getName());
        }
        prfl->setSelect(selected_profile);
        top->addEntry(prfl);

        SetupMenuValFloat *cf = new SetupMenuValFloat("CenterFreq", "Hz", audio_setup_f, false, &center_freq);
        cf->setHelp("Center frequency for Audio at zero Vario or zero S2F delta");
        top->addEntry(cf);

        SetupMenuValFloat *oc = new SetupMenuValFloat("Tone Variation", "fold", audio_setup_f, false, &tone_var);
        oc->setHelp("Maximum tone frequency variation");
        top->addEntry(oc);

        SetupMenuSelect *dt = new SetupMenuSelect("Dual Tone", RST_NONE, audio_setup_s, &dual_tone);
        dt->setHelp("Select dual tone modue aka ilec sound (di/da/di), or single tone (di/di/di) mode");
        dt->mkEnable();
        top->addEntry(dt);

        SetupMenuSelect *tch = new SetupMenuSelect("Chopping", RST_NONE, audio_setup_s, &chopping_mode);
        tch->setHelp("Select tone chopping option on positive values for Vario and or S2F");
        tch->addEntry("Disabled");      // 0
        tch->addEntry("Vario only");    // 1
        tch->addEntry("S2F only");      // 2
        tch->addEntry("Vario and S2F"); // 3  default
        top->addEntry(tch);

        SetupMenuSelect *tharm = new SetupMenuSelect("Harmonics", RST_NONE, audio_setup_s, &audio_harmonics);
        tharm->setHelp("Add harmonics to the vario tone, making it a bit more crispy");
        tharm->addEntry("None", AUD_HARM_OFF);
        tharm->addEntry("Some", AUD_HARM_LOW);
        tharm->addEntry("More", AUD_HARM_MED);
        tharm->addEntry("Sparky", AUD_HARM_HIGH);
        top->addEntry(tharm);

        SetupMenuValFloat *afac = new SetupMenuValFloat("Audio Response", "", audio_setup_f, false, &audio_factor);
        afac->setHelp("How the audio frequency responds to the climb rate: < 1 for logarithmic, and > 1 for exponential, response");
        top->addEntry(afac);
    }
    else {
        // update selected profile
        SetupMenuSelect *prfl = static_cast<SetupMenuSelect*>(top->getEntry(0));
        prfl->setSelect(selected_profile);
        ESP_LOGI(FNAME,"set selection %d", selected_profile);
    }

}

void audio_menu_create_deadband(SetupMenu *top) {
    update_range_entry(0);
    SetupMenuSelect *audio_range_sm = new SetupMenuSelect("Range", RST_NONE, audio_setup_s, &audio_range);
    audio_range_sm->addEntry(range0.c_str());
    audio_range_sm->addEntry(range1.c_str());
    audio_range_sm->addEntry(range2.c_str());
    audio_range_sm->setHelp("Audio range: fixed, or variable according to current Vario display range setting");
    top->addEntry(audio_range_sm);

	SetupMenuValFloat *dbminlv = new SetupMenuValFloat("Lower Vario", "", audio_setup_f, false, &deadband_neg);
	top->addEntry(dbminlv);

	SetupMenuValFloat *dbmaxlv = new SetupMenuValFloat("Upper Vario", "", audio_setup_f, false, &deadband);
	top->addEntry(dbmaxlv);

	SetupMenuValFloat *dbmaxls2fn = new SetupMenuValFloat("Lower S2F", "", audio_setup_f, false, &s2f_deadband_neg);
	top->addEntry(dbmaxls2fn);

	SetupMenuValFloat *dbmaxls2f = new SetupMenuValFloat("Upper S2F", "", audio_setup_f, false, &s2f_deadband);
	top->addEntry(dbmaxls2f);
}


void audio_menu_create(SetupMenu *audio) {
    if (audio->getNrChilds() == 0)
    {
        SetupMenuSelect *ageda = new SetupMenuSelect("General Switch", RST_NONE, audio_setup_s, &audio_mute_gen);
        ageda->setHelp("All audio on, only alarms, or audio switched entirely off");
        ageda->addEntry("All On", AUDIO_ON);
        ageda->addEntry("Alarms Only", AUDIO_ALARMS);
        ageda->addEntry("No Sound", AUDIO_OFF);
        audio->addEntry(ageda);

        SetupMenuSelect *asida = new SetupMenuSelect("V-Tone at Sink", RST_NONE, audio_setup_s, &audio_mute_sink);
        asida->setHelp("Select whether vario audio will be on while in sink");
        asida->addEntry(ENABLE_MODE[0].data(), 1);
        asida->addEntry(ENABLE_MODE[1].data(), 0); // inverted logic
        audio->addEntry(asida);

        SetupMenuValFloat *dv = new SetupMenuValFloat("Default Volume", "%", show_vol_dflt, false, &default_volume);
        dv->setHelp("Default volume for Audio when device is switched on");
        audio->addEntry(dv);

        SetupMenuValFloat *flarmv = new SetupMenuValFloat("Alarm Vol. Raise", "%", nullptr, false, &alarm_volraise);
        flarmv->setHelp("Audio volume raise for alarms and warnings above normal volume (min. 60%)");
        audio->addEntry(flarmv);

        SetupMenuSelect *amspvol = new SetupMenuSelect("Split Volume", RST_NONE, nullptr, &audio_split_vol);
        amspvol->setHelp("Independent audio volumes for Cruise and Vario mode");
        amspvol->mkEnable();
        audio->addEntry(amspvol);

        SetupMenuSelect *abnm = new SetupMenuSelect("Cruise Audio", RST_NONE, nullptr, &cruise_audio_mode);
        abnm->setHelp("Select either S2F command or Vario tone while cruising");
        abnm->addEntry("Speed2Fly");
        abnm->addEntry("Vario");
        audio->addEntry(abnm);

        getAudioProfile();
        SetupMenu *astyle = new SetupMenu("Tone Style", audio_menu_create_tonestyle);
        astyle->setBuzzword(prfl_name.c_str());
        astyle->setHelp("Refine XCV audio by preset tone styles, or a customized parameter set");
        audio->addEntry(astyle);

        SetupMenu *db = new SetupMenu("Range & Deadbands", audio_menu_create_deadband);
        audio->addEntry(db);
        db->setHelp("Dead band limits within which audio remains silent.  1 m/s equals roughly 200 fpm or 2 knots");
    }
}


void free_audio_menu()
{
    range0.clear();
    range0.shrink_to_fit();
    range1.clear();
    range1.shrink_to_fit();
    range2.clear();
    range2.shrink_to_fit();
    prfl_name.clear();
    prfl_name.shrink_to_fit();
}
