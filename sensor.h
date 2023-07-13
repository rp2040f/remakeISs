#ifndef sensor_H_
#define sensor_H_

#include "IO.h"
#include "SHT31.h"
#include <Wire.h>

#define SHT35_ADDRESS 0x45
#define DELAY_W_SHT 2

static int sensorIDs[] = {69};
static int sensorIDs_len = 1;
bool is_sensorID_valid(int sensorIDs[],int len,int sensorID);

//capteur de temperature
void init_sht(SHT31 *sht);
bool get_temp_from_sht(SHT31 *sht,float *temperature);


#endif