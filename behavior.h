

#ifndef behavior_H_
#define behavior_H_

typedef struct behavior {
    float data[1325];
    long long int time=-1;
    int timer;
    int period;
    int sensorID;
    bool in_use = false;
    int flag = false;
}


class behaviors {
    private:

    public:

    behavior regular_behaviors[4];
    int nbre_behaviors = 5;

    void init_behavior(int sensorID,int period,int timer,int flag);


};


#endif