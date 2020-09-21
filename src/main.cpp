#include <Arduino.h>
#include <M5Stack.h>
#include <RhythmServo.h>

#define SERVO_NUM 3

#define BEAT_LEN 16
#define PATTERN_LEN 16

typedef enum {
    Pause = 0,
    Play,
    NumState
} State;
State state = Pause;

uint8_t beat0[SERVO_NUM][BEAT_LEN] = {{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                                      {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
                                      {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}};
uint8_t beat1[SERVO_NUM][BEAT_LEN] = {{1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0},
                                      {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                      {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}};
// uint8_t patterns[PATTERN_LEN] = {0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1};
// uint8_t patterns[PATTERN_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t patterns[PATTERN_LEN] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

int beatIndex = 0;
int patternIndex = 0;

// int beatInterval = (60. / 120.) / 4 * 1000; // 120 bpm -> 2 beat / 1sec -> 500msec / 4つ打ち (500 / 4) = 125
// int beatInterval = (60. / 85.) / 4 * 1000;  // 85 bpm 
int beatInterval = (60. / 81.4) / 4 * 1000; // 81.4 bpm 
// int beatInterval = (60. / 60.) / 4 * 1000;  // 60 bpm
// int beatInterval = (60. / 30.) / 4 * 1000;  // 30 bpm

unsigned long lastUpdate;
unsigned long printLastUpdate;
bool lastWasReset = false;

RhythmServo servos[SERVO_NUM] = {RhythmServo(1, beatInterval, 90, Minus), 
                                 RhythmServo(3, beatInterval, 95, Plus),
                                 RhythmServo(5, beatInterval, 90, Plus)};


void servo_reset()
{
    for(int i=0; i<SERVO_NUM;++i)
    {
        servos[i].Reset();
    }
}

void servo_set()
{
    for(int i=0; i<SERVO_NUM;++i)
    {
        servos[i].Set();
    }
}

void servo_update()
{
    unsigned long current_time = millis();
    if(current_time - lastUpdate > beatInterval){
        lastUpdate = current_time;
        beatIndex = (beatIndex+1) % BEAT_LEN;
        if(beatIndex == 0)
            patternIndex = (patternIndex+1) % PATTERN_LEN; 
    }

    for(int i=0; i<SERVO_NUM;++i)
    {
        if(patterns[patternIndex] > 0)
            servos[i].Update(beat1[i][beatIndex]);
        else
            servos[i].Update(beat0[i][beatIndex]);
    }
}

void setup() {
    M5.begin(true, false, true);
    M5.Lcd.println("Super tompy");
    M5.Lcd.println("BtnA: Start/Pause");
    M5.Lcd.println("BtnB: Pause and set base/target position");
    M5.Lcd.println("BtnC: Pause and reset");

    Wire.begin(21, 22, 100000);

    servo_set();
    delay(1000);
}

void display()
{
    unsigned long currentTime = millis();
    if(currentTime - printLastUpdate > 100){
        M5.Lcd.startWrite();

        printLastUpdate = currentTime;
        M5.Lcd.setCursor(0, 190);
        for(int i=0; i<SERVO_NUM;++i)
        {
            M5.Lcd.printf("%d: %+4d, %d, %d, %d\n", servos[i].Pin(), servos[i].Pos(), 
                servos[i].BasePos(), servos[i].TargetAng(), servos[i].RotateDirection());
        }
        if (state == Play)
            M5.Lcd.println("Play ");
        else if(state == Pause)
            M5.Lcd.println("Pause");        
        M5.Lcd.printf("patternIndex: %3d\n", patternIndex);
        M5.Lcd.printf("beatIndex: %3d\n", beatIndex);

        M5.Lcd.endWrite();
    }
}

void input()
{
    M5.update();
    if(M5.BtnA.wasPressed())
    {
        state = State((state + 1) % NumState);
    }
    if(M5.BtnB.wasPressed())
    {
        state = Pause;
        if(lastWasReset)
            servo_set();
        else
            servo_reset();
        lastWasReset = !lastWasReset;
    }
    if(M5.BtnC.wasPressed())
    {
        state = Pause;
        servo_reset();
        beatIndex = 0;
        patternIndex = 0;
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
    display();
    input();
    update();
}