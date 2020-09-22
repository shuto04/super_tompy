#ifndef _H_ANIMATION_SERVO_
#define _H_ANIMATION_SERVO_

#include <Wire.h>

struct KeyFrame{
    int angle;     // deg
    int duration;  // msec
    KeyFrame(int angle, int duration) : angle(angle), duration(duration){}
};

class AnimationServo
{
    uint8_t _pin;
    KeyFrame* _pKeyframes;
    int _keyframeLength;
    int _keyframeIndex;
    int _pos;
    int _baseAng;
    int _increment;
    int _updateInterval;
    unsigned long _lastUpdate;

    public:
        AnimationServo(uint8_t pin, int baseAng, KeyFrame *keyframes, int keyframeLength)
        {
            _pin = pin;
            _pKeyframes = keyframes;
            _keyframeLength = keyframeLength;
            _keyframeIndex = 0;
            _baseAng = baseAng;
            _pos = _pKeyframes[_keyframeLength].angle;
            _updateInterval = _pKeyframes[_keyframeIndex].duration;  
        }

        void Reset()
        {
            if(_pos != _pKeyframes[0].angle)
            {
                _keyframeIndex = 0;
                _pos = _pKeyframes[_keyframeLength].angle;
                _updateInterval = _pKeyframes[_keyframeIndex].duration;  
                _write_angle(_baseAng);
            }
        }

        void Update()
        {
            unsigned long currentTime = millis();
            if((currentTime - _lastUpdate) > _updateInterval)
            {
                _lastUpdate = currentTime;
                _keyframeIndex = (_keyframeIndex+1) % _keyframeLength;
                _pos = _pKeyframes[_keyframeIndex].angle;
                _updateInterval = _pKeyframes[_keyframeIndex].duration;

                _write_angle(_pos);
            }
        }

        int Pos()
        {
            return _pos;
        }

        int KeyframeIndex()
        {
            return _keyframeIndex;
        }

        int KeyframeLength()
        {
            return _keyframeLength;
        }

        uint8_t Pin()
        {
            return _pin;
        }

        // addr 0x01 means "control the number 1 servo by us"
        void _write_us(uint16_t us) {
            Wire.beginTransmission(0x53);
            Wire.write(0x00 | _pin);
            Wire.write(us & 0x00ff);
            Wire.write(us >> 8 & 0x00ff);
            Wire.endTransmission();
        }

        // addr 0x11 means "control the number 1 servo by angle"
        void _write_angle(uint8_t angle) {
            
            Wire.beginTransmission(0x53);
            Wire.write(0x10 | _pin);
            Wire.write(angle);
            Wire.endTransmission();
        }        
};

#endif // _H_ANIMATION_SERVO_