

#ifndef behavior_H_
#define behavior_H_

typedef struct behavior {
    int mode;
    float *data;
    float one_value_data;
    long long int time=-1;
    int timer;
    int period;
    int sensorID;
    bool in_use = false;
    int flag = false;
    bool push_to_graphana = false;
    float min = -1;
    float max = -1;
    bool in_alert_mode = false;
}behavior;


class behaviors {
    private:

    public:

    behavior regular_behaviors[4];
    int nbre_behaviors = 4;
    
    void init_behavior(int sensorID,int period,int timer,int flag,int push_to_grafana,int mode);
    void update_data(int timer,float data);
    void shutdown_behavior(int timer,int mode);
    void allocate_databank(int timer,int nbre)


};


#endif
