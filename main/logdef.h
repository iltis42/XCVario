#pragma once

#include <esp_log.h>

// __FILENAME__ liefert nur den Dateinamen, ohne Pfad
#define FNAME __FILENAME__

#if defined(CONFIG_LOG_DISABLED)
# include "logdefnone.h"
#endif
