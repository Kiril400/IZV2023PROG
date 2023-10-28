#ifndef GyverTimer_h
#define GyverTimer_h
#include <Arduino.h>
enum timerType {
    US,
    MS,
};
class GTimer {
public:
    GTimer(timerType type = MS, uint32_t interval = 0);
    void setInterval(uint32_t interval);	
    void setTimeout(uint32_t timeout);		
    boolean isReady();						
    boolean isEnabled();					
    void reset();							
    void start();							
    void stop();								
    void resume();							
    
    void setMode(boolean mode);				
    
private:
    uint32_t _timer = 0;
    uint32_t _interval = 0;
    uint32_t _resumeBuffer = 0;
    boolean _mode = true;
    boolean _state = false;
    boolean _type = true;
};

#define MANUAL 0
#define AUTO 1
#define TIMER_TIMEOUT 0
#define TIMER_INTERVAL 1


class GTimer_ms {
public:
    GTimer_ms();							
    GTimer_ms(uint32_t interval);			
    void setInterval(uint32_t interval);	
    void setTimeout(uint32_t interval);		
    void setMode(uint8_t mode);				
    boolean isReady();						
    void reset();							
    void stop();							
    void start();							
    
private:
    uint32_t _timer = 0;
    uint32_t _interval = 1000;
    boolean _mode = true;
    boolean _state = true;
};

class GTimer_us {
public:
    GTimer_us();							
    GTimer_us(uint32_t interval);			
    void setInterval(uint32_t interval);	
    void setTimeout(uint32_t interval);		
    void setMode(uint8_t mode);				
    boolean isReady();						
    void reset();						
    void stop();							
    void start();							
    
private:
    uint32_t _timer = 0;
    uint32_t _interval = 1000;
    boolean _mode = true;
    boolean _state = true;
};
#endif
