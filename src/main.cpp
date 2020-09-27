#include <Arduino.h>
#include <M5Stack.h>
#include <RhythmServo.h>
#include <AnimationServo.h>

#include <CSV_Parser.h>

#define RHYTHM_SERVO_NUM 6
#define ANIMATION_SERVO_NUM 6

#define BEAT_LEN 16
#define PATTERN_LEN 20

#define KEYFRAME_LEN 8

typedef enum {
    Pause = 0,
    Play,
    NumState
} State;
State state = Pause;

const char *pattern_csv_file_path = "/dram_pattern.csv";
const char *pattern_list_csv_file_path = "/dram_pattern_list.csv";

uint8_t **beats = nullptr;
String *servo_names = nullptr;
uint8_t *patterns_list = nullptr;
uint8_t *bpm_list = nullptr;

uint8_t beat0[RHYTHM_SERVO_NUM][BEAT_LEN] = {{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0},
                                             {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}, 
                                            };
uint8_t beat1[RHYTHM_SERVO_NUM][BEAT_LEN] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                             {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                            };
uint8_t beat2[RHYTHM_SERVO_NUM][BEAT_LEN] = {{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                            };
uint8_t beat3[RHYTHM_SERVO_NUM][BEAT_LEN] = {{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                            };
uint8_t beat4[RHYTHM_SERVO_NUM][BEAT_LEN] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
                                             {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                            };
uint8_t beat5[RHYTHM_SERVO_NUM][BEAT_LEN] = {{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
                                             {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                            };
uint8_t beat6[RHYTHM_SERVO_NUM][BEAT_LEN] = {{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                            };
uint8_t beat7[RHYTHM_SERVO_NUM][BEAT_LEN] = {{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                                             {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                            };
uint8_t beat8[RHYTHM_SERVO_NUM][BEAT_LEN] = {{0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0},
                                             {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                            };

// uint8_t patterns[PATTERN_LEN] = {0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1};
// uint8_t patterns[PATTERN_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t patterns[PATTERN_LEN] = {0, 1, 1, 1, 1, 2, 2, 2, 3, 1, 1, 1, 4, 1, 5, 6, 7, 7, 7, 8};

int beatIndex = 0;
int patternIndex = 0;

// int beatInterval = (60. / 120.) / 4. * 1000; // 120 bpm -> 2 beat / 1sec -> 500msec / 4つ打ち (500 / 4) = 125
int beatInterval = (60. / 140) / 4. * 1000;  // 140 bpm 
// int beatInterval = (60. / 85.) / 4. * 1000;  // 85 bpm 
// int beatInterval = (60. / 81.4) / 4. * 1000; // 81.4 bpm 
// int beatInterval = (60. / 60.) / 4. * 1000;  // 60 bpm
// int beatInterval = (60. / 150.) / 4. * 1000;  // 60 bpm
// int beatInterval = (60. / 75.) / 4. * 1000;  // 60 bpm
// int beatInterval = (60. / 30.) / 4. * 1000;  // 30 bpm


unsigned long lastUpdate = 0;
unsigned long printLastUpdate = 0;
bool lastWasReset = false;

RhythmServo rhythm_servos[RHYTHM_SERVO_NUM] = {RhythmServo(1,  beatInterval, 90, Minus), 
                                               RhythmServo(3,  beatInterval, 95, Plus),                                               
                                               RhythmServo(5,  beatInterval, 90, Plus),
                                               RhythmServo(7,  beatInterval, 95, Plus),
                                               RhythmServo(9,  beatInterval, 95, Plus),
                                               RhythmServo(11, beatInterval, 95, Plus)};

KeyFrame keyframes[KEYFRAME_LEN] = { {110, 200}, {90, 200}, {110, 200}, {90, 200}, {120, 100}, {90, 100}, {120, 100}, {90, 100}};
AnimationServo anim_servos[ANIMATION_SERVO_NUM] = {AnimationServo(0, 90, keyframes, KEYFRAME_LEN),
                                                   AnimationServo(2, 90, keyframes, KEYFRAME_LEN),
                                                   AnimationServo(4, 90, keyframes, KEYFRAME_LEN),
                                                   AnimationServo(6, 90, keyframes, KEYFRAME_LEN),
                                                   AnimationServo(8, 90, keyframes, KEYFRAME_LEN),
                                                   AnimationServo(10, 90, keyframes, KEYFRAME_LEN)};

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
    for(int i=0; i<RHYTHM_SERVO_NUM;++i)
    {
        if(patterns[patternIndex] == 1)
            rhythm_servos[i].Update(beat1[i][beatIndex]);
        else if(patterns[patternIndex] == 2)
            rhythm_servos[i].Update(beat2[i][beatIndex]);        
        else if(patterns[patternIndex] == 3)
            rhythm_servos[i].Update(beat3[i][beatIndex]);
        else if(patterns[patternIndex] == 4)
            rhythm_servos[i].Update(beat4[i][beatIndex]);            
        else if(patterns[patternIndex] == 5)
            rhythm_servos[i].Update(beat5[i][beatIndex]);                        
        else if(patterns[patternIndex] == 6)
            rhythm_servos[i].Update(beat6[i][beatIndex]);                                    
        else if(patterns[patternIndex] == 7)
            rhythm_servos[i].Update(beat7[i][beatIndex]);                                    
        else if(patterns[patternIndex] == 8)
            rhythm_servos[i].Update(beat8[i][beatIndex]);                                                
        else
            rhythm_servos[i].Update(beat0[i][beatIndex]);
    }

    for(int i=0; i<ANIMATION_SERVO_NUM;++i)
    {
        anim_servos[i].Update();
    }

    unsigned long current_time = millis();
    if(current_time - lastUpdate > beatInterval){
        lastUpdate = current_time;
        beatIndex = (beatIndex+1) % BEAT_LEN;
        if(beatIndex == 0)
            patternIndex = (patternIndex+1) % PATTERN_LEN; 
    }
}

void setup() {
    M5.begin(true, true, true);

    {
        File csv_file = SD.open(pattern_csv_file_path);
        CSV_Parser cp("sL", false, ',');
        if(!csv_file){
            M5.Lcd.println("Failed to open csv file.");
            while(1);
        }
        while(csv_file.available()){
            cp << csv_file.read();
        }
        csv_file.close();
        cp.parseLeftover();

        beats = new uint8_t*[cp.getRowsCount()];
        servo_names = new String[cp.getRowsCount()];
        char **values = (char**)cp[0];        
        for(int row = 0; row < cp.getRowsCount(); row++)
        {
            beats[row] = new uint8_t[cp.getColumnsCount()-1];
            servo_names[row] = (uint8_t)(atoi(values[row]));
        }
        for(int col = 1; col < cp.getColumnsCount(); col++){
            values = (char**)cp[col-1];
            for(int row = 0; row < cp.getRowsCount(); row++)
            {
                beats[row][col-1] = (uint8_t)(atoi(values[row]));
            }
        }
    }
    {
        File csv_file = SD.open(pattern_list_csv_file_path);
        CSV_Parser cp("sL", false, ',');
        if(!csv_file){
            M5.Lcd.println("Failed to open csv file.");
            while(1);
        }
        while(csv_file.available()){
            cp << csv_file.read();
        }
        csv_file.close();
        cp.parseLeftover();
        cp.print();
        patterns_list = new uint8_t[cp.getColumnsCount()-1];
        bpm_list = new uint8_t[cp.getColumnsCount()-1];        
        for(int col = 1; col < cp.getColumnsCount(); col++){
            char **values = (char**)cp[col-1];
            patterns_list[col-1] = (uint8_t)(atoi(values[0]));
            bpm_list[col-1] = (uint8_t)(atoi(values[1]));
        }
    }

    M5.Lcd.println("Finished to load file.");

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
    // display();
    // input();
    // update();
}