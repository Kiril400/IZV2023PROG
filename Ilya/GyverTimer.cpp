// The provided code is part of the "GyverTimer" library, which offers flexible timer functionality for Arduino projects. 
// This code defines the behavior of the GTimer and GTimer_ms classes, which are used for timer management.
// This code defines the core functionality of timers, both in milliseconds and microseconds.
#include "GyverTimer.h"
// Include the GyverTimer library for timer functionality.

GTimer::GTimer(timerType type, uint32_t interval) {
    setInterval(interval);
    _type = type;
}
// Constructor for the GTimer class. Sets the timer interval and type (millis or micros).

void GTimer::setInterval(uint32_t interval) {
    if (interval != 0) {
        _interval = interval;
        _mode = TIMER_INTERVAL;
        start();
    } else stop();
}
// Sets the timer interval, mode (interval or timeout), and starts the timer if the interval is not zero.

void GTimer::setTimeout(uint32_t timeout) {
    setInterval(timeout);
    _mode = TIMER_TIMEOUT;
}
// Sets the timer for a timeout mode, which is similar to interval but disables the timer after expiration.

void GTimer::stop() {
    _state = false;
    _resumeBuffer = ( (_type) ? millis() : micros() ) - _timer;
}
// Stops the timer and stores a buffer for resuming the timer later.

void GTimer::resume() {
    start();
    _timer -= _resumeBuffer;
}
// Resumes the timer after it was stopped, accounting for the time elapsed during the stop.

void GTimer::start() {
    _state = true;
    reset();
}
// Starts the timer and resets the timer value.

void GTimer::reset() {
    _timer = (_type) ? millis() : micros();
}
// Resets the timer value to the current millis or micros time.

boolean GTimer::isEnabled() {
    return _state;
}
// Checks if the timer is enabled.

boolean GTimer::isReady() {
    if (!_state) return false;
    uint32_t thisTime = (_type) ? millis() : micros();
    if (thisTime - _timer >= _interval) {
        if (_mode) {
            do {
                _timer += _interval;
                if (_timer < _interval) break;
            } while (_timer < thisTime - _interval);
        } else {
            _state = false;
        }
        return true;
    } else {
        return false;
    }
}
// Checks if the timer has reached its set interval or timeout.

void GTimer::setMode(boolean mode) {
    _mode = mode;
}
// Sets the mode of the timer (interval or timeout).

GTimer_ms::GTimer_ms() {}

GTimer_ms::GTimer_ms(uint32_t interval) {
    _interval = (interval == 0) ? 1 : interval;
    reset();
}
// Constructors for GTimer_ms class, similar to GTimer class but for milliseconds.

void GTimer_ms::setInterval(uint32_t interval) {
    _interval = (interval == 0) ? 1 : interval;
    // Set the timer interval in milliseconds, ensuring it is at least 1 millisecond.
    GTimer_ms::reset();
    // Reset the timer to the current millisecond time.
    _state = true;
    // Enable the timer.
    _mode = true;
    // Set the timer mode to interval.
}

void GTimer_ms::setTimeout(uint32_t interval) {
    setInterval(interval);
    // Set the timer for a timeout mode, similar to an interval but disables the timer after expiration.
    _mode = false;
    // Set the timer mode to timeout.
}

void GTimer_ms::setMode(uint8_t mode) {
    _mode = mode;
    // Set the mode of the timer (interval or timeout).
}

void GTimer_ms::start() {
    _state = true;
    // Start the timer.
}

void GTimer_ms::stop() {
    _state = false;
    // Stop the timer.
}

boolean GTimer_ms::isReady() {
    if (!_state) return false;
    // If the timer is not enabled, return false.
    uint32_t thisMls = millis();
    // Get the current time in milliseconds.
    if (thisMls - _timer >= _interval) {
        // If the time elapsed is greater than or equal to the interval.
        if (_mode) {
            do {
                _timer += _interval;
                if (_timer < _interval) break;
                // Adjust the timer for multiple intervals that may have passed.
            } while (_timer < thisMls - _interval);
        } else {
            _state = false;
            // Disable the timer in timeout mode.
        }
        return true;
    } else {
        return false;
    }
}

void GTimer_ms::reset() {
    _timer = millis();
    // Reset the timer to the current millisecond time.
}
// These methods and functions are similar to the GTimer class but operate in milliseconds.

