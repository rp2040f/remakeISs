
#include "networkIS.h"
#include "measure.h"


// #include <Ethernet_Generic.h>
request Request;
measures Measures;

SHT31 sht;

void setup1()
{
    //Serial.begin(9600);

    pinMode(LINK_STAT_LED, OUTPUT);
    pinMode(ONBOARD_LED, OUTPUT);

    Request.networkSetup();
}

void setup(){
    Serial.begin(115200);
    
    delay(5000);
    init_sht(&sht);
    
}


void loop1()
{
    Request.networkCheck();
    Request.get_request();
}

//core 0 is responsable for managing interrupts cuz it seems that timeInterruptInterval works only with core 0
void loop() {
    
    Measures.get_command_from_other_core();
    Measures.check_measure_activated(&sht);
}
