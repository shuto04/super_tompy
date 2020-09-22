#include <Arduino.h>
#include <M5Stack.h>
#include <RhythmServo.h>
#include <AnimationServo.h>

#define RHYTHM_SERVO_NUM 3
#define ANIMATION_SERVO_NUM 1

#define BEAT_LEN 16
#define PATTERN_LEN 16

#define KEYFRAME_LEN 8

typedef enum {
    Pause = 0,
    Play,
    NumState
} State;
State state = Pause;

uint8_t beat0[RHYTHM_SERVO_NUM][BEAT_LEN] = {{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                                      {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
                                      {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}};
uint8_t beat1[RHYTHM_SERVO_NUM][BEAT_LEN] = {{1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0},
                                      {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                      {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}};
// uint8_t patterns[PATTERN_LEN] = {0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1};
// uint8_t patterns[PATTERN_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t patterns[PATTERN_LEN] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

int beatIndex = 0;
int patternIndex = 0;

// int beatInterval = (60. / 120.) / 4. * 1000; // 120 bpm -> 2 beat / 1sec -> 500msec / 4つ打ち (500 / 4) = 125
// int beatInterval = (60. / 85.) / 4. * 1000;  // 85 bpm 
// int beatInterval = (60. / 81.4) / 4. * 1000; // 81.4 bpm 
int beatInterval = (60. / 60.) / 4. * 1000;  // 60 bpm
// int beatInterval = (60. / 30.) / 4. * 1000;  // 30 bpm


unsigned long lastUpdate;
unsigned long printLastUpdate;
bool lastWasReset = false;

RhythmServo rhythm_servos[RHYTHM_SERVO_NUM] = {RhythmServo(1, beatInterval, 90, Minus), 
                                 RhythmServo(3, beatInterval, 95, Plus),
                                 RhythmServo(5, beatInterval, 90, Plus)};

KeyFrame keyframes[KEYFRAME_LEN] = { {110, 200}, {90, 200}, {110, 200}, {90, 200}, {120, 100}, {90, 100}, {120, 100}, {90, 100}};
AnimationServo anim_servos[ANIMATION_SERVO_NUM] = {AnimationServo(7, 90, keyframes, KEYFRAME_LEN)};


void servo_reset()
{
    for(int i=0; i<RHYTHM_SERVO_NUM;++i)
    {
        rhythm_servos[i].Reset();
    }
    for(int i=0; i<ANIMATION_SERVO_NUM;++i)
    {
        anim_servos[i].Reset();
    }

}

void servo_set()
{
    for(int i=0; i<RHYTHM_SERVO_NUM;++i)
    {
        rhythm_servos[i].Set();
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

    for(int i=0; i<RHYTHM_SERVO_NUM;++i)
    {
        if(patterns[patternIndex] > 0)
            rhythm_servos[i].Update(beat1[i][beatIndex]);
        else
            rhythm_servos[i].Update(beat0[i][beatIndex]);
    }

    for(int i=0; i<ANIMATION_SERVO_NUM;++i)
    {
        anim_servos[i].Update();
    }
}

void setup() {
    M5.begin(true, false, true);
    M5.Lcd.println("Super tompy");
    M5.Lcd.println("BtnA: Start/Pause");
    M5.Lcd.println("BtnB: Pause and set base/target position");
    M5.Lcd.println("BtnC: Pause and reset");

    Wire.begin(21, 22, 100000);

    servo_reset();
    delay(1000);
}

void display()
{
    unsigned long currentTime = millis();
    if(currentTime - printLastUpdate > 100){
        M5.Lcd.startWrite();

        printLastUpdate = currentTime;
        M5.Lcd.setCursor(0, 150);
        for(int i=0; i<RHYTHM_SERVO_NUM;++i)
        {
            M5.Lcd.printf("%d: %+4d, %d, %d, %d\n", rhythm_servos[i].Pin(), rhythm_servos[i].Pos(), 
                rhythm_servos[i].BasePos(), rhythm_servos[i].TargetAng(), rhythm_servos[i].RotateDirection());
        }
        if (state == Play)
            M5.Lcd.println("Play ");
        else if(state == Pause)
            M5.Lcd.println("Pause");        
        M5.Lcd.printf("patternIndex: %3d, beatIndex: %3d\n", patternIndex, beatIndex);
        M5.Lcd.printf("beatInterval: %3d\n", beatInterval);
        for(int i=0; i<ANIMATION_SERVO_NUM;++i)
        {
            M5.Lcd.printf("%d: %+4d, %d, %d\n", anim_servos[i].Pin(), anim_servos[i].Pos(), 
                anim_servos[i].KeyframeIndex(), anim_servos[i].KeyframeLength());
        }
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