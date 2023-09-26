#ifndef _EXCESS_TRACKER_H_
#define _EXCESS_TRACKER_H_

#include <Preferences.h> // Wrapper around the Non-volatile storage on ESP32 processor
#include <string>
#include <utility>

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
     * Get the count of Rows stored in the Preferences storage.
     * 
     * @returns the count of rows
    */
    uint16_t getRowCount();

    /**
     * Saves the speed or the G-Load excess to Preferences with a timestamp.
     * 
     * @param speed2Fly the speed to fly
     * @param gLoad the G-Load
     * @returns 1 if data was successfully stored
     *          0 if the storage is full
    */
    int trackExcess(float_t speed2Fly, float_t gLoad);

    /**
     * Get Rows stored at the index.
     * 
     * @returns the (Speed, G-Load) Pair located at the index
     *          (0.0f, 0.0f) if the index is smaller zero or 
     *            greater or equal the row count
    */
    std::pair<float, float> getRow(uint16_t index);

private:
    Preferences preferences;
};

#endif