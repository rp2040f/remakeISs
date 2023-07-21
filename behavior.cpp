#include "behavior.h"

#include <LittleFS.h>

void behaviors::init_behavior(int sensorID,int period,int timer,int flag,int push_to_graphana,int mode) {
    regular_behaviors[timer].in_use = true;
    regular_behaviors[timer].flag = flag;
    regular_behaviors[timer].period = period;
    regular_behaviors[timer].sensorID = sensorID;
    regular_behaviors[timer].push_to_graphana = push_to_graphana;
    regular_behaviors[timer].mode = mode;
    
}

void behaviors::update_data(int timer,float data){
    regular_behaviors[timer].one_value_data = data;
}

void behaviors::shutdown_behavior(int timer, int mode){
    regular_behaviors[timer].in_use = false;
    regular_behaviors[timer].in_alert_mode = false;
    regular_behaviors[timer].push_to_graphana = false;
    regular_behaviors[timer].flag = false'
    Serial.print("timer shutdown behave:");
    Serial.println(timer);
    if (regular_behaviors[timer].data) {
        free(regular_behaviors[timer].data);
    }
}

void behaviors::init_alert(int timer,float min , float max) {
    regular_behaviors[timer].min = min;
    regular_behaviors[timer].max = max;
}

