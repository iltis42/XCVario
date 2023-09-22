#ifndef _EXCESS_TRACKER_H_
#define _EXCESS_TRACKER_H_

#include <Preferences.h> // Wrapper around the Non-volatile storage on ESP32 processor
#include <string>

#define NAMESPACE "XCVario" // Note: Namespace name is limited to 15 chars.
#define READ_ONLY  false

/**
 * ExcessTracker saves speed and G-Load excesses permanently with the Preferences API.
 * It has 20 kb peserved memory.
 * 
*/
class ExcessTracker
{
public:

    /**
     * ExcessTracker Ctor.
     * Opens the Preferences with the "XCVario" Namespace in RW-Mode.
    */
	ExcessTracker();

    /**
     * ExcessTracker Destruktor.
     * Closes the Preferences.
    */
	virtual ~ExcessTracker();

    /**
     * Deletes all keys and values from the "XCVario" Namespace
    */
    void pugeData();

    /**
     * Saves the speed or the G-Load excess to Preferences with a timestamp.
     * 
     * @param speed2Fly the speed to fly
     * @param gLoad the G-Load
     * @returns 1 if data was successfully stored
     *          0 if the storage is full
    */
    int trackExcess(const float speed2Fly, const float gLoad);

    // ToDo: searchable Excess

private:
    Preferences preferences;
    ESP32Time rtc(3600);  // offset in seconds GMT+1
};

#endif