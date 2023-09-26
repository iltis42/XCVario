#ifndef _EXCESS_TRACKER_H_
#define _EXCESS_TRACKER_H_

#include <Preferences.h> // Wrapper around the Non-volatile storage on ESP32 processor
#include <string>
#include <vector>
#include <utility>
#include <esp_timer.h>

#define NAMESPACE "XCVario" // Note: Namespace name is limited to 15 chars.
#define READ_ONLY  false
#define MICRO_SEC_TO_SEC 1000000.0f

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
     * Saves the speed or the G-Load excess to Preferences.
     * 
     * @param speed2Fly the speed to fly
     * @param gLoad the G-Load
     * @returns 1 if data was successfully stored
     *          0 if the storage is full
    */
    int trackExcess(float_t speed2Fly, float_t gLoad);

    /**
     * Saves the speed or the G-Load excess to Preferences with its duration if it is greather 1e-6 seconds.
     * 
     * @param speed2Fly the speed to fly
     * @param gLoad the G-Load
     * @param duration the duration of the excess
     * @returns 1 if data was successfully stored
     *          0 if the storage is full
    */
    int trackExcessDuration(float_t speed2Fly, float_t gLoad, float_t duration);

    /**
     * Get Rows stored at the index.
     * 
     * @returns the [Speed, G-Load, Duration] List located at the index
     *          (0.0f, 0.0f, 0.0f) if the index is smaller zero or 
     *            greater or equal the row count
    */
    std::vector<float> getRow(uint16_t index);

    /**
     * Starts the excess timer and sets the isExcess property (to true).
     * Then it updates the excess values (Speed2Fly, G-Load) 
     * if they are greather then the current maxima.
    */
    void setExcess(float_t speed2Fly, float_t gLoad);

    /**
     * If there is currently no excess set, then it does nothing.
     * 
     * Stops the excess timer and updates the isExcess property (to false).
     * Stores the Maxima of the Excess together with its duration.
     * Resets the Maxima to zero.
    */
    void stopExcess();

private:
    Preferences preferences;

    bool isExcess = false;
    int64_t startTimeMicoS = 0;
    float_t maxSpeed2Fly = 0.0f;
    float_t maxGLoad = 0.0f;


    int storeExcess(const uint16_t index, float_t speed2Fly, float_t gLoad);
    int storeDuration(const uint16_t index, float_t duration);
    void updateSize(const uint16_t newSize);
};

#endif