#pragma once

typedef enum { RST_NONE, RST_ON_EXIT, RST_IMMEDIATE } e_restart_mode_t;

struct bitfield_select {
    e_restart_mode_t _restart :2;
    bool _ext_handler         :1;
    bool _save                :1;
    bool _end_setup            :1;
 };

 