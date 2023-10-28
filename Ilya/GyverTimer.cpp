#include "GyverTimer.h"

GTimer::GTimer(timerType type, uint32_t interval) {
    setInterval(interval);		
    _type = type;				
}


void GTimer::setInterval(uint32_t interval) {
    if (interval != 0) {				
        _interval = interval;			
        _mode = TIMER_INTERVAL;			
        start();						
    } else stop();						
}


void GTimer::setTimeout(uint32_t timeout) {
    setInterval(timeout);		
    _mode = TIMER_TIMEOUT;		
}


void GTimer::stop() {
    _state = false;
    _resumeBuffer = ( (_type) ? millis() : micros() ) - _timer;		
}


void GTimer::resume() {
    start();
    _timer -= _resumeBuffer;	
}

void GTimer::start() {
    _state = true;
    reset();
}

void GTimer::reset() {
    _timer = (_type) ? millis() : micros();
}


boolean GTimer::isEnabled() {
    return _state;
}

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

void GTimer::setMode(boolean mode) {
    _mode = mode;
}

GTimer_ms::GTimer_ms() {}

GTimer_ms::GTimer_ms(uint32_t interval) {
    _interval = (interval == 0) ? 1 : interval;	
    reset();
}

void GTimer_ms::setInterval(uint32_t interval) {
    _interval = (interval == 0) ? 1 : interval;		
    GTimer_ms::reset();
    _state = true;
    _mode = true;
}
void GTimer_ms::setTimeout(uint32_t interval) {
    setInterval(interval);
    _mode = false;
}
void GTimer_ms::setMode(uint8_t mode) {
    _mode = mode;
}
void GTimer_ms::start() {
    _state = true;
}
void GTimer_ms::stop() {
    _state = false;
}
boolean GTimer_ms::isReady() {	
    if (!_state) return false;
    uint32_t thisMls = millis();
    if (thisMls - _timer >= _interval) {
        if (_mode) {
            do {
                _timer += _interval;
                if (_timer < _interval) break;      
            } while (_timer < thisMls - _interval); 	
        } else {
            _state = false;
        }
        return true;
    } else {
        return false;
    }
}

void GTimer_ms::reset() {
    _timer = millis();
}

GTimer_us::GTimer_us() {}

GTimer_us::GTimer_us(uint32_t interval) {
    _interval = (interval == 0) ? 1 : interval;	
    reset();
}

void GTimer_us::setInterval(uint32_t interval) {
    _interval = (interval == 0) ? 1 : interval;	
    GTimer_us::reset();
    _state = true;
    _mode = true;
}
void GTimer_us::setTimeout(uint32_t interval) {
    setInterval(interval);
    _mode = false;
}
void GTimer_us::setMode(uint8_t mode) {
    _mode = mode;
}
void GTimer_us::start() {
    _state = true;
}
void GTimer_us::stop() {
    _state = false;
}
boolean GTimer_us::isReady() {
    if (!_state) return false;
    uint32_t thisUs = micros();
    if (thisUs - _timer >= _interval) {
        if (_mode) {
            do {
                _timer += _interval;
                if (_timer < _interval) break;    
            } while (_timer < thisUs - _interval);  		
        } else {
            _state = false;
        }
        return true;
    } else {
        return false;
    }
}

void GTimer_us::reset() {
    _timer = micros();
}
