#include "ExcessTracker.h"

#define ENTRY_SIZE_KEY "Entries"
#define SPEED2FLY_POSTFIX "_S2F"
#define GLOAD_POSTFIX "_G"
#define DURATION_POSTFIX "_S"
#define ENTRY_COUNT_PER_EXCESS 6
#define ENTRY_COUNT_DURATION 3

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

int ExcessTracker::storeExcess(const uint16_t index, float_t speed2Fly, float_t gLoad)
{
    int result = 1; // SUCCESSFULL
    const size_t freeEntires = preferences.freeEntries();

    if (freeEntires >= index + ENTRY_COUNT_PER_EXCESS)
    { // Two entires are free
        std::string prefix = std::to_string(index);

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
    }
    else
    {
        result = 0;
    }
    return result;
}

int ExcessTracker::storeDuration(const uint16_t index, float_t duration)
{
    int result = 1; // SUCCESSFULL

    const size_t freeEntires = preferences.freeEntries();
    if (freeEntires >= index + ENTRY_COUNT_DURATION)
    { // Two entires are free
        std::string prefix = std::to_string(index);
        std::string durationKey = prefix + DURATION_POSTFIX;
        size_t bytesWritten = preferences.putFloat( // write G-Load using 3 entries
            durationKey.c_str(),
            duration);
        assert(bytesWritten > 0);
    }
    else
    {
        result = 0;
    }
    return result;
}

void ExcessTracker::updateSize(const uint16_t newSize)
{
    size_t bytesWritten = preferences.putUInt( // write new Number of entries
        ENTRY_SIZE_KEY,
        newSize);
    assert(bytesWritten > 0);
}

int ExcessTracker::trackExcess(float_t speed2Fly, float_t gLoad)
{
    return trackExcessDuration(speed2Fly, gLoad, 0.0f);
}

int ExcessTracker::trackExcessDuration(float_t speed2Fly, float_t gLoad, float_t duration)
{
    const uint16_t size = getRowCount();
    int result = storeExcess(size, speed2Fly, gLoad);

    if (result == 1)
    { // Only save duration and update size if there was enugh space to store the excess.
        if (duration > 0.000001f) 
        { // No need to save the duration if there is none.
            result = storeDuration(size, duration);
        }
        updateSize(size + 1);
    }

    return result;
}

std::vector<float> ExcessTracker::getRow(uint16_t index)
{
    if (index < 0 || index >= getRowCount())
    { // avoid out of bound accesses
        return {0.0f, 0.0f, 0.0f};
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

    std::string durationKey = prefix + DURATION_POSTFIX;
    float duration = preferences.getFloat(
        durationKey.c_str(),
        0.0f);

    return {speed2Fly, gLoad, duration};
}

void ExcessTracker::setExcess(float_t speed2Fly, float_t gLoad)
{
    if (!isExcess)
    {
        startTimeMicoS = esp_timer_get_time();
        isExcess = true;
    }

    if (gLoad > maxGLoad)
    {
        maxGLoad = gLoad;
    }
    if (speed2Fly > maxSpeed2Fly)
    {
        maxSpeed2Fly = maxSpeed2Fly;
    }
}

void ExcessTracker::stopExcess()
{
    if (isExcess)
    {
        int64_t endTimeMicoS = esp_timer_get_time();
        int64_t durationMicorS = endTimeMicoS - startTimeMicoS;
        float_t durationS = ((float_t)durationMicorS) / MICRO_SEC_TO_SEC;

        int r = trackExcessDuration(maxSpeed2Fly, maxGLoad, durationS);
        assert(r == 1);

        startTimeMicoS = 0;
        maxSpeed2Fly = 0.0f;
        maxGLoad = 0.0f;
        isExcess = false;
    }
}
