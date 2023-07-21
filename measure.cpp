#include "measure.h"

int measures::init_regular_measure(int period,int sensorID,int push_to_grafana) {
    int free_spot = check_not_used_measure();
    if (free_spot == -1)
    {
        Serial.println("max number of measure");
        return -1;
    }

    Serial.println("found free spot measure number"+(String)free_spot);
    regular_measures[free_spot].period = period;
    regular_measures[free_spot].sensorID = sensorID;
    regular_measures[free_spot].push_to_graphana = (bool)push_to_grafana;
    regular_measures[free_spot].flag = false;
    
    if (free_spot == 0 && !timers[0])
    {
        if (ITimers[0].attachInterruptInterval(  period*1000, Timer0Handler)) {
            Serial.println("Starting ITimer 0 OK, millis() = " +String(millis()));
            regular_measures[free_spot].in_use = true;
            regular_measures[free_spot].timer = 0;
            return free_spot;
        }
        else{
            Serial.println("Can't set ITimer 0. Select another freq. or timer");
            return -2;

        }
    }
    if (free_spot == 1 && !timers[1])
    {
        if (ITimers[1].attachInterruptInterval(  period*1000, Timer1Handler)) {
            Serial.println("Starting ITimer 1 OK, millis() = " +String(millis()));
            regular_measures[free_spot].in_use = true;
            regular_measures[free_spot].timer = 1;
            return free_spot;
        }
        else{
            Serial.println("Can't set ITimer 1. Select another freq. or timer");
            return -2;

        }
    }
    if (free_spot == 2 && !timers[2])
    {
        if (ITimers[2].attachInterruptInterval(  period*1000, Timer2Handler)) {
            Serial.println("Starting ITimer 2 OK, millis() = " +String(millis()));
            regular_measures[free_spot].in_use = true;
            regular_measures[free_spot].timer = 2;
            return free_spot;
        }
        else{
            Serial.println("Can't set ITimer 2. Select another freq. or timer");
            return -2;

        }
    }
    if (free_spot == 3 && !timers[3])
    {
        if (ITimers[3].attachInterruptInterval(  period*1000, Timer3Handler)) {
            Serial.println("Starting ITimer OK 3, millis() = " +String(millis()));
            regular_measures[free_spot].in_use = true;
            regular_measures[free_spot].timer = 3;
            return free_spot;
        }
        else{
            Serial.println("Can't set ITimer 3. Select another freq. or timer");
            return -2;

        }
    }
    
    return -3;
}

bool measures::Timer0Handler(struct repeating_timer *t)
{
    core_debug((char *)"interruption !!!! ");
    Serial.println();
    measure_activated[0] = true;
    return measure_activated[0];
}

bool measures::Timer1Handler(struct repeating_timer *t)
{
    core_debug((char *)"interruption 1 !!!! ");
    Serial.println();
    measure_activated[1] = true;
    return measure_activated[1];
}

bool measures::Timer2Handler(struct repeating_timer *t)
{
    core_debug((char *)"interruption 2 !!!! ");
    Serial.println();
    measure_activated[2] = true;
    return measure_activated[2];
}

bool measures::Timer3Handler(struct repeating_timer *t)
{
    core_debug((char *)"interruption 3 !!!! ");
    Serial.println();
    measure_activated[3] = true;
    return measure_activated[3];
}



void measures::check_measure_activated(SHT31 *sht){
    for (int i = 0; i < 4; i++)
    {
        if (measure_activated[i]) {
            if (get_temp_from_sht(sht,&regular_measures[i].data)) {
                    
                    core_debug((char *)"measure\n");
                    Serial.println("temp measured:"+(String)regular_measures[i].data);
                    send_info_to_other_core(1,&regular_measures[i]);
            }        
            else {
                    Serial.println("failed to read temperature!");
            }
            measure_activated[i] = false;
        } 
    }
}

void measures::get_command_from_other_core() {
    uint32_t mode;
    if (read_from_other_core(&mode)) {
        if (mode == 1)
        {
            int mes_num = get_mode_regular_measure();
            if (mes_num >= 0)
            {
                send_info_to_other_core(8,&regular_measures[mes_num]);
                core_debug((char *)"regular init sent to other core\n");
            }
            else {
                core_debug((char *)"failed to send regular init params to other core!\n");
            }
            
        }
        if (mode == 11)
        {
            int mes_num = get_mode_clear_measure();
            Serial.println(mes_num);
            send_info_to_other_core(11,&regular_measures[mes_num]);
            Serial.println(regular_measures[mes_num].timer);
        }

        if (mode == 2)
        {
            int mes_num = get_mode_regular_measure_alert();
            if (mes_num >= 0)
            {
                send_info_to_other_core(8,&regular_measures[mes_num]);
                send_info_to_other_core(2,&regular_measures[mes_num]);
                core_debug((char *)"regular init with init sent to other core\n");
            }
            else {
                core_debug((char *)"failed to send regular init params to other core!\n");
            }
        }
        
          
    }

}

int measures::get_mode_regular_measure() {
    uint32_t period, sensorID,push_to_grafana;
    int timeout = 10;
    while (timeout && !read_from_other_core(&period))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("period not found");
        return -1;
    }

    while (timeout && !read_from_other_core(&sensorID))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("sensorID not found");
        return -2;
    }
    while (timeout && !read_from_other_core(&push_to_grafana))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("push_to_grafana param not found");
        return -1;
    }

    if (period == 0)
    {
        Serial.println("period value not correct");
        return -3;
    }

    if (!is_sensorID_valid(sensorIDs,sensorIDs_len,sensorID))
    {
        Serial.println("Wrong sensorID");
        return -4;
    }

    if (push_to_grafana != 0 && push_to_grafana != 1)
    {
        Serial.println("push_to_grafana is wrong <> 0 1");
        return -5;
    }
    
    int output = init_regular_measure(period,sensorID,push_to_grafana);
    
    return output; 
}

int measures::get_mode_regular_measure_alert() {
    uint32_t period, sensorID,push_to_grafana,min,max;
    int timeout = 10;
    while (timeout && !read_from_other_core(&period))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("period not found");
        return -1;
    }

    while (timeout && !read_from_other_core(&sensorID))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("sensorID not found");
        return -2;
    }
    while (timeout && !read_from_other_core(&push_to_grafana))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("push_to_grafana param not found");
        return -1;
    }
    while (timeout && !read_from_other_core(&min))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("min param not found");
        return -1;
    }
    while (timeout && !read_from_other_core(&max))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("max param not found");
        return -1;
    }

    if (period == 0)
    {
        Serial.println("period value not correct");
        return -3;
    }

    if (!is_sensorID_valid(sensorIDs,sensorIDs_len,sensorID))
    {
        Serial.println("Wrong sensorID");
        return -4;
    }

    if (push_to_grafana != 0 && push_to_grafana != 1)
    {
        Serial.println("push_to_grafana is wrong <> 0 1");
        return -5;
    }
    if (min == max)
    {
        Serial.println("min is equal to max");
        return -6;
    }

    int timer = init_regular_measure(period,sensorID,push_to_grafana);
    if (timer >= 0 )
    {
        int outupt = set_alert(timer,float min,float max);
    }
    //TODO finish the alert , checkpoint already get measure alert params and must init min and max in measure struct
    else {
        return -1;
        core_debug("failed to init regular alert");
    }
    return output; 
}

int measures::set_alert(int timer) {
    regular_measures[timer].min = min;
    regular_measures[timer].max = max;
}

int measures::check_not_used_measure() {
    for (int i = 0; i < nbre_measures; i++)
    {
        if (!regular_measures[i].in_use)
        {
            return i;
        }
        
    }
    return -1;
    
}

int measures::get_mode_clear_measure() {
    uint32_t interruption;
    uint32_t flag_reset;
    int timeout = 10;
    while (timeout && !read_from_other_core(&interruption))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("interruption not found");
        return -1;
    }

    while (timeout && !read_from_other_core(&flag_reset))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("flag_Reset not found");
        return -2;
    }

    if (interruption > 3 )
    {
        Serial.println("interruption value not correct");
        return -3;
    }

    if (flag_reset != 0 && flag_reset != 1)
    {
        Serial.println("wrong flag "+(String)flag_reset+ "reset! <> 0 1");
        return -4;
    }
    

    int output = clear_measure(interruption,flag_reset);
    Serial.println(output);

    return output; 
}

int measures::clear_measure(int interruption,int flag_Reset) {
    if (flag_Reset)
    {
        regular_measures[interruption].flag = false;
        core_debug((char*)"flag is reset!");    
    }
    else {
        ITimers[interruption].detachInterrupt();
        regular_measures[interruption].in_use = false;
        Serial.println("timer "+(String)interruption + " is sleepy now");
    }
    return interruption;
    
}

void measures::send_info_to_other_core(int mode,Measure *mes) {
    if (mode == 8) //thid mode is added to inform the other core that a regular measure is intitialized
    {
        write_to_other_core(8);
        write_to_other_core((uint32_t)mes->sensorID);
        write_to_other_core((uint32_t)mes->period);
        write_to_other_core((uint32_t)mes->timer);
        write_to_other_core((uint32_t)mes->flag);
        write_to_other_core((uint32_t)mes->push_to_graphana);

    }
    if (mode == 1)
    {
        write_to_other_core(1);
        write_to_other_core((uint32_t)mes->timer);
        write_to_other_core(floatToUint32(mes->data));
        write_to_other_core(uint32_t(mes->in_alert_mode));
    }
    if (mode == 11)
    {
        write_to_other_core(11);
        write_to_other_core((uint32_t)mes->timer);
        write_to_other_core((uint32_t)mes->flag);
    }
    if ( mode == 2)
    {
        write_to_other_core(2);
        write_to_other_core(mes->timer);
        write_to_other_core(uint32_t(mes->in_alert_mode));
        write_to_other_core(floatToUint32(mes->min));
        write_to_other_core(floatToUint32(mes->max));
    }
       
}


