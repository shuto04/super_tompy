#ifndef _H_SERVO_
#define _H_SERVO_

#include <Wire.h>
#define SERVO_ADDR 0x53

class Servo
{
    uint8_t _pin;
    int _pos;
    int _minAng;
    int _maxAng;
    int _increment;
    int _updateInterval;
    unsigned long _lastUpdate;

    public:
        Servo(uint8_t pin, int interval, int minAng, int maxAng)
        {
            _pin = pin;
            _updateInterval = interval;
            _minAng = minAng;
            _maxAng = maxAng;
            _increment = 1;
        }

        void Reset(int ang)
        {
            _write_us(ang);
        }

        void Update()
        {
            if((millis() - _lastUpdate) > _updateInterval)
            {
                _lastUpdate = millis();
                _pos += _increment;
                _write_angle(_pos);
                if ((_pos >= _maxAng) || (_pos <= _minAng)) // end of sweep
                {
                    // reverse direction
                    _increment = -_increment;
                }                
            }
        }

        // addr 0x01 means "control the number 1 servo by us"
        void _write_us(uint16_t us) {
            Wire.beginTransmission(SERVO_ADDR);
            Wire.write(0x00 | _pin);
            Wire.write(us & 0x00ff);
            Wire.write(us >> 8 & 0x00ff);
            Wire.endTransmission();
        }

        // addr 0x11 means "control the number 1 servo by angle"
        void _write_angle(uint8_t angle) {
            
            Wire.beginTransmission(SERVO_ADDR);
            Wire.write(0x10 | _pin);
            Wire.write(angle);
            Wire.endTransmission();
        }        
};

#endif // _H_SERVO_
