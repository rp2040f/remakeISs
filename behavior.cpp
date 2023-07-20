#include "behavior.h"

#include <LittleFS.h>

void behaviors::init_behavior(int sensorID,int period,int timer,int flag,int push_to_graphana) {
    regular_behaviors[timer].in_use = true;
    regular_behaviors[timer].flag = flag;
    regular_behaviors[timer].period = period;
    regular_behaviors[timer].sensorID = sensorID;
    regular_behaviors[timer].push_to_graphana = push_to_graphana;
    
}

void behaviors::update_data(int timer,float data){
    regular_behaviors[timer].one_value_data = data;
}

void behaviors::shutdown_behavior(int timer){
    regular_behaviors[timer].in_use = false;
    Serial.print("timer shutdown behave:");
    Serial.println(timer);
}

