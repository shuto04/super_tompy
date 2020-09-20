#include <Arduino.h>
#include <M5Stack.h>
#include <RhythmServo.h>

#define SERVO_NUM 3

#define BEAT_LEN 16

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
// int beatInterval = 176; // 85 bpm
int beatInterval = 250; // 85 bpm
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
        servos[i].Update(beats[i][beatIndex]);
    }
}

void setup() {
    M5.begin(true, false, true);
    M5.Lcd.println("Super tompy");
    M5.Lcd.println("Start by Left button");

    Wire.begin(21, 22, 100000);

    servo_reset();
}

void display()
{
    unsigned long currentTime = millis();
    if(currentTime - printLastUpdate > 100){
        printLastUpdate = currentTime;
        M5.Lcd.setCursor(0, 190);
        for(int i=0; i<SERVO_NUM;++i)
        {
            M5.Lcd.printf("%d: %+4d, %d, %d, %d\n", servos[i].Pin(), servos[i].Pos(), 
                servos[i].BasePos(), servos[i].TargetAng(), servos[i].RotateDirection());
        }
    }
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
    display();
    input();
    update();
}