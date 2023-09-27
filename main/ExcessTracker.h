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

#define SPEED2FLY_BOUND  285.0f
#define GLOAD_UPPER_BOUND 10.0f
#define GLOAD_LOWER_BOUND  -7.5f

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
     * Getter for the Speed to Fly upper bound.
     * 
     * @returns the Speed to Fly
    */
    float_t getSpeed2FlyBound();

    /**
     * Setter for the Speed to Fly upper bound.
     * 
     * @param speed2FlyBound the Speed to Fly bound to be set.
    */
    void setSpeed2FlyBound(float_t speed2FlyBound);

    /**
     * Getter for the upper G-Load Bound.
     * 
     * @returns the upper G-Load Bound
    */
    float_t getGLoadUpperBound();

    /**
     * Setter for the upper G-Load Bound.
     * 
     * @param gLoadUpperBound the new upper G-Load bound to be set
    */
    void setGLoadUpperBound(float_t gLoadUpperBound);
    
    /**
     * Getter for the lower G-Load Bound.
     * 
     * @returns the lower G-Load Bound
    */
    float_t getGLoadLowerBound();

    /**
     * Setter for the lower G-Load Bound
     * 
     * @param the new lower G-Load bound th be set
    */
    void setGLoadLowerBound(float_t gLoadLowerBound);

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
     * @param index the index of the element in range [0; size[
     * @returns the [Speed, G-Load, Duration] List located at the index
     *          (0.0f, 0.0f, 0.0f) if the index is smaller zero or 
     *            greater or equal the row count
    */
    std::vector<float> getRow(uint16_t index);

    /**
     * Checks weather the provided values constitute a excess, 
     * with the configured bounds and the choosen model (static/dynamic).
     * 
     * @param speed2Fly the current speed to fly
     * @param gLoad the current gload
     * @returns true if it is a excess
     *          false otherwise
    */
    bool isExcess(float_t speed2Fly, float_t gLoad);

    /**
     * Starts the excess timer and sets the isExcess property (to true).
     * Then it updates the excess values (Speed2Fly, G-Load) 
     * if they are greather then the current maxima.
     * 
     * @param speed2Fly the current speed to fly
     * @param gLoad the current gload
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

    bool isStaticModel = true;

    float_t speed2FlyBound = SPEED2FLY_BOUND;
    float_t gLoadUpperBound = GLOAD_UPPER_BOUND;
    float_t gLoadLowerBound = GLOAD_LOWER_BOUND;

    bool excessStarted = false;
    int64_t startTimeMicoS = 0;
    float_t maxSpeed2Fly = 0.0f;
    float_t maxGLoad = 0.0f;


    int storeExcess(const uint16_t index, float_t speed2Fly, float_t gLoad);
    int storeDuration(const uint16_t index, float_t duration);
    void updateSize(const uint16_t newSize);
};

#endif