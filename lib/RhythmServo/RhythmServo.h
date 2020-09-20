#ifndef _H_RHYTHM_SERVO_
#define _H_RHYTHM_SERVO_

#include <Wire.h>
#define SERVO_ADDR 0x53

class RhythmServo
{
    uint8_t _pin;
    int _pos;
    int _minAng;
    int _maxAng;
    int _increment;
    int _updateInterval;
    unsigned long _lastUpdate;

    public:
        RhythmServo(uint8_t pin, int beatInterval, int initial_ang)
        {
            _pin = pin;
            _pos = _minAng = initial_ang;
            _maxAng = initial_ang + beatInterval / 10;         // 0 -> 20 -> 0 can be 100msec
            _increment = 1;
            _updateInterval = 5;  // 動作速度：0.3秒/60度 https://www.amazon.co.jp/dp/B07TYYLMVY
        }

        void Reset(int ang)
        {
            _write_angle(ang);
        }

        void Update(unsigned long current_time, uint8_t canPlay)
        {
            if((current_time - _lastUpdate) > _updateInterval && canPlay)
            {
                _lastUpdate = current_time;
                
                _pos += _increment;
                _write_angle(_pos);
                if ((_pos >= _maxAng) || (_pos <= _minAng)) // end of sweep
                {
                    // reverse direction
                    _increment = -_increment;
                }                
            }
        }

        int Pos()
        {
            return _pos;
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

#endif // _H_RHYTHM_SERVO_