#include "ExcessTracker.h"

#define ENTRY_SIZE_KEY "Entries"
#define SPEED2FLY_POSTFIX "_S2F"
#define GLOAD_POSTFIX "_G"
#define ENTRY_COUNT_PER_EXCESS 6

ExcessTracker::ExcessTracker()
{
    // Open Preferences with XCVario namespace.
    assert(
        preferences.begin(NAMESPACE, READ_ONLY));
}

ExcessTracker::~ExcessTracker()
{
    // Close the Preferences
    preferences.end();
}

void ExcessTracker::pugeData()
{
    // Delete all keys and values
    // Note: The namespace name still exists afterward.
    assert(
        preferences.clear());
}

uint16_t ExcessTracker::getRowCount()
{
    return preferences.getUInt( // Get Number of entries
        ENTRY_SIZE_KEY,
        0 // return ZERO if the key is not available
    );
}

int ExcessTracker::trackExcess(float_t speed2Fly, float_t gLoad)
{
    int result = 1; // SUCCESSFULL
    const uint16_t size = this->getRowCount();
    const size_t freeEntires = preferences.freeEntries();

    if (freeEntires >= size + ENTRY_COUNT_PER_EXCESS)
    { // Two entires are free
        std::string prefix = std::to_string(size);

        size_t bytesWritten = 0;
        std::string S2FKey = prefix + SPEED2FLY_POSTFIX;
        bytesWritten = preferences.putFloat( // write Speed2Fly using 3 entires
            S2FKey.c_str(),
            speed2Fly);
        assert(bytesWritten > 0);

        std::string gLoadKey = prefix + GLOAD_POSTFIX;
        bytesWritten = preferences.putFloat( // write G-Load using 3 entries
            gLoadKey.c_str(),
            gLoad);
        assert(bytesWritten > 0);

        bytesWritten = preferences.putUInt( // write new Number of entries
            ENTRY_SIZE_KEY,
            size + 1);
        assert(bytesWritten > 0);
    }
    else
    {
        result = 0;
    }
    return result;
}

std::pair<float, float> ExcessTracker::getRow(uint16_t index)
{
    if (index < 0 || index >= this->getRowCount())
    { // avoid out of bound accesses
        return std::make_pair(0.0f, 0.0f);
    }

    const std::string prefix = std::to_string(index);

    std::string S2FKey = prefix + SPEED2FLY_POSTFIX;
    float speed2Fly = preferences.getFloat(
        S2FKey.c_str(),
        0.0f);

    std::string gLoadKey = prefix + GLOAD_POSTFIX;
    float gLoad = preferences.getFloat(
        gLoadKey.c_str(),
        0.0f);

    return std::make_pair(speed2Fly, gLoad);
}
