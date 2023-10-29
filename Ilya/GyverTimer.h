 #ifndef GyverTimer_h
#define GyverTimer_h
// Header guard: Prevents double inclusion of the library.

#include <Arduino.h>
// Include the Arduino library for necessary functions and types.

enum timerType {
    US,
    MS,
};
// Define an enumeration to represent timer types: microseconds (US) and milliseconds (MS).

class GTimer {
public:
    GTimer(timerType type = MS, uint32_t interval = 0);
    // Constructor for the GTimer class with default type as milliseconds and default interval of 0.
    void setInterval(uint32_t interval);
    // Set the timer interval in milliseconds or microseconds.
    void setTimeout(uint32_t timeout);
    // Set the timer as a timeout with the specified duration.
    boolean isReady();
    // Check if the timer has elapsed and is ready.
    boolean isEnabled();
    // Check if the timer is enabled.
    void reset();
    // Reset the timer to the current time.
    void start();
    // Start the timer.
    void stop();
    // Stop the timer.
    void resume();
    // Resume a previously stopped timer.
    
    void setMode(boolean mode);
    // Set the timer mode (interval or timeout).

private:
    uint32_t _timer = 0;
    uint32_t _interval = 0;
    uint32_t _resumeBuffer = 0;
    boolean _mode = true;
    boolean _state = false;
    boolean _type = true;
};
// Definition of the GTimer class with methods for timer control and state.

#define MANUAL 0
#define AUTO 1
#define TIMER_TIMEOUT 0
#define TIMER_INTERVAL 1
// Macros and enum values to represent manual and auto modes, timer timeout and interval.

class GTimer_ms {
public:
    GTimer_ms();
    // Default constructor for GTimer_ms.
    GTimer_ms(uint32_t interval);
    // Constructor for GTimer_ms with a specified interval.
    void setInterval(uint32_t interval);
    // Set the timer interval in milliseconds.
    void setTimeout(uint32_t interval);
    // Set the timer as a timeout with the specified duration.
    void setMode(uint8_t mode);
    // Set the timer mode (manual or auto).
    boolean isReady();
    // Check if the timer has elapsed and is ready.
    void reset();
    // Reset the timer to the current time.
    void stop();
    // Stop the timer.
    void start();
    // Start the timer.
    
private:
    uint32_t _timer = 0;
    uint32_t _interval = 1000;
    boolean _mode = true;
    boolean _state = true;
};
// Definition of the GTimer_ms class for millisecond-based timers.

class GTimer_us {
public:
    GTimer_us();
    // Default constructor for GTimer_us.
    GTimer_us(uint32_t interval);
    // Constructor for GTimer_us with a specified interval.
    void setInterval(uint32_t interval);
    // Set the timer interval in microseconds.
    void setTimeout(uint32_t interval);
    // Set the timer as a timeout with the specified duration.
    void setMode(uint8_t mode);
    // Set the timer mode (manual or auto).
    boolean isReady();
    // Check if the timer has elapsed and is ready.
    void reset();
    // Reset the timer to the current time.
    void stop();
    // Stop the timer.
    void start();
    // Start the timer.
    
private:
    uint32_t _timer = 0;
    uint32_t _interval = 1000;
    boolean _mode = true;
    boolean _state = true;
};
// Definition of the GTimer_us class for microsecond-based timers.
#endif
