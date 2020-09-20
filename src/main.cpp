#include <Arduino.h>
#include <M5Stack.h>
#include <RhythmServo.h>

#define SERVO_NUM 3

#define BEAT_LEN 16

typedef enum {
    PauseWithBase = 0,
    PauseWithTarget,
    Play,
    NumState
} State;
State state = PauseWithBase;

uint8_t beats[SERVO_NUM][BEAT_LEN] = {{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                                      {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
                                      {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}};
int beatIndex = 0;
// int beatInterval = 125; // 120 bpm -> 2 beat / 1sec -> 500msec / 4つ打ち (500 / 4) = 125
int beatInterval = 176; // 85 bpm
// int beatInterval = 250; // 60 bpm
// int beatInterval = 500; // 30 bpm
unsigned long lastUpdate;
unsigned long printLastUpdate;

RhythmServo servos[SERVO_NUM] = {RhythmServo(1, beatInterval, 100, Minus), 
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
    }

    for(int i=0; i<SERVO_NUM;++i)
    {
        servos[i].Update(beats[i][beatIndex]);
    }
}

void setup() {
    M5.begin(true, false, true);
    M5.Lcd.println("Super tompy");
    M5.Lcd.println("Start by Left button");

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
        M5.Lcd.setCursor(0, 190);
        for(int i=0; i<SERVO_NUM;++i)
        {
            M5.Lcd.printf("%d: %+4d, %d, %d, %d\n", servos[i].Pin(), servos[i].Pos(), 
                servos[i].BasePos(), servos[i].TargetAng(), servos[i].RotateDirection());
        }
        if (state == Play)
            M5.Lcd.println("Play           \n");
        else if(state == PauseWithBase)
            M5.Lcd.println("PauseWithBase  \n");
        else if(state == PauseWithTarget)
            M5.Lcd.println("PauseWithTarget\n");        

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
}

void update()
{
    if (state == Play)
    {
        servo_update();
    }else if(state == PauseWithBase)
    {
        servo_reset();
    }else if(state == PauseWithTarget)
    {
        servo_set();
    }
}

void loop()
{
    display();
    input();
    update();
}