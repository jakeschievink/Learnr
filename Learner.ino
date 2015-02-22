
#include "pitches.h"
#include <ChibiOS_AVR.h>

static WORKING_AREA(waTh1, 100);
static WORKING_AREA(waTh2, 100);
static WORKING_AREA(waTh3, 100);
#define SPEAK_PIN 13
#define POWERCONTROL_PIN 12
#define PRESSURE_PIN A0
#define BLUE_PIN 11
#define GREEN_PIN 10
#define RED_PIN 9

boolean grabbed = false;

msg_t powercontrolThread(void  *args) {
    while(1){
        chThdSleep(random(4000));
        digitalWrite(POWERCONTROL_PIN, 0);
        chThdSleep(random(4000));
    }
}

msg_t blinkerThread(void  *args) {
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);

    while(1){
        if(!grabbed){
            analogWrite(BLUE_PIN, random(255));
            chThdSleep(100);
            analogWrite(BLUE_PIN, random(255));
            chThdSleep(100);
            digitalWrite(GREEN_PIN, 0);
        }else{
            int displayVal = map(analogRead(PRESSURE_PIN), 10, 1000, 0, 255);
            analogWrite(GREEN_PIN, displayVal);
            digitalWrite(BLUE_PIN, 0);
        }
    }
}
msg_t mainThread(void  *args) {
    pinMode(PRESSURE_PIN, INPUT);
    pinMode(POWERCONTROL_PIN, OUTPUT);
    int initialPressure = analogRead(PRESSURE_PIN);

    while(1){
        int pressureReading = analogRead(PRESSURE_PIN);

        if(pressureReading > initialPressure+400){
            static int newspeakerVal = 0;
            grabbed = true;
            int speakerVal = map(analogRead(PRESSURE_PIN), 0, 1000, 500, 1000);
            tone(SPEAK_PIN, speakerVal, 3000);
            newspeakerVal = (map(analogRead(PRESSURE_PIN), 0, 1000, 500, 1000)+newspeakerVal)/2;
            if(pressureReading > initialPressure+900){
                digitalWrite(POWERCONTROL_PIN, 1);
            }
        }else{
            grabbed = false;
            noTone(SPEAK_PIN);
        }
    }
}

void setup() {
    Serial.begin(9600);
    chBegin(chSetup);
    while(1);
}

void chSetup(){
    chThdCreateStatic(waTh1, sizeof(waTh1), NORMALPRIO, blinkerThread, NULL);
    chThdCreateStatic(waTh2, sizeof(waTh2), NORMALPRIO, mainThread, NULL);
    chThdCreateStatic(waTh3, sizeof(waTh3), NORMALPRIO, powercontrolThread, NULL);
}

void loop() {}
