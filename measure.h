#ifndef Measure_H
#define Measure_H

#include <RPi_Pico_TimerInterrupt.h>
#include "basic_funcs.h"


#include "sensor.h"

// This is the common delay to wait for SHT sensor ready
#define DELAY_W_SHT 2


typedef struct Measure{

    float data=-1;
    long long int time=-1;
    int timer;
    int period;
    int sensorID;
    bool in_use = false;
    bool flag = false;
    bool push_to_graphana = false;
    bool in_alert_mode = false;
    float min;
    float max;

}Measure;

//those are the 4 bools that are responsable for triggering a measure , volatile because interrutptions needto manipulate on volatile variables
static volatile bool measure_activated[4] = {false,false,false,false};

class measures {
    private:

    public:

    Measure regular_measures[4];
    int nbre_measures = 4;

    int init_regular_measure(int period,int sensorID,int push_to_graphana);
    int get_mode_regular_measure();
    int check_not_used_measure();

    bool timers[4] = {false,false,false,false}; //keep track of used timers , true if timer is already in use


    static bool Timer0Handler(struct repeating_timer *t);
    static bool Timer1Handler(struct repeating_timer *t);
    static bool Timer2Handler(struct repeating_timer *t);
    static bool Timer3Handler(struct repeating_timer *t);
    
    //always check if a measure is activated and set it off after execution , the measure is activated when the ITimer call the handler function
    void check_measure_activated(SHT31 *sht);

    //clear measure
    int get_mode_clear_measure();
    int clear_measure(int interruption,int flag_Reset);

    //listen for incomming comands via FIFO
    void get_command_from_other_core();

    void send_info_to_other_core(int mode, Measure *mes);

    int set_alert(int timer);

    int get_mode_regular_measure_alert();
    
};

static RPI_PICO_Timer ITimer0(0);
static RPI_PICO_Timer ITimer1(1);
static RPI_PICO_Timer ITimer2(2);
static RPI_PICO_Timer ITimer3(3);
static RPI_PICO_Timer ITimers[4] = {ITimer0,ITimer1,ITimer2,ITimer3};

#endif

