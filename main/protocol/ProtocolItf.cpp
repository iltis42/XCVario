#include "ProtocolItf.h"
#include "logdef.h"

ProtocolItf::~ProtocolItf()
{
    ESP_LOGI(FNAME, "Dtor dev %d, proto %d", _did, getProtocolId());
}