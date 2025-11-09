
#pragma once

#include <string_view>

class SetupMenu;

extern const std::string_view flap_labels[55];

void flap_menu_create_flap_sensor(SetupMenu *wkm);
void flap_levels_menu_create(SetupMenu* top);
void free_flap_menu();