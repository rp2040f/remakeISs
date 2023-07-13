#include "networkIS.h"

int request::networkCheck() {

    //This use Leds as visual indicator for the network status.
    
    //In case of internal shield issue
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("Ethernet shield was not found.");
        digitalWrite(LINK_STAT_LED, HIGH);
        digitalWrite(ONBOARD_LED, LOW);
        
        return -1;
    }

    //In case of network shutdown
    else if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Ethernet cable is not connected.");
        digitalWrite(LINK_STAT_LED, HIGH);
        digitalWrite(ONBOARD_LED, LOW);
        return -2;
    } else {
        digitalWrite(LINK_STAT_LED, LOW);
        digitalWrite(ONBOARD_LED, HIGH);
        return 0;

    }
}

void request::networkSetup() {

    //The CS pin of the Ethernet module is GPIO17
    Ethernet.init(17);
    delay(1000);
    
    delay(3000);//TODO REMOVE AFTER DEBUG

    Serial.println("Using the Default MAC");

    Ethernet.begin(Default_mac, own_ip);
    delay(1000);

    //At this point the IS is connected to the network
    core_debug("set up");
    // IPAddress gateway(192, 168, 10, 1); //Only for local switch network
    // Ethernet.setGatewayIP(gateway);
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
    Serial.print("My gateway address: ");
    Serial.println(Ethernet.gatewayIP());
    Serial.print("My DNS address: ");
    Serial.println(Ethernet.dnsServerIP());
    

    server.begin();

}

void request::try_connect_to_server() {
    Serial.println(client.connect(server_ip,80));
    if (client.connect(server_ip,80))
    {
        Serial.println("connected to server via port 80");
    }
    else {
        Serial.println("connexion to server via port 80 failed :/");
    }
    
}

void request::sendTime() {

    String Type = "GET";
    String Http_Level = "HTTP/1.1";

    //WARNING il faut une requete avec reponse directe tres courte sinon cela peut entraver la partie Timer
    // et plus de mesure ne peuvent être initiées!!!! LONG BUG
    //   ISensor_alertRequest.sh //original used script WARNING OLD WAY too slow
    String time = (String) millis();
    String URL_Body_sh = "/cgi-bin/get_mydata.sh?time=" + time ;

    String FullReqURL = Type + " " + URL_Body_sh + " " + Http_Level;

    if (client.connect(server_ip, 80)) {
        Serial.println("connected");
        client.println(FullReqURL);
        client.print("Host: ");
        client.println(server_ip);
        client.println("Connection: close");
        client.println();
        client.flush();
        read_data_back();
    } 
    else {
        Serial.println("connection failed");
    }
}


void request::read_data_back() {
    Serial.println("read_data starting ...");

    int count_lines = 0;
    int json_start = 0;
    String date = "";
    String json_str = "";
    int timeout_duree = 0;

    while (!client.available() && timeout_duree < 1000) {
        Serial.print(".");
        timeout_duree++;
        delay(1);
    }
    if (!client.available()) {
        Serial.println("no response from server under 1s , Too slow!!");
    }
    else {
        while (client.available()) {
            
            char c = client.read();

            if (c == '\n')
            {
                count_lines ++;
            }
            
            if (count_lines == DATE_RESPONSE_LINE ){ // la date correspond a ligne 5 pour lighttd et ligne 2 pour apache
                date = date + c;
            }
            
            if (c == '{') {
                json_start ++;
            }

            if (json_start != 0) {
                json_str = json_str + c;
            }

            if (c == '}') {
                json_start --;
            }
        }
        Serial.println(date);
        Serial.println(json_str);
        Serial.println();
    }
    Serial.flush();
    rdisconnect();

}

void request::rdisconnect() {
    if (!client.connected()) {
        // read_data = false;
        Serial.println("disconnecting.");
        client.stop();

  }
}

void request::get_request() {
    handleRoot();
    client.stop();
}

void request::process_command(DynamicJsonDocument jsonDoc) {
    if (jsonDoc["M"] == 1) //regular measure
    {
        write_to_other_core(jsonDoc["M"]);
        write_to_other_core(jsonDoc["P"]);
        write_to_other_core(jsonDoc["sensorID"]);
        
    }
    if (jsonDoc["M"] == 9) // unitary measure
    {
        write_to_other_core(jsonDoc["M"]);
        write_to_other_core(jsonDoc["sensorID"]);
    }
    
    if (jsonDoc["M"] == 11) //clear meausre
    {
        write_to_other_core(jsonDoc["M"]);
        write_to_other_core(jsonDoc["I"]);
        write_to_other_core(jsonDoc["D"]);
    }
    

}

void request::handleRoot() {
    client = server.available();
    if (client) {
        // Wait for a request to be available
        Serial.println("got it!");
        if (client.connected()) {
        // Read the GET request
        String request = "";
        while (client.connected()) {
            if (client.available()) {
            char c = client.read();
            request += c;
            if (c == '\n') {
                if (request.startsWith("GET")) {
                
                // Parse the query parameters
                int paramStart = request.indexOf('?');
                
                if (paramStart != -1) {
                    int paramEnd = request.indexOf(' ', paramStart);
                    if (paramEnd != -1) {
                    String query = request.substring(paramStart + 1, paramEnd);
                    
                    // Create a JSON object
                    DynamicJsonDocument jsonDoc(1024);
                    
                    // Split the query parameters
                    int paramCount = 0;
                    while (query.length() > 0) {
                        int separatorIndex = query.indexOf('&');
                        String param;
                        if (separatorIndex != -1) {
                        param = query.substring(0, separatorIndex);
                        query.remove(0, separatorIndex + 1);
                        } else {
                        param = query;
                        query = "";
                        }
                        // Extract parameter name and value
                        int equalsIndex = param.indexOf('=');
                        if (equalsIndex != -1) {
                        String paramName = param.substring(0, equalsIndex);
                        String paramValue = param.substring(equalsIndex + 1);
                        jsonDoc[paramName] = paramValue.toInt();

                        core_print_uint32(jsonDoc[paramName]);
                        paramCount++;
                        }
                    }

                    if (paramCount > 0) {
                        // Convert the JSON object to a string
                        String jsonString;
                        
                        serializeJson(jsonDoc, jsonString);

                        process_command(jsonDoc);

                        // Set the content type of the response to application/json
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: application/json");
                        client.println();
                        // Send the JSON response
                        client.println(jsonString);
                    }
                    }
                }
                }
                break;
            }
            }
        }
        client.stop();
        }
    }
}



