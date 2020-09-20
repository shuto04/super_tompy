#include <Arduino.h>
#include <M5Stack.h>
#include <RhythmServo.h>

#define SERVO_NUM 3
#define SERVO_ALL_NUM 12

#define BEAT_LEN 16

#define INITIAL_ANGLE 90

typedef enum {
    Pause = 0,
    Play,
    NumState
} State;
State state = Pause;

uint8_t beats[SERVO_NUM][BEAT_LEN] = {{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                                      {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
                                      {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}};
int beatIndex = 0;
// int beatInterval = 125; // 120 bpm -> 2 beat / 1sec -> 500msec / 4つ打ち (500 / 4) = 125
int beatInterval = 176; // 85 bpm
unsigned long lastUpdate;

RhythmServo servos[SERVO_NUM] = {RhythmServo(1, beatInterval, INITIAL_ANGLE), 
                                 RhythmServo(3, beatInterval, INITIAL_ANGLE),
                                 RhythmServo(5, beatInterval, INITIAL_ANGLE)};


void servo_reset(int ang)
{
    for(int i=0; i<SERVO_ALL_NUM;++i)
    {
        servos[i].Reset(ang);
    }
    delay(3000);
}

void servo_update()
{
    unsigned long current_time = millis();
    if(current_time - lastUpdate > beatInterval){
        lastUpdate = current_time;
        beatIndex = (beatIndex+1) % BEAT_LEN;
    }

    for(int i=0; i<SERVO_NUM;++i)
    {
        servos[i].Update(current_time, beats[i][beatIndex]);
    }
}

void setup() {
    M5.begin(true, false, true);
    M5.Lcd.println("Super tompy");
    M5.Lcd.println("Start by Left button");

    Wire.begin(21, 22, 100000);

    servo_reset(INITIAL_ANGLE);

    state = Pause;
}

void input()
{
    M5.update();
    if(M5.BtnA.wasPressed())
    {
        state = State((state + 1) % NumState);
        if (state == Play)  
            M5.Lcd.println("Play");
        else
            M5.Lcd.println("Pause");
    }
}

void update()
{
    if (state == Play)
    {
        servo_update();
    }
}

void loop()
{
    input();
    update();
}