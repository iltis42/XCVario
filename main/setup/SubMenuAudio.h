
#pragma once

class SetupMenu;
class SetupMenuSelect;
class SetupMenuValFloat;

void audio_menu_create(SetupMenu *top);
int update_range_entry_s(SetupMenuSelect *p);
int audio_setup_f(SetupMenuValFloat *p);
void free_audio_menu();