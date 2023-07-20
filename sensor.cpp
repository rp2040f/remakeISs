#include "sensor.h"

void init_sht(SHT31 *sht) {
    pinMode(PIN_VCC_I2C, OUTPUT);
    digitalWrite(PIN_VCC_I2C,HIGH);

    delay(100);

    if (sht->begin(SHT35_ADDRESS)){
        Serial.println("sht is set successfully!");
    }
    else {
        Serial.println("no sht found!");
    }
}

bool get_temp_from_sht(SHT31 *sht,float *temperature)
{       int isReadyTimeout = 20;
        // la fonction isConnected() remplace la variable fixée au démarrage : SHT35_detected
        // WARNING isConnected contacte quel capteur si j'en ai plusieurs ? SHT31-F SHT35 ?
        if (sht->isConnected()) {

            //fait comme mesure unitaire mais le retour data est different !
            //TODO il faudrait une fonction !! ?
            //TODO il faut switcher sur le bon capteur si plusieurs SHT,
            // donc il faut savoir si plusieurs SHT detecté ( variable à lire ou fonction )
            // puis il faut faire un sht->begin (SHT_ADRESS)

            bool success  = false;

            while (!success) {

                sht->requestData();

                //WAIT Ready
                Serial.print("Wait SHT");
                while (!sht->dataReady()){

                    isReadyTimeout--;
                    if ( isReadyTimeout == 0 ){
                        Serial.println("*** WARNING SHT sensor never ready so exit");
                        return false;
                        break;
                    }
                    Serial.print(".");
                    delay(DELAY_W_SHT);
                }

                success  = sht->readData();//WARNING PAS DE CRC PAR defaut

                if (success == false)
                {
                    Serial.println("  \\ WARNING Failed read");
                }
                else
                {
                    *temperature = sht->getTemperature();// * (175.0 / 65535) - 45;
                    return true;
                }
            }
        }
        else
        {
          Serial.println(" >SHT unconnected !");
        }
        return false;

}

bool is_sensorID_valid(int sensorIDs[],int len ,int sensorID) {
    for (int i = 0; i < len; i++)
    {
        if (sensorID == sensorIDs[i])
        {
            
            return true;
        }
        
    }
    return false;
    
}
