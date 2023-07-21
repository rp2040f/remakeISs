

#ifndef behavior_H_
#define behavior_H_

typedef struct behavior {
    float data[1325];
    float one_value_data;
    long long int time=-1;
    int timer;
    int period;
    int sensorID;
    bool in_use = false;
    int flag = false;
    bool push_to_graphana = false;
    bool in_alert_mode = false;
    float min = -1;
    float max = -1;
}behavior;


class behaviors {
    private:

    public:

    behavior regular_behaviors[4];
    int nbre_behaviors = 4;
    
    void init_behavior(int sensorID,int period,int timer,int flag,int push_to_grafana);
    void update_data(int timer,float data);
    void shutdown_behavior(int timer);

    void init_alert(int timer,float min , float max);


};


#endif